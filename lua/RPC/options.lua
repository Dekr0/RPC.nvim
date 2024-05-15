--[[
--  custom_state and custom_detail are strings respectively that can be either a
--  pure plain text, or pure directives (indicate by square bracket []), or a
--  mix of above two. The directives will be parsed and be replace to the 
--  corresponding content.
--
--  Following are the directives that can be parsed (must be exact match)
--      - [file] => replace with current active buffer name
--      - [mode] => replace with current active mode
--      - [workplace] => replace with current working directory
--      - [apm] => replace with user's APM (Action Per Minute)
--      directory
-- 
-- An example: 
--      custom_detail = "Editing: [file] | Mode: [mode]",
--      custom_state  ="In workplace: [workplace] | APM: [apm]"
--]]
---@class Opts
---@field apm               boolean
---@field auto_update       boolean
---@field auto_update_timer integer | nil
---@field custom_detail     string  | nil 
---@field custom_state      string  | nil
---@field logging           boolean 
---@field profiling         boolean
local Opts = {}
Opts.__index = Opts

function Opts:new(usr)
    usr.auto_update_timer = usr.auto_update_timer or 10000
    usr.auto_update_timer = usr.auto_update_timer > 5000 and
        usr.auto_update_timer or 10000

   ---@type Opts
   local opts = setmetatable({
       apm               = usr.apm or false,
       auto_update       = usr.auto_update or false,
       auto_update_timer = usr.auto_update_timer or false,
       custom_detail     = usr.custom_detail or "Editing: [file] | Mode: [mode]",
       custom_state      = usr.custom_state or "In workplace: [workplace] | APM: [apm]",
       logging           = usr.logging or true,
       profiling         = usr.profiling or false
   }, self)

   return opts
end

return Opts
