#ifndef KEYLOGGER_CRYPTO_H_
#define KEYLOGGER_CRYPTO_H_

typedef struct CryptoState CryptoState;

void freeCrypto(CryptoState* state);

int headerLength(CryptoState* state);

// Encryption

CryptoState* initEncryption(const char* rsa_pubkey);

// return buffer
// caller must free the result
char* getHeader(CryptoState* state);

void writeData(CryptoState* state, char* buffer,
               const char* data, int size);

// Decryption

CryptoState* initDecryption(const char* rsa_privkey);

// data's size is headerLength()
// return if an error occured
int readHeader(CryptoState* state, const char* data);

void readData(CryptoState* state, char* buffer,
              const char* data, int size);

#endif
