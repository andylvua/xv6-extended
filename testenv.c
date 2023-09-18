//
// Created by andrew on 9/18/23.
//
#include "types.h"
#include "user.h"

extern char **environ;

int main() {
    for (int i = 0; environ[i] != 0; i++) {
        printf(1, "environ[%d] = %s\n", i, environ[i]);
    }
    exit();
}
