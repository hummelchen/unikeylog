local ffi = require("ffi")

-- импортируем функции malloc и free
ffi.cdef[[
void *malloc(size_t size);
void free(void *ptr);
]]

-- импортируем функции и структуры из crypto.h
ffi.cdef("\
typedef struct CryptoState CryptoState;\
\
void freeCrypto(CryptoState* state);\
\
int headerLength(CryptoState* state);\
\
// Encryption\
\
CryptoState* initEncryption(const char* rsa_pubkey);\
\
// return buffer\
// caller must free the result\
char* getHeader(CryptoState* state);\
\
void writeData(CryptoState* state, char* buffer,\
               const char* data, int size);\
\
// Decryption\
\
CryptoState* initDecryption(const char* rsa_privkey);\
\
// data's size is headerLength()\
// return if an error occured\
int readHeader(CryptoState* state, const char* data);\
\
void readData(CryptoState* state, char* buffer,\
              const char* data, int size);\
")

local crypto = ffi.load("./crypto.so")

-- объявляем нечто вроде класса
local CryptoState = ffi.metatype("CryptoState", {
    -- финализатор - вместо деструктора
    __gc = crypto.freeCrypto,

    -- члены класса (методы)
    __index = {
        initEncryption = crypto.initEncryption,

        getHeader = function(self)
            local header_c = crypto.getHeader(self)
            local header = ffi.string(header_c,
                self:headerLength())
            ffi.C.free(header_c)
            return header
        end,

        headerLength = crypto.headerLength,

        writeData = function(self, data)
            local encrypted = ffi.new("uint8_t[?]", #data)
            crypto.writeData(self, encrypted, data, #data)
            return ffi.string(encrypted, #data)
        end,

        initDecryption = crypto.initDecryption,

        readHeader = crypto.readHeader,

        readData = function(self, data)
            local decrypted = ffi.new("uint8_t[?]", #data)
            crypto.readData(self, decrypted, data, #data)
            return ffi.string(decrypted, #data)
        end,
    }
})

return CryptoState
