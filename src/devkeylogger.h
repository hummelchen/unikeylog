#ifndef KEYLOGGER_DEV_H
#define KEYLOGGER_DEV_H

typedef struct DevKeylogger DevKeylogger;

DevKeylogger* initDevKeylogger();

void freeDevKeylogger(DevKeylogger* logger);

int catchDevKey(DevKeylogger* logger);

#endif
