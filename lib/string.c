#include "./stdint.h"
#include "./kernel/debug.h"
#include "./kernel/global.h"

/*  string.c
支持的内存和字符串处理函数:
memset()    memcpy()    memcmp()
strcpy()    strcmp()    strlen()    strcat()
strchr()    strrchr()   strchrs()
*/


void memset(void* addr_,uint8_t value,uint32_t size)
{
    ASSERT(dst_ != NULL);
    uint8_t* addr = (uint8_t*)addr_;
    while(size-- > 0){
        *addr++ = value;
    }
}


void memcpy(const void* src_,void* dest_,uint32_t size)
{
    ASSERT(dst_ != NULL && src_ != NULL);
    const uint8_t* src = (uint8_t*)src_;
    uint8_t* dest = (uint8_t*)dest_;
    while(size-- > 0){
        *dest++ = *src++;
    }
}


int8_t memcmp(void* a_,void* b_,uint32_t size)
{
    uint8_t* a = (uint8_t*)a_;
    uint8_t* b = (uint8_t*)b_;
    while(size-- > 0){
        if(*a != *b) {return *a>*b?1:-1;}
        a++;
        b++;
    } 
    return 0;   
}


char* strcpy(char* dst_, const char* src_)
{
   ASSERT(dst_ != NULL && src_ != NULL);
   char* r = dst_;      //用来返回目的字符串起始地址
   while((*dst_++ = *src_++));
   return r;
}


int8_t strcmp(void* a,void* b)
{
    ASSERT(a != NULL && b != NULL);
    while(*a != 0 && *a == *b){
        a++;
        b++;
    }
    return *a < *b ? −1 : *a > *b;
}


uint32_t strlen(const char* str) 
{
   ASSERT(str != NULL);
   const char* p = str;
   while(*p++);     //ascii('\0') == 0
   return (p - str - 1);    //不将末尾的\0算入length
}


char* strcat(const char* src_,char* dst_) 
{
   ASSERT(dst_ != NULL && src_ != NULL);
   char* str = dst_;
   while (*str++);
   --str;
   while((*str++ = *src_++));
   return dst_;
}


/* 从左到右查找字符串str中首次出现字符ch的地址*/
char* strchr(const char* str, const uint8_t ch) 
{
   ASSERT(str != NULL);
   while (*str != 0) {
      if (*str == ch) {return (char*)str;}
      str++;
   }
   return NULL;
}


/*从后往前查找字符串str中首次出现字符ch的地址*/
char* strrchr(const char* str, const uint8_t ch) 
{
   ASSERT(str != NULL);
   const char* last_char = NULL;
   while (*str != 0) {
      if (*str == ch) {
	 last_char = str;
      }
      str++;
   }
   return (char*)last_char;
}


/*在字符串str中查找指定字符ch出现的次数*/
uint32_t strchrs(const char* str, uint8_t ch) 
{
   ASSERT(str != NULL);
   uint32_t ch_cnt = 0;
   const char* p = str;
   while(*p != 0) {
      if (*p == ch) {
	 ch_cnt++;
      }
      p++;
   }
   return ch_cnt;
}