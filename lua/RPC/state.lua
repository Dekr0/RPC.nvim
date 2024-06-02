local utils = require("RPC.utils")

---@class Timestamps
---@field start integer
---@field end   integer | nil

---@class Assets
---@field large_image string
---@field large_text  string | nil
---@field small_image string
---@field small_text  string | nil

---@class Activity
---@field assets         Assets
---@field details        string
---@field instance       boolean
---@field state          string
---@field timestamps     Timestamps

---@enum Icon 
local Icon = {
    C         = "c",
    CSS       = "css",
    GIT       = "git",
    GO        = "go",
    H         = "h",
    JS        = "js",
    LUA       = "lua",
    MAKEFILE  = "makefile",
    MD        = "md",
    TS        = "ts",
    INSERT    = "insert",
    NORMAL    = "normal",
    REPLACE   = "replace",
    VISUAL    = "visual",
    NEOVIM   = "neovim",
}

---@class State
---@field activity        Activity | nil
---@field apm             number
---@field ext             string
---@field filename        string
---@field workplace       string
---@field mode            string
local State = {}
State.__index = State


---@return State
function State:new()
    local filename = ""
    local ext = ""
    if not utils.open_as_workplace() then
        filename, ext = utils.get_name_ext()
    end

    ---@type Activity
    ---@diagnostic disable
    local activity = {
        timestamps = {
            start = os.time() -- Need replacement 
        },
        instance = true
    }

    ---@type State
    local state = setmetatable({
        activity = activity,
        apm = 0,
        ext = ext,
        filename = filename,
        mode = "Normal",
        workplace = utils.cwd(),
    }, self)

    state:set_activity()

    return state
end

function State:tostring()
    return string.format("filename = %s, ext = %s, workplace = %s", 
        self.filename, self.ext, self.workplace)
end

-- True means there is state changed. False means there is no state changed.
---@return boolean
function State:on_buf_enter()
    local buf_name = vim.api.nvim_buf_get_name(0)
    if vim.fn.isdirectory(buf_name) == 1 then
        return false
    end
    if string.len(buf_name) == 0 then
        return false
    end
    local filename, ext = utils.get_name_ext()

    if filename == self.filename and ext == self.ext then
        return false
    end

    self.filename = filename
    self.ext = ext

    return true
end

---@param workplace string | nil
function State:on_dir_changed(workplace)
    self.workplace = workplace and vim.fs.basename(workplace) or "???"
end


local MAX_TEXT_LEN  = 128
local MAX_EXT_LEN   = 12
local STATE_PREFIX       = "In "
local APM_PREFIX         = " | APM: "
local DETAILS_PREFIX     = "Editing ."
local FILENAME_POSTFIX   = "... "
local POSTFIX            = "..."
local MAX_WORKPLACE_LEN = MAX_TEXT_LEN - #STATE_PREFIX - #APM_PREFIX - 3

function State:set_activity()
    math.randomseed(os.time())
    math.random(); math.random(); math.random() -- warm up

    self.apm = math.random(60, 90)

    if self.filename == "" then
        self.activity.assets = {
            large_image = "neovim",
            large_text  = string.format("In %s", self.workplace),
            small_image = Icon.NORMAL,
            small_text  = "In Normal mode"
        }
    else
        self.activity.assets = {
            large_image = Icon[self.ext:upper()] or "unknown",
            large_text  = string.format("%s.%s", self.filename, self.ext),
            small_image = Icon.INSERT,
            small_text  = string.format("In %s mode", Icon.INSERT)
        }
    end


    if self.filename == "" then 
        self.activity.details = ""
    else
        -- Definitely need a rewrite for trimming the display message
        local filename_lim = MAX_TEXT_LEN - #DETAILS_PREFIX - 
            (#self.ext < MAX_EXT_LEN and #self.ext or MAX_EXT_LEN)

        self.activity.details = string.format("Editing %s",
            #self.filename <= filename_lim and self.filename 
                or self.filename:sub(1, filename_lim - #FILENAME_POSTFIX)..FILENAME_POSTFIX
        )
        
        if self.filename ~= self.ext then
            self.activity.details = self.activity.details..string.format(".%s",
                #self.ext <= MAX_EXT_LEN and self.ext 
                    or self.ext:sub(1, MAX_EXT_LEN - #POSTFIX)..POSTFIX
            )
        end
    end
    
    self.activity.state = string.format("In %s | APM: %d",

        #self.workplace < MAX_WORKPLACE_LEN and self.workplace or 
            self.workplace:sub(1, MAX_WORKPLACE_LEN - #POSTFIX)..POSTFIX,

        self.apm
    )
end

function State:clear_activity()
   self.activity.assets.large_image = Icon.NEOVIM
   self.activity.assets.large_text  = ""
   self.activity.assets.small_image = ""
   self.activity.assets.small_text  = ""
   
   self.activity.details = ""

   self.activity.state = ""
end

return State
