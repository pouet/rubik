#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "secure.h"

void eprintf(char * fmt, ...) {
  va_list ap;
  
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  exit(EXIT_FAILURE);
}

int aprintf(char * fmt, ...) {
  va_list ap;
  int n;
  
  va_start(ap, fmt);
  n = vfprintf(stderr, fmt, ap);
  va_end(ap);
  return n;
}

void * emalloc(size_t size, char * s) {
  void * p;
  
  p = malloc(size);
  if (p == NULL) {
    if (s)
      eprintf(s);
    else
      eprintf("Erreur d'allocation de mémoire.\n");
  }
  
  return p;
}

void * ecalloc(size_t count, size_t size, char * s) {
  void * p;
  
  p = calloc(count, size);
  if (p == NULL) {
    if (s)
      eprintf(s);
    else
      eprintf("Erreur d'allocation de mémoire.\n");
  }
  
  return p;
}

void * erealloc(void * ptr, size_t size, char * s) {
  void * p;
  
  p = realloc(ptr, size);
  if (p == NULL) {
    if (s)
      eprintf(s);
    else
      eprintf("Erreur d'allocation de mémoire.\n");
  }
  
  return p;
}
