local Options = require("richpresence.options")
local logger = require("richpresence.logger")
local sdk    = require("richpresence.sdk")

local namespace = "richpresence.nvim"

---@class M
---@field app App
local M = { }

---@class App
---@field discord    any 
---@field next_state State
---@field opts       Options
---@field timer      uv_timer_t
local App = {}

App.__index = App

---@param opts Options
function App:new(opts)
    local next_state = require("richpresence.state")
    local discord = sdk.init(
        next_state.workplace,
        next_state.filename,
        next_state.ext,
        next_state.mode,
        next_state.apm
    )
    local timer = vim.uv.new_timer()

    local app = setmetatable({
        discord    = discord,
        next_state = next_state,
        opts       = Options(opts),
        timer      = timer
    }, self)

    if opts.auto_update then
        timer:start(5000, 5000, function () app:update() end)
    end

    return app
end

function App:destroy()
    if self.timer then
        self.timer:stop()
        self.timer:close()
    end
    if self.discord then
        sdk.clean(self.discord)
    end
end

function App:log(f, m)
    logger:log(string.format("%s.%s: %s", namespace, f, m))
end

function App:update()
    if self.opts.logging then
        self:log("run_callbacks", "run all pending DiscordSDK callbacks")
    end

    math.randomseed(os.time())
    math.random(); math.random(); math.random()

    self.next_state.apm = math.random(60, 90)

    if self.opts.logging then
        self:log("run_sdk_callbacks", self.next_state:tostring())
    end

    sdk.run_callback(
        self.discord,
        self.next_state.workplace,
        self.next_state.filename,
        self.next_state.ext,
        self.next_state.mode,
        self.next_state.apm
    )
end

---@param opts Options
function M.setup(opts)
    M.app = App:new(opts)
    vim.api.nvim_create_autocmd("BufEnter",   { callback = function ()
        M.app.next_state:on_buf_enter()
    end})
    vim.api.nvim_create_autocmd("DirChanged", { callback = function ()
        -- get current directory after cd => vim.v.event["cwd"]
    end})
    vim.api.nvim_create_autocmd("VimLeave",   { callback = function ()
        M.app:destroy()
    end})
end

function M.show()
    logger:show()
end

return M
