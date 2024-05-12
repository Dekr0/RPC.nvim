local logger = require("RPC.logger")

local n = "RPC.init"

---@class RPCOpts
---@field auto_update       boolean
---@field auto_update_timer integer
---@field logging           boolean 
---@field profiling         boolean
local RPCOpts = {}
RPCOpts.__index = RPCOpts

---@class App
---@field auto_update       boolean
---@field auto_update_timer integer 
---@field logging           boolean 
---@field profiling         boolean
---@field __IPC             IPC
---@field __next_state      State
---@field __timer           uv_timer_t
local RPC = {}
RPC.__index = RPC

---@param opts RPCOpts
function RPC:setup(opts)
    local timer = vim.uv.new_timer()

    opts.auto_update_timer = opts.auto_update_timer or 10000
    opts.auto_update_timer = opts.auto_update_timer > 5000 and
        opts.auto_update_timer or 10000

    local app = setmetatable({
        auto_update       = opts.auto_update or false,
        auto_update_timer = opts.auto_update_timer,
        logging           = opts.logging or false,
        profiling         = opts.profiling or false,
        __IPC             = require("RPC.ipc"),
        __next_state      = require("RPC.state"),
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

function RPC:update()
    if self.logging then
        logger(n, "run_callbacks", "run all pending DiscordSDK callbacks")
    end

    math.randomseed(os.time())
    math.random(); math.random(); math.random()

    self.__next_state.apm = math.random(60, 90)

    if self.logging then
        logger(n, "run_sdk_callbacks", self.__next_state:tostring())
    end
end

function RPC:show_log()
    logger:show()
end

return RPC
