local logger = require("RPC.logger")

local Opts = require("RPC.options")

local n = "RPC.init"

---@class App
---@field opts         Opts
---@field __IPC        IPC
---@field __state      State
---@field __timer      uv_timer_t
local RPC = {}
RPC.__index = RPC

function RPC:setup(usr)
    assert(usr.ipc_path, "Please specify the absolute path of discord IPC file.")

    local timer = vim.uv.new_timer()

    local opts = Opts:new(usr)

    if usr.log_level then logger:set_filter(usr.log_level) end

    local app = setmetatable({
        opts         = opts,
        __IPC        = require("RPC.ipc"):new(usr.ipc_path),
        __state      = require("RPC.state"):new(),
        __timer      = timer
    }, self)

    if opts.auto_update then
        timer:start(
            opts.auto_update_timer,
            opts.auto_update_timer,
            function ()
                if app.__IPC.handshake_finish then
                    vim.schedule(function () app:update() end)
                end
            end
        )
    end

    vim.api.nvim_create_autocmd("BufEnter",   { callback = function ()
        app.__state:on_buf_enter()
    end})
    vim.api.nvim_create_autocmd("DirChanged", { callback = function ()
        app.__state:on_dir_changed(vim.v.event["cwd"])
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
    self.__IPC:destroy()
end

function RPC:update()
    logger:l(30, n, "RPC:update", string.format("Updating activity..."))

    self.__state:set_activity()

    self.__IPC:set_activity(self.__state.activity)

    logger:l(20, n, "RPC:update", self.__state:tostring())
end

function RPC:show_log() logger:show() end

function RPC:pause_update()
    if self.__timer then self.__timer:stop() end
end

function RPC:resume_update()
    if self.__timer then
        self.__timer:start(
            self.opts.auto_update_timer,
            self.opts.auto_update_timer,
            function ()
               if self.__IPC.handshake_finish then
                    vim.schedule(function () self:update() end)
               end
            end
        )
    end
end

return RPC
