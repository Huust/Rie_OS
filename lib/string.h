#ifndef _STRING_H_
#define _STRING_H_
void rie_memset(void* addr_,uint8_t value,uint32_t size);
void rie_memcpy(const void* src_,void* dest_,uint32_t size);
int8_t rie_memcmp(void* a_,void* b_,uint32_t size);
char* rie_strcpy(char* dst_, const char* src_);
int8_t rie_strcmp(void* a_, void* b_);
uint32_t rie_strlen(const char* str);
char* rie_strcat(const char* src_,char* dst_);
char* rie_strchr(const char* str, const uint8_t ch);
char* rie_strrchr(const char* str, const uint8_t ch);
uint32_t rie_strchrs(const char* str, uint8_t ch);
#endif