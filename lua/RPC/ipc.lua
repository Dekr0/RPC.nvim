local logger = require("RPC.logger")
local pack = require("pack")
local uuid = require("uuid")

local n = "RPC.ipc"

local client_id = "1222702873440157796"

---@enum CMD
local CMD = {
    DISPATCH                       = "DISPATCH",
    AUTHORIZE                      = "AUTHORIZE",
    AUTHENTICATE                   = "AUTHENTICATE",
    GET_GUILD                      = "GET_GUILD",
    GET_GUILDS                     = "GET_GUILDS",
    GET_CHANNEL                    = "GET_CHANNEL",
    GET_CHANNELS                   = "GET_CHANNELS",
    SUBSCRIBE                      = "SUBSCRIBE",
    UNSUBSCRIBE                    = "UNSUBSCRIBE",
    SET_USER_VOICE_SETTINGS        = "SET_USER_VOICE_SETTINGS",
    SELECT_VOICE_CHANNEL           = "SELECT_VOICE_CHANNEL",
    GET_SELECTED_VOICE_CHANNEL     = "GET_SELECTED_VOICE_CHANNEL",
    SELECT_TEXT_CHANNEL            = "SELECT_TEXT_CHANNEL",
    GET_VOICE_SETTINGS             = "GET_VOICE_SETTINGS",
    SET_VOICE_SETTINGS             = "SET_VOICE_SETTINGS",
    SET_CERTIFIED_DEVICES          = "SET_CERTIFIED_DEVICES",
    SET_ACTIVITY                   = "SET_ACTIVITY",
    SEND_ACTIVITY_JOIN_INVITE      = "SEND_ACTIVITY_JOIN_INVITE",
    CLOSE_ACTIVITY_REQUEST         = "CLOSE_ACTIVITY_REQUEST"
}

---@enum Event
local Event = {
    READY                          = "READY",
    ERROR                          = "ERROR",
    GUILD_STATUS                   = "GUILD_STATUS",
    GUILD_CREATE                   = "GUILD_CREATE",
    CHANNEL_CREATE                 = "CHANNEL_CREATE",
    VOICE_CHANNEL_SELECT           = "VOICE_CHANNEL_SELECT",
    VOICE_STATE_CREATE             = "VOICE_STATE_CREATE",
    VOICE_STATE_UPDATE             = "VOICE_STATE_UPDATE",
    VOICE_STATE_DELETE             = "VOICE_STATE_DELETE",
    VOICE_SETTINGS_UPDATE          = "VOICE_SETTINGS_UPDATE",
    VOICE_CONNECTION_STATUS        = "VOICE_CONNECTION_STATUS",
    SPEAKING_START                 = "SPEAKING_START",
    SPEAKING_STOP                  = "SPEAKING_STOP",
    MESSAGE_CREATE                 = "MESSAGE_CREATE",
    MESSAGE_UPDATE                 = "MESSAGE_UPDATE",
    MESSAGE_DELETE                 = "MESSAGE_DELETE",
    NOTIFICATION_CREATE            = "NOTIFICATION_CREATE",
    ACTIVITY_JOIN                  = "ACTIVITY_JOIN",
    ACTIVITY_SPECTATE              = "ACTIVITY_SPECTATE",
    ACTIVITY_JOIN_REQUEST          = "ACTIVITY_JOIN_REQUEST"
}

-- Types of binary message
---@enum Opcode
local Opcode = {
    Handshake = 0,
    Frame     = 1,
    Close     = 2,
    Ping      = 3,
    Pong      = 4
}

---@class Payload
---@field args      table   | nil
---@field client_id string  | nil
---@field cmd       string  | nil
---@field data      table   | nil
---@field evt       string  | nil
---@field nonce     string  | nil
---@field v         integer | nil
local Payload = {}

---@class IPC
---@field connected        boolean
---@field handshake_finish boolean
---@field logger           Logger
---@field pipe             uv_pipe_t
local IPC = {}
IPC.__index = IPC

-- Discord can be on any pipe ranging from `discord-ipc-0` to `discord-ipc-9`.
-- Try and connect to each one. Keep the first one successfully connected.
---@param ipc_path string
function IPC:new(ipc_path)
    local instance = setmetatable({
        connected        = false,
        handshake_finish = false,
        pipe             = vim.loop.new_pipe(false)
    }, self)

    instance.pipe:connect(ipc_path, function (reason)
       assert(not reason, reason)
       instance.connected = true
       vim.schedule(function () instance:handshake() end)
    end)

    return instance
end

---@param req_opcode integer 
---@param chunk  string | nil 
function IPC:read(req_opcode, chunk)
    assert(req_opcode >= Opcode.Handshake
        and req_opcode <= Opcode.Pong, req_opcode)

    if not chunk then return end

    local res_opcode_chunk = string.sub(chunk, 1, 4)
    local res_length_chunk = string.sub(chunk, 5, 8)
    local data = string.sub(chunk, 9)

    local res_opcode = pack.unpack_uint32(res_opcode_chunk, 1)
    local res_length = pack.unpack_uint32(res_length_chunk, 1)

    assert(res_opcode >= Opcode.Handshake
        and res_opcode <= Opcode.Pong, res_opcode)

    local metainfo = string.format(
        "Response Length: %d; "
        .."Response Opcode: %d; "
        .."Response JSON Length (from Header): %d; "
        .."Response JSON Length (from Lua String Length): %d",
        #chunk, res_opcode, res_length, #data)

    logger:l(20, n, "IPC:read", metainfo)
    logger:l(20, n, "IPC:read", chunk)

    assert(res_length == #data,
        "JSON length miss match with the header provided length")

    if (res_opcode == Opcode.Frame) then
        vim.schedule(function ()
            self:parse_frame(req_opcode, data)
        end)
    end
end

---@param data string
function IPC:parse_frame(req_opcode, data)
    ---@type Payload | nil
    local json_data = vim.fn.json_decode(data)
    assert(json_data, "Nil JSON data")

    if (req_opcode == Opcode.Handshake) then
        assert(json_data.cmd == CMD.DISPATCH)
        assert(json_data.evt == Event.READY,
            "Failed to establish a Discord RPC handshake")
        self.handshake_finish = true
    end
end

---@param req_opcode      integer 
---@param t               Payload 
function IPC:write(req_opcode, t)
    assert(req_opcode >= Opcode.Handshake
        and req_opcode <= Opcode.Pong, req_opcode)

    local pl = vim.fn.json_encode(t)

    pl = pack.pack_uint32(req_opcode, 1)..pack.pack_uint32(#pl, 1)..pl

    ---@param err   string
    ---@param chunk string
    self.pipe:read_start(function(err, chunk)
        if (req_opcode == Opcode.Handshake) then
            assert(not err, err)
        else
            if err then
                logger:l(50, n, "IPC:write -> pipe:read_start", err)
            end
        end

        logger:l(30, n, "IPC:write -> pipe:read_start", "...Payload arrived");
        self:read(req_opcode, chunk)
    end)

    self.pipe:write(pl, function (err)
        if (req_opcode == Opcode.Handshake) then
            assert(not err, err)
        else
            if (err) then
                logger:l(50, n, "IPC:write -> pipe:write", err)
            end
        end

        logger:l(30, n, "IPC:write -> pipe:write", "...Delivered payload");
    end)
end

--[[ 
 There is no need to go through the authentication process (OAuth2) as suggested 
 in the Discord Developer Documentation. You can send a 
 "SET_ACTIVITY" payload directly to the local RPC server.
--]]
---@param activity Activity
function IPC:set_activity(activity)
    local t = {
        args = {
            activity = activity,
            pid = vim.loop:os_getpid()
        },
        cmd = "SET_ACTIVITY",
        nonce = uuid.uuid4()
    }

    self:write(Opcode.Frame, t)
end

function IPC:handshake()
    if self.handshake_finish then return end

    ---@type Payload
    local t = { client_id = client_id, v = 1 }

    self:write(Opcode.Handshake, t)
end

function IPC:destroy()
    self.pipe:close()
end

return IPC
