local KEY_1 = 2

local ch_table = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\r',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    'X', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 'X',
    'X', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'
    }

local ch_table_upper = {
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\r',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    'X', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'', 'X',
    'X', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?'
}

local ch_map = {
    [1] = '<Esc>',
    [14] = '<Backspace>',
    [15] = '<Tab>',
    [28] = '<Return>',
    [29] = '<LeftCtrl>',
    [42] = '<LeftShift>',
    [54] = '<RightCtrl>',
    [56] = '<LeftAlt>',
    [58] = '<CapsLock>',
    [127] = '<LeftAlt>',
    [57] = ' ',
    [59] = '<F1>',
    [60] = '<F2>',
    [61] = '<F3>',
    [62] = '<F4>',
    [63] = '<F5>',
    [64] = '<F6>',
    [65] = '<F7>',
    [66] = '<F8>',
    [67] = '<F9>',
    [68] = '<F10>',
    [87] = '<F11>',
    [88] = '<F12>',
}

local function getButton(code, upper)
    if ch_map[code] then
        return ch_map[code]
    end
    code = code - KEY_1
    code = code + 1 -- Lua's indexes start from 1
    if upper then
        return ch_table_upper[code] or '?'
    else
        return ch_table[code] or '?'
    end
end

return function(code_and_upper)
    local code = math.floor(code_and_upper / 2)
    local upper = (code_and_upper % 2) == 1
    local text = getButton(code, upper)
    return text
end
