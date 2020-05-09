#include "debug.h"
#include "interrupt.h"
#include "print.h"
void panic(const char* filename,int line,const char* func,const char* condition)
{
    rie_intr_disable();
    rie_puts("\n\n\n-----error-----\n\r");
    rie_puts("filename:");rie_puts((char*)filename);rie_puts("\n\r");
    rie_puts("line:");rie_puti(line);rie_puts("\n\r");
    rie_puts("function:");rie_puts((char*)func);rie_puts("\n\r");
    rie_puts("condition:");rie_puts((char*)condition);rie_puts("\n\r");
    rie_puts("------end------\n\r");
    while(1);
}