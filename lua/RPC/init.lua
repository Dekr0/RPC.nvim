local logger = require("RPC.logger")
local encoder = require("RPC.encoder")

local namespace = "RPC.nvim"

local Opcode = {
    Handshake = 0,
    Frame     = 1,
    Close     = 2,
    Ping      = 3,
    Pong      = 4
}

---@class RPCOpts
---@field auto_update       boolean
---@field auto_update_timer integer
---@field log_level         string
---@field profiling         boolean
local RPCOpts = {}
RPCOpts.__index = RPCOpts

---@class App
---@field auto_update       boolean
---@field auto_update_timer integer 
---@field log_level         string 
---@field profiling         boolean
---@field __IPC             uv_pipe_t
---@field __next_state      State
---@field __timer           uv_timer_t
local RPC = {}
RPC.__index = RPC

---@param opts RPCOpts
function RPC:setup(opts)
    local next_state = require("richpresence.state")

    local timer = vim.uv.new_timer()

    opts.auto_update_timer = opts.auto_update_timer or 10000
    opts.auto_update_timer = opts.auto_update_timer > 5000 and
        opts.auto_update_timer or 10000

    local app = setmetatable({
        auto_update       = opts.auto_update or false,
        auto_update_timer = opts.auto_update_timer,
        logging           = opts.log_level or false,
        profiling         = opts.profiling or false,
        __next_state      = next_state,
        __timer           = timer
    }, self)

    if app.auto_update then
        timer:start(
            app.auto_update_timer,
            app.auto_update_timer,
            function () app:update() end
        )
    end

    vim.api.nvim_create_autocmd("BufEnter",   { callback = function ()
        app.__next_state:on_buf_enter()
    end})
    vim.api.nvim_create_autocmd("DirChanged", { callback = function ()
        -- get current directory after cd => vim.v.event["cwd"]
    end})
    vim.api.nvim_create_autocmd("VimLeave",   { callback = function ()
        app:destroy()
    end})

    return app
end

function RPC:destroy()
    if self.__timer then
        self.__timer:stop()
        self.__timer:close()
    end
end

function RPC:log(f, m)
    logger:log(string.format("%s.%s: %s", namespace, f, m))
end

function RPC:show_log()
    logger:show()
end

function RPC:update()
    if self.log_level then
        self:log("run_callbacks", "run all pending DiscordSDK callbacks")
    end

    math.randomseed(os.time())
    math.random(); math.random(); math.random()

    self.__next_state.apm = math.random(60, 90)

    if self.log_level then
        self:log("run_sdk_callbacks", self.__next_state:tostring())
    end
end

function RPC:connect()
   self.__IPC = vim.loop.new_pipe(false)
   self.__IPC:connect("/run/user/1000/discord-ipc-0", function (err)
       if (err) then
           error(err)
       else
           vim.schedule(function ()
               self:handshake()
           end)
       end
   end)
end

function RPC:handshake()
    local req_payload = {
        client_id = "1222702873440157796",
        v = 1
    }
    local req_json = vim.fn.json_encode(req_payload)
    print(req_json)
end

return RPC
