//
// Created by andrew on 9/6/23.
//
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf(2, "Usage: cp source_file destination_file\n");
    exit();
  }
  char *source = argv[1];
  char *destination = argv[2];
  int src_fd, dest_fd;
  int r, w = 0;
  char buf[1024];
  struct stat st;

  if (strcmp(source, destination) == 0) {
    printf(2, "cp: %s and %s are the same file\n", source, destination);
    exit();
  }
  if ((src_fd = open(source, O_RDONLY)) < 0) {
    printf(2, "cp: cannot open %s\n", source);
    exit();
  }
  if (fstat(src_fd, &st) < 0) {
    printf(2, "cp: cannot stat %s\n", source);
    close(src_fd);
    exit();
  }
  if (st.type != T_FILE) {
    printf(2, "cp: %s is not a file\n", source);
    close(src_fd);
    exit();
  }
  if ((dest_fd = open(destination, O_CREATE | O_WRONLY)) < 0) {
    printf(2, "cp: cannot create %s\n", destination);
    close(src_fd);
    exit();
  }

  while ((r = read(src_fd, buf, sizeof(buf))) > 0) {
    if ((w = write(dest_fd, buf, r)) != r) {
      break;
    }
  }
  if (r < 0 || w < 0) {
    printf(2, "cp: error copying %s to %s\n", source, destination);
  }
  close(src_fd);
  close(dest_fd);
  exit();
}
