local logger = require("RPC.logger")
local pack = require("pack")

local n = "RPC.ipc"

local client_id = "1222702873440157796"

-- Types of binary message
---@class Opcode
---@field Handshake integer 
---@field Frame     integer 
---@field Close     integer 
---@field Ping      integer 
---@field Pong      integer 
local Opcode = {
    Handshake = 0,
    Frame     = 1,
    Close     = 2,
    Ping      = 3,
    Pong      = 4
}

---@class IPC
---@field connected        boolean
---@field handshake_finish boolean
---@field logger           Logger
---@field pipe             uv_pipe_t
local IPC = {}
IPC.__index = IPC

-- Discord can be on any pipe ranging from `discord-ipc-0` to `discord-ipc-9`.
-- Try and connect to each one. Keep the first one successfully connected.
---@return string | nil
local function pipe_path_lookup()
    return "/run/user/1000/discord-ipc-0"
end

function IPC:new()
    local pipe_path = pipe_path_lookup()

    assert(pipe_path, "Failed to locate Discord's pipe for IPC")

    local instance = setmetatable({
        connected        = false,
        handshake_finish = false,
        pipe             = vim.loop.new_pipe(false)
    }, self)

    instance.pipe:connect(pipe_path, function (reason)
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
end

-- Abstraction on message delivery
-- Binary message structure used to communicate Discord via pipe (Windows) or 
-- unix domain socket (Unix).
--
-- The binary message structure is a frame represented via a single byte array.
--
-- | header | data in JSON format | 
--
-- The header's structure => | opcode | length of entire frame |
--      Opcode and length of entire frame are little endian unsigned 
--      integers (32 bits).
--      Make sure to convert them based on architecture specific 
-- 
-- Cautions (Windows as an example):
--     Multiple stream.Write(opcode); stream.Write(length) will break the pipe. 
--     Instead create a buffer, write the data to the buffer, then send the 
--     entire buffer to the stream.
-- 
---@param req_opcode      integer 
---@param t           table 
function IPC:write(req_opcode, t)
    assert(req_opcode >= Opcode.Handshake 
        and req_opcode <= Opcode.Pong, req_opcode)

    local pl = vim.fn.json_encode(t)

    pl = pack.pack_uint32(req_opcode, 1)..pack.pack_uint32(#pl, 1)..pl

    ---@param err   string
    ---@param chunk string
    self.pipe:read_start(function(err, chunk)
        assert(not err, err)
        logger:l(n, "IPC:write: pipe:write", "...Payload arrived");
        logger:l(n, "IPC:write: pipe:write", vim.inspect(chunk));
        self:read(req_opcode, chunk)
    end)

    self.pipe:write(pl, function (reason)
        assert(not reason, reason)
        logger:l(n, "IPC:write: pipe:write", "...Delivered payload");
    end)
end

function IPC:handshake()
    if self.handshake_finish then return end

    local t = { client_id = client_id, v = 1 }

    self:write(Opcode.Handshake, t)
end

return IPC:new()
