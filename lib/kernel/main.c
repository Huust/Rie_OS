#include "./print.h"
#include  "./all_init.h"
#include "./console.h"
#include "./process.h"
#include "./syscall.h"

void thread_a(void* arg);
void thread_b(void* arg);
void proc_a(void);
void proc_b(void);

int test_a = 0;
int test_b = 0;

int main(void)
{
    rie_puts("rieos kernel\r\n");
    all_init();
    thread_start("funca", thread_a, "arg2 ", 8);
    thread_start("funcb",thread_b, "arg3 ", 21);
    process_start("proc_a", proc_a, 15);
    process_start("proc_b", proc_b, 15);
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
        console_puts("test_a:");
        console_puti(test_a);
        console_puts("  ");
    }
}

void thread_b(void* arg)
{
    char* b = (char*)arg;
    while(1){
        console_puts("test_b:");
        console_puti(test_b);
        console_puts("  ");
    }
}

void proc_a(void)
{
    while(1){
        test_a = get_pid() ;
    }
}

void proc_b(void)
{
    while(1){
        test_b = get_pid() ;
    }
}