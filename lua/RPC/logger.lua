---@enum LEVEL
local LEVEL = {
    TRACE = 10,
    DEBUG = 20,
    INFO  = 30,
    WARN  = 40,
    ERROR = 50,
    FATAL = 60
}

---@class Logger
---@field LEVEL     Enum
---@field filter    LEVEL
---@field lines     string[]
---@field max_lines number
local Logger = {}

local ns = "RPC.logger"

---@type Logger | nil
local instance = nil

Logger.LEVEL = LEVEL

Logger.__index = Logger


function Logger:clear() self.lines = {} end

--- A helper function for produce formatted log message.
---@param l LEVEL  -- log level
---@param n string -- name of the module that invokes logger
---@param f string -- name of the function that invokes logger
---@param m string -- log message
function Logger:l(l, n, f, m)
    if l < self.filter then return end

    self:log(l, string.format("%s.%s: %s", n, f, m))
end

---@param level LEVEL
---@param line  string
function Logger:log(level, line)
    if level < self.filter then return end

    table.insert(self.lines, line)

    while #self.lines > self.max_lines do
        table.remove(self.lines, 1)
    end
end

---@param filter LEVEL | nil
---@return Logger
function Logger:new(filter)
    if instance then return instance end

    instance = setmetatable({
        filter = filter or LEVEL.DEBUG,
        lines = {},
        max_lines = 50,
    }, self)

    return instance
end

---@param filter any
function Logger:set_filter(filter)
    if type(filter) ~= "number" then
        self:l(40, ns, "Logger:set_filter", "Invalid log level filter")
        return
    end

    if filter < LEVEL.TRACE or filter > LEVEL.FATAL then
        self:l(40, ns, "Logger:set_filter", "Invalid log level filter")
        return
    end

    self.filter = filter
end

function Logger:show()
    local bufnr = vim.api.nvim_create_buf(false, true)
    vim.api.nvim_buf_set_lines(bufnr, 0, -1, false, self.lines)
    vim.api.nvim_win_set_buf(0, bufnr)
end

return Logger:new(20)
