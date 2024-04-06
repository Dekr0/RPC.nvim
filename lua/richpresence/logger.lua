---@class Logger
---@field lines string[]
---@field max_lines number
local Logger = {}

Logger.__index = Logger

---@return Logger
function Logger:new()
    local logger = setmetatable({
        lines = {},
        max_lines = 50,
    }, self)

    return logger
end

---@param line string
function Logger:log(line)
    table.insert(self.lines, line)

    while #self.lines > self.max_lines do
        table.remove(self.lines, 1)
    end
end

function Logger:clear()
    self.lines = {}
end

function Logger:show()
    local bufnr = vim.api.nvim_create_buf(false, true)
    vim.api.nvim_buf_set_lines(bufnr, 0, -1, false, self.lines)
    vim.api.nvim_win_set_buf(0, bufnr)
end

return Logger:new()
