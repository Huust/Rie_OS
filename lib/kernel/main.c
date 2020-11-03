#include "./print.h"
#include  "./all_init.h"
#include "./console.h"
#include "./process.h"

void thread_a(void* arg);
void thread_b(void* arg);
void proc_c(void);

int test_c = 0;

int main(void)
{
    rie_puts("rieos kernel\r\n");
    all_init();
    thread_start("funca", thread_a, "arg2 ", 8);
    thread_start("funcb",thread_b, "arg3 ", 21);
    process_start("funcc",proc_c, 15);
    rie_intr_enable();
    while(1){
        console_puts("main1 ");
    }
    return 0;
}

void thread_a(void* arg)
{
    char* a = (char*)arg;
    while(1){
        console_puts(a);
    }
}

void thread_b(void* arg)
{
    char* b = (char*)arg;
    while(1){
        console_puts(b);
    }
}

void proc_c()
{
    while(1){
        test_c++ ;
    }
}