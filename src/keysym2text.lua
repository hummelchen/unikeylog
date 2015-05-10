
local utf8 = require 'utf8' -- Lua 5.3 or luarocks install utf8

local function binaryToInt(n)
    local a, b, c, d = n:byte(1, 4)
    return d * 256^3 + c * 256^2 + b * 256 + a
end

local keysym_map = {}
for line in io.lines('keysyms.txt') do
    if line:sub(1, 1) ~= '#' then
        -- example: 0x0020   U0020   .   # space
        local keysym, char = line:match('^0x(%x+)%s+U(%x+)')
        if keysym and char then
            keysym = tonumber(keysym, 16)
            --keysym = numberToBinary(keysym)
            char = tonumber(char, 16)
            if char > 0 and char >= string.byte(' ') then
                char = utf8.char(char)
                keysym_map[keysym] = char
            end
        end
    end
end

local keysym2def = {}
for line in io.lines('/usr/include/X11/keysymdef.h') do
    -- example: #define XK_Tab           0xff09
    local def, keysym = line:match('^#define XK_([^ ]+)%s+0x(%x+)')
    if keysym and def then
        keysym = tonumber(keysym, 16)
        keysym2def[keysym] = def
    end
end

return function(keysym)
    if keysym_map[keysym] then
        return keysym_map[keysym]
    end
    if keysym2def[keysym] then
        return "<" .. keysym2def[keysym] .. ">"
    end
    return '?'
end
