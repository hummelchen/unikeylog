#ifndef KEYLOGGER_X_H
#define KEYLOGGER_X_H

typedef struct XKeylogger XKeylogger;

XKeylogger* initXKeylogger();

void freeXKeylogger(XKeylogger* logger);

int catchXKey(XKeylogger* logger);

#endif
