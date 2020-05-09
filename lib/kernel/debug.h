#ifndef _DEBUG_H_
#define _DEBUG_H_
//panic()申明
void panic(const char* filename,int line,const char* func,const char* condition);

//重新宏定义
#define PANIC(...) panic(__FILE__, __LINE__, __func__, __VA_ARGS__)

#ifdef ASSERT_DISABLE
#define ASSERT(CONDITION) ((void)0)
#else 
#define ASSERT(CONDITION)       \
    if(CONDITION){}             \
    else{PANIC(#CONDITION);}
#endif

#endif