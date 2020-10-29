#include "./print.h"
#include  "./all_init.h"
#include "./console.h"

void thread_a(void* arg);
void thread_b(void* arg);


int main(void)
{
    rie_puts("rieos kernel\r\n");
    all_init();
    // thread_start("funca", thread_a, "arg2 ", 8);
    // thread_start("funcb",thread_b, "arg3 ", 21);
    rie_intr_enable();
    while(1){
        // rie_intr_disable();
        // rie_puts("main1 ");
        // rie_intr_enable();
        // console_puts("main1 ");
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
