local logger = require("RPC.logger")
local Opts = require("RPC.options")

local n = "RPC.init"

---@class App
---@field opts         Opts
---@field __IPC        IPC
---@field __next_state State
---@field __timer      uv_timer_t
local RPC = {}
RPC.__index = RPC

function RPC:setup(usr)
    local timer = vim.uv.new_timer()

    local opts = Opts:new(usr)

    local app = setmetatable({
        opts         = opts,
        __IPC        = require("RPC.ipc"),
        __next_state = require("RPC.state"),
        __timer      = timer
    }, self)

    if opts.auto_update then
        timer:start(
            opts.auto_update_timer,
            opts.auto_update_timer,
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
    if self.opts.logging then
        logger:l(n, "run_callbacks", "run all pending DiscordSDK callbacks")
    end

    math.randomseed(os.time())
    math.random(); math.random(); math.random()

    self.__next_state.apm = math.random(60, 90)

    if self.opts.logging then
        logger:l(n, "run_sdk_callbacks", self.__next_state:tostring())
    end
end

function RPC:show_log()
    logger:show()
end

return RPC
