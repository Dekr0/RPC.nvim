local logger = require("RPC.logger")

local Opts = require("RPC.options")

local n = "RPC.init"

---@class App
---@field app          App -- This need to be fixed. Returning from RPC:setup() does not persist.
---@field opts         Opts
---@field __IPC        IPC | nil
---@field __state      State
---@field __timer      uv_timer_t | nil
local RPC = {}
RPC.__index = RPC


function RPC:setup(usr)
    if self.app then return end

    assert(usr.ipc_path, "Please specify the absolute path of discord IPC file.")

    local timer = vim.uv.new_timer()

    local opts = Opts:new(usr)

    if usr.log_level then logger:set_filter(usr.log_level) end

    self.app = setmetatable({
        opts         = opts,
        __IPC        = require("RPC.ipc"):new(opts.ipc_path),
        __state      = require("RPC.state"):new(),
        __timer      = timer
    }, self)

    if opts.auto_update then
        timer:start(
            opts.auto_update_timer,
            opts.auto_update_timer,
            function ()
                if self.app.__IPC.handshake_finish then
                    vim.schedule(function () self.app:update() end)
                end
            end
        )
    end

    vim.api.nvim_create_autocmd("BufEnter",   { callback = function ()
        self.app.__state:on_buf_enter()
    end})
    vim.api.nvim_create_autocmd("DirChanged", { callback = function ()
        self.app.__state:on_dir_changed(vim.v.event["cwd"])
    end})
    vim.api.nvim_create_autocmd("VimLeave",   { callback = function ()
        self.app:destroy()
    end})
end


function RPC:destroy()
    if self.__timer then
        self.__timer:stop()
        self.__timer:close()
    end
    if self.__IPC then self.__IPC:destroy() end
end

function RPC:update()
    logger:l(30, n, "RPC:update", string.format("Updating activity..."))

    self.__state:set_activity()

    self.__IPC:set_activity(self.__state.activity)

    logger:l(20, n, "RPC:update", self.__state:tostring())
end


function RPC:show_log() logger:show() end


--- Pause the update loop for Discord Rich Presence 
---@param clear boolean Clear activity status if it is true
function RPC:pause_update(clear)
    if self.__timer then self.__timer:stop() end

    if clear then
        vim.schedule(function ()
            logger:l(30, n, "RPC:update", string.format("Clearing activity"))

            self.__state:clear_activity()

            self.__IPC:set_activity(self.__state.activity)
        end)
    end
end


--- Resume the update loop for Discord Rich Presence
function RPC:resume_update()
    assert(self.__IPC, "Connection with Discord is permantely closed. Please reset the \
        plugin state with RPC:close() to clear out potential dirty state. Then \
        reopen a new connection with Discord and a new update loop with \
        RPC:open()")
    assert(self.__timer, "Update loop is permantely closed. Please reset the \
        plugin state with RPC:close() to clear out potential dirty state. Then \
        reopen a new connection with Discord and a new update loop with \
        RPC:open()")
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


--- Close the connection with Discord
function RPC:close()
    if self.__timer then
        self.__timer:stop()
        self.__timer = nil
    end
    if self.__IPC then
        self.__IPC:destroy()
        self.__IPC = nil
    end
end


function RPC:open()
    assert(not self.__IPC and not self.__timer, "Failed to open a new a new \
        connection with Discord, and then start a new update loop. Reason: \
        there's an existence connection is still open, or there's an existence \
        update loop that is still running. Please use RPC:close() to clear out \
        the entire state first before calling RPC:open()")

    self.__IPC = require("RPC.ipc"):new(self.opts.ipc_path)
    self.__timer = vim.uv.new_timer()

    if self.opts.auto_update then
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
