local sdk = require("richpresence.sdk")
local logger = require("richpresence.logger")

local namespace = "richpresence.nvim"
local logw = function (f, m)
    logger:log(string.format("%s.%s: %s", namespace, f, m))
end

---@class M
---@field setup function
---@field show  function
local M = {}

---@class App
---@field next_state State
---@field timer uv_timer_t
---@field sdk any 
---@field chan uv_pipe_t
---@field server     uv_pipe_t
local App = {}

App.__index = App

local on_buf_enter = function()
    App.next_state:on_buf_enter()
end

local on_dir_changed = function()
    -- get current directory after cd => vim.v.event["cwd"]
end

local run_sdk_callbacks = function()
    logw("run_callbacks", "run all pending DiscordSDK callbacks")

    -- generate random APM
    math.randomseed(os.time())
    math.random(); math.random(); math.random()
    App.next_state.apm = math.random(60, 90)

    logw("run_sdk_callbacks", App.next_state:tostring())

    sdk.run_callback(
        App.sdk,
        App.next_state.workplace,
        App.next_state.filename,
        App.next_state.ext,
        App.next_state.mode,
        App.next_state.apm
    )
end

local on_sdk_data = function(err, data)
    logw("on_sdk_data", vim.inspect(err))
    logw("on_sdk_data", vim.inspect(data))
end

local serve = function()
   if not App.chan then
       App.chan = vim.loop.new_pipe(false)
       App.server:accept(App.chan)
       App.chan:read_start(on_sdk_data)
       logw("serve", "connection established with discord SDK.")
       return
   end
   logw("serve", "multiple instances of discord SDK is not allowed.")
end

local destroy = function()
    if App.timer then
        App.timer:stop()
        App.timer:close()
    end
    if App.sdk then
        sdk.clean(App.sdk)
    end
    if App.chan then
        App.chan:read_stop()
        App.chan:close()
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

        App.next_state = require("richpresence.state")

        App.sdk = sdk.init(
            App.next_state.workplace,
            App.next_state.filename,
            App.next_state.ext,
            App.next_state.mode,
            App.next_state.apm
        )

        App.timer = vim.uv.new_timer()
        App.timer:start(5000, 5000, run_sdk_callbacks)
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
