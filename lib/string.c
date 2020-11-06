#include "./stdint.h"
#include "./kernel/debug.h"
#include "./kernel/global.h"

/*  string.c
支持的内存和字符串处理函数:
memset()    memcpy()    memcmp()
strcpy()    strcmp()    strlen()    strcat()
strchr()    strrchr()   strchrs()
*/


void rie_memset(void* addr_,uint8_t value,uint32_t size)
{
    ASSERT(addr_ != NULL);
    uint8_t* addr = (uint8_t*)addr_;
    while(size-- > 0){
        *addr++ = value;
    }
}

void rie_memcpy(void* dst, const void* src, uint32_t size)
{
    ASSERT(dst != NULL && src != NULL);

    uint8_t* _dst = (uint8_t*) dst;
    const uint8_t* _src = (uint8_t*) src;

    while (size-- > 0) {
        *_dst++ = *_src++;
    }
}

int8_t rie_memcmp(void* a_,void* b_,uint32_t size)
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


char* rie_strcpy(char* dst_, const char* src_)
{
   ASSERT(dst_ != NULL && src_ != NULL);
   char* r = dst_;      //用来返回目的字符串起始地址
   while((*dst_++ = *src_++));
   return r;
}


int8_t rie_strcmp(void* a_, void* b_)
{
   ASSERT(a_ != NULL && b_ != NULL);
   
   uint8_t* a = (uint8_t*)a_;
   uint8_t* b = (uint8_t*)b_;

   while(*a != 0 && *a == *b){
        a++;
        b++;
   }
   return (*a < *b) ? (-1) : (*a > *b);
}


uint32_t rie_strlen(const char* str) 
{
   ASSERT(str != NULL);
   const char* p = str;
   while(*p++);     //ascii('\0') == 0
   return (p - str - 1);    //不将末尾的\0算入length
}


char* rie_strcat(const char* src_,char* dst_) 
{
   ASSERT(dst_ != NULL && src_ != NULL);
   char* str = dst_;
   while (*str++);
   --str;
   while((*str++ = *src_++));
   return dst_;
}


/* 从左到右查找字符串str中首次出现字符ch的地址*/
char* rie_strchr(const char* str, const uint8_t ch) 
{
   ASSERT(str != NULL);
   while (*str != 0) {
      if (*str == ch) {return (char*)str;}
      str++;
   }
   return NULL;
}


/*从后往前查找字符串str中首次出现字符ch的地址*/
char* rie_strrchr(const char* str, const uint8_t ch) 
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
uint32_t rie_strchrs(const char* str, uint8_t ch) 
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