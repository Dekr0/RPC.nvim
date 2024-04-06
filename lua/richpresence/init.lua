local ipcwriter = require("richpresence.ipcwriter")
local logger = require("richpresence.logger")

local namespace = "richpresence.nvim"
local data = string.format("%s/%s", vim.fn.stdpath("data"), namespace)
local logw = function (f, m)
    logger:log(string.format("%s.%s: %s", namespace, f, m))
end

---@class M
---@field setup function
---@field show  function
local M = {}

---@class App
---@field job number
---@field packet number
---@field middleware uv_pipe_t
---@field server     uv_pipe_t
local App = {}

App.__index = App

local on_buf_enter = function()
    -- get current buffer => vim.api.nvim_buf_get_name
end

local on_dir_changed = function()
    -- get current directory after cd => vim.v.event["cwd"]
end

local serve = function()
   if not App.middleware then
       App.middleware = vim.loop.new_pipe(false)
       App.server:accept(App.middleware)
       logw("serve", "connection established with middleware.")
       return
   end
   logw("serve", "multiple instances of middleware is not allowed.")
end

local destroy = function()
    if App.ipcwriter then
        ipcwriter.destroy(App.ipcwriter)
        os.remove("/tmp/middlware.socket")
    end
    if App.middleware then
        App.middleware:close()
    end
    if App.server then
        App.server:close()
    end
    if App.job > 0 then
        vim.fn.jobstop(App.job)
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

local on_middleware_exit = function (_, d, _)
    logw("on_middleware_exit", vim.inspect(d))
end

local on_middleware_err = function (_, d, _)
    logw("on_middleware_err", vim.inspect(d))
end

local on_middleware_stdout = function (_, d, _)
    logw("on_middleware_stdout", vim.inspect(d))
end

local start_middleware = function()
    if not App.job then
        local opts = {
            cwd = data,
            on_exit = on_middleware_exit,
            on_stderr = on_middleware_err,
            on_stdout = on_middleware_stdout
        }
        local cmd = {"bash", "-c", './Middleware'}
        App.job = vim.fn.jobstart(cmd, opts)
        if App.job <= 0 then
           logw("start_middleware", "failed to start middleware")
           return
        end
        logw("start_middleware", "middleware is running")
    end
end

local init = function()
    App.packet = 0
    if #vim.fs.find("nvim.socket", { path = "/tmp"}) > 0 then
        logw("init",
        "an instance of neovim is already running (check detail in README.md)")
        return
    end
    if not App.server then
        App.server = vim.loop.new_pipe(false)
        App.server:bind("/tmp/nvim.socket")
        App.server:listen(128, serve)
        App.counter = 0
        logw("init", "server is running.")
        register_vim_autocmd()
        return
    end
    logw("init", "an instance of server is already running.")
    start_middleware()
    vim.defer_fn(function ()
    end, 2000)
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
