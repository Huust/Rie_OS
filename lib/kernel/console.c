#include "./console.h"

static struct lock console_lock;

/*
@notes:
    很简陋的控制台;
    我们常说给资源上锁,这里就是在给控制台上锁
    控制台实际上就是显存,所以说白了就是显存保护
*/


/* 控制台初始化其实就是初始化一个控制台锁，谁拥有该锁即可使用控制台（访问显存） */
void console_init() 
{
    lock_init(&console_lock);
}
/* 获取终端 */
void console_acquire() 
{
    lock_acquire(&console_lock);
}
/* 释放终端 */
void console_release() 
{
    lock_release(&console_lock);
}
/* 终端中输出字符串 */
void console_puts(char* mystr)
{
    console_acquire();
    rie_puts(mystr);
    console_release();
}
/* 终端中输出字符 */
void console_putc(uint8_t mychar)
{
    console_acquire();
    rie_putc(mychar);
    console_release();
}
/* 终端中输出十六进制整数 */
void console_puti(uint32_t mynum) 
{
    console_acquire();
    rie_puti(mynum);
    console_release();
}