local pubkey = [[
-----BEGIN PUBLIC KEY-----
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCeYM5oXEtEXOvejmTUsE9hN0sV
Jk9amBOJ/EPHKLYh15WPm0nEegH4fJvFSWqx6ts70iPpp0SZNF0ZuKzhEdE+t+Tq
arH/JSe92FnCGwVBxqwC0pjimeZd8+n5s2zBLztxdcxh+GYKaMKI6Tay017/uTCy
XCmEuDrgs+zhfWfF3QIDAQAB
-----END PUBLIC KEY-----
]]

local privkey = [[
-----BEGIN RSA PRIVATE KEY-----
MIICXAIBAAKBgQCeYM5oXEtEXOvejmTUsE9hN0sVJk9amBOJ/EPHKLYh15WPm0nE
egH4fJvFSWqx6ts70iPpp0SZNF0ZuKzhEdE+t+TqarH/JSe92FnCGwVBxqwC0pji
meZd8+n5s2zBLztxdcxh+GYKaMKI6Tay017/uTCyXCmEuDrgs+zhfWfF3QIDAQAB
AoGARl6e93Gio7JhvtYn6PSZoIlP3ArUkV0muVWTy+bD3g+KzW7i/OB1BbJgB+2R
8ZF3bYH2IPgIG8ZPY4FRlwMyo+c+Fgn0oILO4GunT0MYxNp9E7KgSquugrqIFCvX
LhCUXGuYD/uPaK3muIt5hJO71GyuSa+JKmpgxd6dLMu8pOECQQDRuu2nOaA8kVbW
L97J3qthQi2gdYd4HwZn7+4wf1dGINFaIB8u78VLhPAFKVkr9oWqJ4/d5IMTdCVA
QNIWfhGVAkEAwVGidPBGKBgOvK2KpGo0FBMw2ZcRNpGqjAx7vpwg1C9tF2Mehnnd
FOBZvVwzaJ2VvRwxBvPVt2vjKyIJ4/fhKQJALz9ZQo7s1jg3w9DM89VB7osmFDvl
5N24Z7nZgvttAdNq1k7eN4i0farZF5CVEh5bRlXbRzWvgbZsnej8ayzDcQJAOPdF
WBA1nDr6nL7GzAw/rGq7SmMpv1rNVteW32RCiEQxcWLT6+ydKTmzOAmjXICTT1av
W80efR5doqg1Ieg5gQJBAJz2QSxoAC2s7w27HWsEGo8lGMFjfSGrVRIwxqP0iXMi
LjbLctPFjOC+Tv1FarUtFgavklMtBq71caTNr4SGT1w=
-----END RSA PRIVATE KEY-----
]]

local crypto = require 'crypto'

local encryptor = crypto.initEncryption(pubkey)
local header = encryptor:getHeader()
local plaintext1 = "plain text"
local plaintext2 = "XXX YYY"
local cryptotext1 = encryptor:writeData(plaintext1)
local cryptotext2 = encryptor:writeData(plaintext2)

local decryptor = crypto.initDecryption(privkey)
decryptor:readHeader(header)
local plaintext1a = decryptor:readData(cryptotext1)
local plaintext2a = decryptor:readData(cryptotext2)

assert(plaintext1 == plaintext1a)
assert(plaintext2 == plaintext2a)
