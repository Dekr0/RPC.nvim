---@class FixedRingBuffer
---@field buffer any[]
---@field head   integer
---@field tail   integer
---@field size   integer
---@field full   boolean
local FixedRingBuffer = {}

FixedRingBuffer.__index    = FixedRingBuffer
FixedRingBuffer.__tostring = FixedRingBuffer.tostring

---@param size number | nil
function FixedRingBuffer:new(size)
    return setmetatable({
        buffer = {},
        head   = 1,
        tail   = 1,
        size   = size or 500,
        full   = false
    }, self)
end

function FixedRingBuffer:next_read()
    self.tail = self.tail + 1 > self.size and 1 or self.tail + 1
end

function FixedRingBuffer:next_write()
    self.head = self.head + 1 > self.size and 1 or self.head + 1
end

function FixedRingBuffer:empty()
    return self.head == self.tail and not self.full
end

function FixedRingBuffer:push(i)
    local old = nil

    if self.full then
        old = self.buffer[self.head]
        self:next_read()
    end

    self.buffer[self.head] = i

    self:next_write()

    if self.head == self.tail then self.full = true end

    return old
end

function FixedRingBuffer:pop()
    if self:empty() then return nil end

    local i = self.buffer[self.tail]

    self.full = false

    self:next_read()

    return i
end

function FixedRingBuffer:emit()
    local buffer = {}

    local i = self:pop()

    while i do
       table.insert(buffer, i)

       i = self:pop()
    end

    return buffer
end

---@return string
function FixedRingBuffer:tostring()
   return string.format("buffer = {%s}; head = %d; tail = %d; full? %s; empty? %s",
        table.concat(self.buffer, " "),
        self.head,
        self.tail,
        self.full    and "Yes" or "No",
        self:empty() and "Yes" or "No"
   )
end

return FixedRingBuffer
