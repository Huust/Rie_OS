#include "debug.h"
#include "interrupt.h"
#include "print.h"
void panic(char* file,int line,const char* func,const char* condition)
{
    rie_intr_disable();
    rie_puts("\n\n\n-----error-----\n");
    rie_puts("filename:");rie_puts(filename);rie_puts("\n");
    //TODO:rie_puti()还未实现
    //rie_puts("line:0x");put_int(line);rie_puts("\n");
    rie_puts("function:");rie_puts((char*)func);rie_puts("\n");
    rie_puts("condition:");rie_puts((char*)condition);rie_puts("\n");
    rie_puts("-----end-----\n");
    while(1);
}