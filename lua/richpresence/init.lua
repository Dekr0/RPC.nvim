local logger   = require("richpresence.logger")
local profiler = require("plenary.profile")
local sdk      = require("richpresence.sdk")

local namespace = "richpresence.nvim"

---@class RichPresenceOptions
---@field auto_update       boolean
---@field auto_update_timer integer
---@field logging           boolean
---@field profiling         boolean
local RichPresenceOptions = {}
RichPresenceOptions.__index = RichPresenceOptions

---@class App
---@field auto_update       boolean
---@field auto_update_timer integer 
---@field logging           boolean
---@field profiling         boolean
---@field __discord         any 
---@field __next_state      State
---@field __timer           uv_timer_t
local App = {}
App.__index = App

---@param opts RichPresenceOptions
function App:setup(opts)
    local next_state = require("richpresence.state")
    local discord = sdk.init(
        next_state.workplace,
        next_state.filename,
        next_state.ext,
        next_state.mode,
        next_state.apm
    )
    local timer = vim.uv.new_timer()

    opts.auto_update_timer = opts.auto_update_timer or 10000
    opts.auto_update_timer = opts.auto_update_timer > 5000 and
        opts.auto_update_timer or 10000

    local app = setmetatable({
        auto_update       = opts.auto_update or false,
        auto_update_timer = opts.auto_update_timer,
        logging           = opts.logging or false,
        profiling         = opts.profiling or false,
        __discord         = discord,
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

function App:destroy()
    if self.__timer then
        self.__timer:stop()
        self.__timer:close()
    end
    if self.__discord then
        sdk.clean(self.__discord)
    end
end

function App:log(f, m)
    logger:log(string.format("%s.%s: %s", namespace, f, m))
end

function App:show_log()
    logger:show()
end

function App:update()
    if self.profiling then profiler.start("profile.log", { flame = true }) end

    if self.logging then
        self:log("run_callbacks", "run all pending DiscordSDK callbacks")
    end

    math.randomseed(os.time())
    math.random(); math.random(); math.random()

    self.__next_state.apm = math.random(60, 90)

    if self.logging then
        self:log("run_sdk_callbacks", self.__next_state:tostring())
    end

    sdk.run_callback(
        self.__discord,
        self.__next_state.workplace,
        self.__next_state.filename,
        self.__next_state.ext,
        self.__next_state.mode,
        self.__next_state.apm
    )

    if self.profiling then profiler.stop() end
end

return App
