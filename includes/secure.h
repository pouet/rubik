#ifndef SECURE_H_INCLUDED
#define SECURE_H_INCLUDED

void eprintf(char * fmt, ...);
int aprintf(char * fmt, ...);
void * emalloc(size_t size, char * s);
void * ecalloc(size_t count, size_t size, char * s);
void * erealloc(void * ptr, size_t size, char * s);

#endif
