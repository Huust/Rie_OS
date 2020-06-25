# Rie_OS
Just for fun,a unix-like toy operating system.

## update date:2020-04-02(first commit)
第一次提交，实模式过渡到保护模式的代码完成；<br>
将环境从windows迁移到了linux。<br>
之后会更新loader，添加分页机制，开启虚拟内存。<br>
## update date:2020-04-08(second commit)
增加了页表，开启了虚拟内存 <br>
将gdt搬运到了高１GB地址，放入内核空间<br>
## update date:2020-04-11
读取elf文件头，将内核文件的segments按照program header进行部署<br>
现在内核只有一个while(1)；之后添加类printf函数后会整体编译debug一遍<br>
## update date:2020-04-14
添加了rie_putchar，主要考虑了普通字符打印和特殊字符如*回车换行、后退、空格* <br>
**但是还没有添加roll_screen的功能，不打算使用显示地址寄存器的功能而是直接暴力刷新**
## update date:2020-04-16
重新编译debug并且完善了mbr.asm，不过bochs显示和调试似乎有点小bug
## update date:2020-04-19
编译测试了过渡保护模式的部分，由于下文代码还没测试故先注释掉<br>
mbr.asm文件中修复了磁盘读写的小问题
## update date:2020-04-21
发现gdt地址加载错地方了，遂改之<br>
编译测试了页表部分<br>
## update date:2020-04-23
bootloader宣布完结（已调试通过，但代码结构和一些函数后期会做修改） <br>
之后就是c与assemble的联合使用；首先是`put_char()`以及*中断*的实现
## update date:2020-04-27
更新`rie_putc()`的实现并且在main.c上得到了测试;之后很快就会把`rie_puts()`以及`rie_puti()`提交
## update date:2020-04-30
~~中断部分代码完成,调试debug中......~~ *这一行commit30mins后基本bug解决,可以响应IR0上的时钟中断;基本中断功能完成* <br>  
在调试中断的时候发现了`rie_putc()` `rie_puts()`潜在的bug,主要是因为**寄存器使用不规范,一些值没有清空导致的** <br>
~~此外`rie_puts()`仍然有一个令人头疼的bug无法fix,心烦.~~ *太真实了,这一行commit不超过10分钟bug就修复了*
## update date:2020-05-04
五四青年节快乐! <br>
丰富了中断处理函数:将原来kernel.asm对中断的简单处理改成了**通过kernel.asm获得中断向量号,再做相应的地址偏移找到存放在数组中对应的中断处理函数入口地址**<br>
另外定时器8253部分暂时不准备去写<br>
一些书上有关中断的知识点写得比较乱,所以打算这两天整理一份**完整的中断部分编写记录**
## update date:2020-05-06
增加了开关中断函数与`panic( )`,用于使用`ASSERT( )`
## update date:2020-05-08
增加`rie_putc( )`用来打印32位整型数
## update date:2020-05-09
增加makefile
## update date:2020-06-09
时隔一个月才回来更新，惭愧惭愧。<br>
说一下进度吧：一个月内把字符串处理函数string.c完成；bitmap以及内存管理(**这部分因为自己太菜，理解和代码实现花了很长时间**)的分配部分完成(*内存释放回收这些之后再做*)<br>
**然而奈何学校的垃圾网，我发现我的code怎么也push不上来（热点也不行）。没办法，抽空或者回去再更新源码部分吧**<br>
哦对了,内存管理之后将开始实现**线程、用户进程以及基本的交互部分**

## update date:2020-06-13
依旧没有push源码<br>
**进度：**
- 完善了内存管理那部分，*主要是修复了一些bug*
- 增加了线程调度的基本功能，但还没有调试
- 增加了双向链表以及与链表相关的操作函数，为之后**多线程调度**做准备

## update date:2020-06-25
- 多线程调度一周前已完成,但是*GP Fault*一直都没有解决;没办法只能去翻i386 manual看看能不能获得一些灵感
- 其实多线程调度不加锁的确容易造成general protection,尤其是光标寄存器的值做为公共资源被多个线程之间相互竞争时.
- 但现在的问题是即使是main线程自己调度自己依然GP;我裂开了
