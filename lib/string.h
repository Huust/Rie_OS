#ifndef _STRING_H_
#define _STRING_H_
void memset(void* addr_,uint8_t value,uint32_t size);
void memcpy(const void* src_,void* dest_,uint32_t size);
int8_t memcmp(void* a_,void* b_,uint32_t size);
char* strcpy(char* dst_, const char* src_);
int8_t strcmp(void* a,void* b);
uint32_t strlen(const char* str);
char* strcat(const char* src_,char* dst_);
char* strchr(const char* str, const uint8_t ch);
char* strrchr(const char* str, const uint8_t ch);
uint32_t strchrs(const char* str, uint8_t ch);
#endif