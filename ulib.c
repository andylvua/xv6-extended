#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "param.h"

extern char **environ;

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

int
strncmp(const char *p, const char *q, uint n)
{
  while(n > 0 && *p && *p == *q)
    n--, p++, q++;
  if(n == 0)
    return 0;
  return (uchar)*p - (uchar)*q;
}

void
strcat(char *dest, const char *src) {
  while (*dest) {
    dest++;
  }
  while (*src) {
    *dest = *src;
    dest++;
    src++;
  }
  *dest = '\0';
}

char*
strtok(char *str, const char *delim) {
  static char *next = 0;
  char *token;

  if (str == 0 && (next == 0 || *next == '\0')) {
    return 0;
  }

  if (str != 0) {
    next = str;
  }

  token = next;
  while (*next != '\0' && strchr(delim, *next) == 0) {
    next++;
  }

  if (*next != '\0') {
    *next = '\0';
    next++;
  }

  return token;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}

char*
getenv(const char *name)
{
  int i, len;

  len = strlen(name);
  for (i = 0; environ[i] != 0; i++) {
    if (strncmp(environ[i], name, len) == 0 && environ[i][len] == '=') {
      return environ[i] + len + 1;
    }
  }
  return 0;
}

int
setenv(const char *name, const char *value, int overwrite) {
  if (name == 0 || value == 0)
    return -1;

  if (strlen(name) > ENVKEY_MAX || strlen(value) > ENVVAL_MAX) {
    printf(2, "The name or value of the environment variable is too long\n");
    return -1;
  }

  int entry_len = strlen(name) + 1 + strlen(value) + 1;

  for (int i = 0; environ[i] != 0; i++) {
    char *entry = environ[i];
    if (strncmp(entry, name, strlen(name)) == 0 && entry[strlen(name)] == '=') {
      if (overwrite) {
        char *new_entry = malloc(entry_len);
        if (new_entry == 0)
          return -1;
        strcpy(new_entry, name);
        strcat(new_entry, "=");
        strcat(new_entry, value);
        environ[i] = new_entry;
      }
      return 0;
    }
  }

  char *new_entry = malloc(entry_len);
  if (new_entry == 0)
    return -1;
  strcpy(new_entry, name);
  strcat(new_entry, "=");
  strcat(new_entry, value);

  int i;
  for (i = 0; environ[i] != 0; i++) {}

  char **new_environ = malloc((i + 2) * sizeof(char *));
  if (new_environ == 0) {
    free(new_entry);
    return -1;
  }

  for (int j = 0; j < i; j++) {
    new_environ[j] = environ[j];
  }

  new_environ[i] = new_entry;
  new_environ[i + 1] = 0;
  int new_environ_len = 0;
  for (int j = 0; new_environ[j] != 0; j++) {
    new_environ_len += 1;
  }

  free(environ);
  environ = new_environ;
  return 0;
}
