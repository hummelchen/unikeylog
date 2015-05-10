#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static FILE* file_to_close_ = 0;

void fileCloser(int signum) {
    if (file_to_close_) {
        fclose(file_to_close_);
        file_to_close_ = 0;
        abort();
    }
}

void setFileToClose(FILE* file) {
    file_to_close_ = file;
    signal(SIGTERM, fileCloser);
    signal(SIGINT, fileCloser);
}
