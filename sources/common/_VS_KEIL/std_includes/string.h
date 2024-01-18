#pragma once


char   *strcat(char *, const char * );
char   *strcpy(void *dest, const char * src);
char   *strlcat(char *dest, const char * src, unsigned int size);
int     strcmp(const void *, const void *);
float   atof(char *);
void    memset(void *, int value, unsigned int size);
void    memcpy(void *dest, const void *src, unsigned int);
int     memcmp(const void *, const void *, unsigned int);
void *  memmove( void * destptr, const void * srcptr, size_t num );
char   *strchr(const char * str, int c);
char    toupper(char);
int     sscanf(void *, void *, ...);
unsigned int  strlen(const char * );
