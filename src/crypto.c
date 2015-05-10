#include <stdlib.h>
#include <string.h>

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>

#include "crypto.h"

// C have no true const
#define AES128_KEY_SIZE 16
#define IV_SIZE 8

void printHex(unsigned char b[], int lim) {
    int i;
    for (i = 0; i < lim; i++) {
        printf("%02X", (unsigned int)b[i]);
    }
    printf("\n");
}

typedef struct CryptoState {
    AES_KEY aes_key;
    RSA* rsa_priv_key;
    unsigned char aes_key_str[AES128_KEY_SIZE];
    unsigned char* aes_key_rsa_encrypted;
    int rsa_size;
    unsigned char ivec[AES_BLOCK_SIZE];
    unsigned char ecount[AES_BLOCK_SIZE];
    unsigned int num;
} CryptoState;

CryptoState* emptyCryptoState() {
    CryptoState* state = malloc(sizeof(CryptoState));
    memset(state, 0, sizeof(CryptoState));
    return state;
}

void freeCrypto(CryptoState* state) {
    if (state) {
        if (state->aes_key_rsa_encrypted) {
            free(state->aes_key_rsa_encrypted);
        }
        if (state->rsa_priv_key) {
            RSA_free(state->rsa_priv_key);
        }
        free(state);
    }
}

int headerLength(CryptoState* state) {
    return state->rsa_size + IV_SIZE;
}

// return if error occured
int encryptWithRsa(CryptoState* state,
        const char* rsa_pubkey_str) {
    BIO* keybio = BIO_new_mem_buf((char*)rsa_pubkey_str, -1);
    if (keybio == NULL) {
        return 1;
    }
    RSA* rsa = PEM_read_bio_RSA_PUBKEY(keybio,
            NULL, NULL, NULL);
    if (!rsa) {
        return 1;
    }
    state->rsa_size = RSA_size(rsa);
    state->aes_key_rsa_encrypted = malloc(state->rsa_size);
    RSA_public_encrypt(
            AES128_KEY_SIZE,
            state->aes_key_str,
            state->aes_key_rsa_encrypted,
            rsa, RSA_PKCS1_PADDING);
    BIO_free(keybio);
    RSA_free(rsa);
    return 0;
}

CryptoState* initEncryption(const char* rsa_pubkey_str) {
    CryptoState* state = emptyCryptoState();
    // generate random AES 128 key
    RAND_bytes(state->aes_key_str, AES128_KEY_SIZE);
    if (AES_set_encrypt_key(state->aes_key_str, 128,
                &state->aes_key) != 0) {
        free(state);
        return 0;
    }
    // encrypt AES key with RSA
    if (encryptWithRsa(state, rsa_pubkey_str) != 0) {
        free(state);
        return 0;
    }
    // initialise AES CTR encryption
    memset(state->ivec, 0, AES_BLOCK_SIZE);
    RAND_bytes(state->ivec, IV_SIZE);
    memset(state->ecount, 0, AES_BLOCK_SIZE);
    // return
    state->rsa_priv_key = 0;
    return state;
}

// return buffer
// caller must free the result
char* getHeader(CryptoState* state) {
    char* header = malloc(headerLength(state));
    memcpy(header, state->aes_key_rsa_encrypted,
           state->rsa_size);
    const unsigned char* iv = state->ivec;
    memcpy(header + state->rsa_size, iv, IV_SIZE);
    return header;
}

void writeData(CryptoState* state, char* buffer,
               const char* data, int size) {
    AES_ctr128_encrypt((const unsigned char*)data,
            (unsigned char*)buffer,
            size, &state->aes_key,
            state->ivec, state->ecount, &state->num);
}

CryptoState* initDecryption(const char* rsa_priv_str) {
    CryptoState* state = emptyCryptoState();
    // set RSA key
    BIO* keybio = BIO_new_mem_buf((char*)rsa_priv_str, -1);
    if (keybio == NULL) {
        return 0;
    }
    state->rsa_priv_key = PEM_read_bio_RSAPrivateKey(keybio,
            NULL, NULL, NULL);
    state->rsa_size = RSA_size(state->rsa_priv_key);
    BIO_free(keybio);
    // return
    return state;
}

int readHeader(CryptoState* state, const char* data) {
    // get AES key from header
    if (RSA_private_decrypt(
            state->rsa_size,
            (const unsigned char*)data,
            state->aes_key_str,
            state->rsa_priv_key,
            RSA_PKCS1_PADDING) == -1) {
        return 1;
    }
    // make AES key
    if (AES_set_encrypt_key(state->aes_key_str, 128,
                &state->aes_key) != 0) {
        free(state);
        return 1;
    }
    // initialise AES CTR decryption
    const char* iv = data + state->rsa_size;
    memset(state->ivec, 0, AES_BLOCK_SIZE);
    memcpy(state->ivec, iv, IV_SIZE);
    memset(state->ecount, 0, AES_BLOCK_SIZE);
    return 0;
}

void readData(CryptoState* state, char* buffer,
        const char* data, int size) {
    AES_ctr128_encrypt((const unsigned char*)data,
            (unsigned char*)buffer,
            size, &state->aes_key,
            state->ivec, state->ecount, &state->num);
}
