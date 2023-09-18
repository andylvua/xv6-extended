//
// Created by andrew on 9/18/23.
//
#include "types.h"
#include "user.h"

char **environ;

int __libc_start_main(int (*main) (int, char**, char**), int argc, char** argv) {
  environ = &argv[argc + 1];

  return main(argc, argv, environ);
}
