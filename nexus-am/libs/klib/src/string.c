#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  return 0;
}

char *strcpy(char* dst,const char* src) {
  /*char *tmp = dst;
  while ((*dst++ = *src++) != '\0');
  return tmp;*/
  size_t i;
  for(i=0;src[i]!='\0';i++)
    dst[i]=src[i];
  dst[i]='\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  return NULL;
}

char* strcat(char* dst, const char* src) {
  char *tmp = dst;
  while (*dst)
    dst++;
  while ((*dst++ = *src++) != '\0');
  return tmp;
}

int strcmp(const char* s1, const char* s2) {
  while(*s1 == *s2)
  {
    if(*s1 == '\0') return 0;
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  return 0;
}

void* memset(void* v,int c,size_t n) {
  char *xs = (char *)v;
  while(n--) {
    *xs++ = c;
  }
  return v; 
}

void* memcpy(void* out, const void* in, size_t n) {
  return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n){
  const  char *su1, *su2;
  int res = 0;
  for( su1 = s1, su2 = s2; 0 < n; ++su1, ++su2, n--)
    if ((res = *su1 - *su2) != 0)
      break;  
  return res;
}

#endif
