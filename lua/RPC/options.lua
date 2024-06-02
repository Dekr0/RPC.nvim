---@class Opts
---@field apm               boolean
---@field auto_update       boolean
---@field auto_update_timer integer | nil
---@field ipc_path          string | nil
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
       ipc_path          = usr.ipc_path or "",
   }, self)

   return opts
end

return Opts
