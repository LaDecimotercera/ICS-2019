#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while(*s++)
    len++;
  return len;
}

char *strcpy(char* dst,const char* src) {
  char *tmp = dst;
  while ((*dst++ = *src++) != '\0');
  return tmp;
}

char* strncpy(char* dst, const char* src, size_t n) {
  char *tmp = dst;
  while(n){
  if(!(*dst++ = *src++)) break; 
    n--; 
  }
  while(n--)
    *dst++ = '\0';  
  return tmp; 
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
  while(n && *s1 && *s2){
    if(*s1 != *s2)
      return (unsigned char)*s1 - (unsigned char)*s2;
    s1++;
    s2++;
    n--;
  }
  if(!n) return 0;  
  if(*s1) return 1;  
  return -1;     
}

void* memset(void* v,int c,size_t n) {
  char *xs = (char *)v;
  while(n--) {
    *xs++ = c;
  }
  return v; 
}

void* memcpy(void* out, const void* in, size_t n) {
  char *tmp = out;
  const char *s = in;
  while (n--)
        *tmp++ = *s++;
    return out;
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
