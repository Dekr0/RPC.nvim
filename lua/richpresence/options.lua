---@class Options
---@field auto_update boolean
---@field logging     boolean
local Options = {}

Options.__index = Options

function Options:new(config)
   local o = config or {
       auto_update = false,
       logging     = false
   }

   return setmetatable(o, self)
end

return Options.new
