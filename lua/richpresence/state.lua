local utils = require("richpresence.utils")

---@class State
---@field workplace string
---@field filename string
---@field ext string
---@field mode string
---@field apm number
local State = {}

State.__index = State

---@return State
function State:new()
    local filename = ""
    local ext = ""
    if not utils.open_as_workplace() then
        filename, ext = utils.cf()
    end
    local state = setmetatable({
        workplace = utils.cwd(),
        filename = filename,
        ext = ext,
        mode = "Normal",
        apm = 0
    }, self)

    return state
end

---@return boolean
function State:on_buf_enter()
    if (string.len(vim.api.nvim_buf_get_name(0)) == 0) then
        return false
    end
    local filename, ext = utils.cf()

    -- exact same file name but in a different directory
    if filename == self.filename and ext == self.ext then
        return false
    end

    self.filename = filename
    self.ext = ext

    return true
end

return State:new()
