#include "./print.h"
#include  "./all_init.h"
#include "./console.h"
#include "./process.h"
#include "./syscall.h"
#include "../stdio.h"

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
        // console_puts("main1 ");
    }
    return 0;
}

void thread_a(void* arg)
{
        console_puts("I am thread_a, my pid is ");
        console_puti(get_pid());
        console_puts("\r\n");
    while(1);
}

void thread_b(void* arg)
{
        console_puts("I am thread_b, my pid is ");
        console_puti(get_pid());
        console_puts("\r\n");
    while(1);
}

void proc_a(void)
{
    char* name = "proc_a";
    printf("I am %s, my pid:%d\r\n", "proc_a", get_pid());
    while(1);
}

void proc_b(void)
{
    char* name = "proc_b";
    printf("I am %s, my pid:%d\r\n", "proc_b", get_pid());
    while(1);
}