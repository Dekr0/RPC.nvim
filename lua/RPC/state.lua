local utils = require("RPC.utils")

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

function State:tostring()
    return string.format("filename = %s, ext = %s", self.filename, self.ext)
end

---@return boolean
function State:on_buf_enter()
    local buf_name = vim.api.nvim_buf_get_name(0)
    if (vim.fn.isdirectory(buf_name) == 1) then
        return false
    end
    if (string.len(buf_name) == 0) then
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
