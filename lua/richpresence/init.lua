local sdk = require("richpresence.sdk")
local logger = require("richpresence.logger")
local utils = require("richpresence.utils")

local namespace = "richpresence.nvim"
local logw = function (f, m)
    logger:log(string.format("%s.%s: %s", namespace, f, m))
end

---@class M
---@field setup function
---@field show  function
local M = {}

---@class App
---@field callback_timer uv_timer_t
---@field middleware any 
---@field middleware_chan uv_pipe_t
---@field server     uv_pipe_t
local App = {}

App.__index = App

local on_buf_enter = function()
end

local on_dir_changed = function()
    -- get current directory after cd => vim.v.event["cwd"]
end

local run_sdk_callbacks = function()
    logw("run_callbacks", "run all pending DiscordSDK callbacks")
    local user_id = sdk.run_callback(App.middleware)
    logw("run_callbacks", string.format("User Id: %d", user_id))
end

local on_sdk_data = function(err, data)
    logw("on_sdk_data", vim.inspect(err))
    logw("on_sdk_data", vim.inspect(data))
end

local serve = function()
   if not App.middleware_chan then
       App.middleware_chan = vim.loop.new_pipe(false)
       App.server:accept(App.middleware_chan)
       App.middleware_chan:read_start(on_sdk_data)
       logw("serve", "connection established with discord SDK.")
       return
   end
   logw("serve", "multiple instances of discord SDK is not allowed.")
end

local destroy = function()
    if App.callback_timer then
        App.callback_timer:stop()
        App.callback_timer:close()
    end
    if App.middleware then
        sdk.clean(App.middleware)
    end
    if App.middleware_chan then
        App.middleware_chan:read_stop()
        App.middleware_chan:close()
    end
    if App.server then
        App.server:close()
    end
end

local register_vim_autocmd = function()
    vim.api.nvim_create_autocmd("VimLeave", {
        callback = destroy
    })
    vim.api.nvim_create_autocmd("BufEnter", {
        callback = on_buf_enter
    })
    vim.api.nvim_create_autocmd("DirChanged", {
        callback = on_dir_changed
    })
end

local init = function()
    if #vim.fs.find("nvim.socket", { path = "/tmp"}) > 0 then
        logw("init",
        "an instance of neovim is already running (check detail in README.md)")
        return
    end
    if not App.server then
        App.server = vim.loop.new_pipe(false)
        App.server:bind("/tmp/nvim.socket")
        App.server:listen(128, serve)
        if utils.open_as_workspace() then
            App.middleware = sdk.init(utils.cwd(), "", "", "Normal");
        else
            local filename, ext = utils.cf()
            App.middleware = sdk.init(utils.cwd(), filename, ext, "Normal");
        end
        App.callback_timer = vim.uv.new_timer()
        App.callback_timer:start(2000, 2000, run_sdk_callbacks)
        register_vim_autocmd()
        return
    end
    logw("init", "an instance of server is already running.")
end

M.setup = function ()
    vim.api.nvim_create_autocmd("VimEnter", {
        callback = init
    })
end

M.show = function ()
    logger:show()
end

return M
