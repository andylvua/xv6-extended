//
// Created by andrew on 9/6/23.
//
#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf(2, "Usage: mv source_file destination_file\n");
    return -1;
  }
  char *source = argv[1];
  char *destination = argv[2];

  if (link(source, destination) < 0) {
    printf(2, "mv: %s failed to move to %s\n", source, destination);
    return -1;
  }
  if (unlink(source) < 0) {
    printf(2, "mv: %s failed to remove\n", source);
    return -1;
  }
  return 0;
}
