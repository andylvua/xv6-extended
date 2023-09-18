//
// Created by andrew on 9/18/23.
//
#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[], char *envp[]) {
    for (int i = 0; envp[i]; i++) {
        printf(1, "envp[%d] = %s\n", i, envp[i]);
    }
    exit();
}
