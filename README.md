# Rie_OS
Just for fun,a unix-like toy operating system.

## update date:2020-04-02
第一次提交，实模式过渡到保护模式的代码完成；<br>
将环境从windows迁移到了linux。<br>
之后会更新loader，添加分页机制，开启虚拟内存。<br>
## update date:2020-04-08
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
- 已解决:导致GP的一个原因是,在调用打印时,**把ds压栈了;导致ds还没来得及被修改回来时,中断来临;使用错误的ds触发了GP**

## update date:2020-06-28
- 暂时还没有增加*信号量*,对于线程调度自己这一块解决方法是**使用公共资源时开关中断**.
- 不过发现很久之前写的print.asm那部分的*基础打印函数*在很多地方都没有考虑全面,导致现在中断开启后,各种*GP Fault*
- **恐怕因为中断,需要重构print.asm了......**

## update date:2020-07-01
- 节日快乐! 2020已过半,而我的OS还不知何时才能写好,比预期慢了好多
- 本次更新后将支持信号量.存在些许bug,正在调试.

## update date:2020-07-03
- 增加了控制台(*其实还是那个bochs的小黑窗;只不过将来键盘驱动实现后,它能够担任输入输出设备;暂时称为console吧*) **还未调试**
- 下次会增加键盘驱动

## update date:2020-07-04
- 增加keyboard支持,之后会完善**键盘驱动部分**;
- 其实在线程调度完成后,就一直在为人机交互功能增加模块;*console->keyboard->环形缓冲区->用户进程*

## update date:2020-07-10
- 增加键盘驱动,晚点调试

## update date:2020-07-11
- 键盘驱动调试通过;**对于键盘的大部分按键支持(除了Fn功能键,以及一些特殊功能键不提供支持.因为也用不到)**

## update date:2020-07-13
- 增加了环形缓冲,给键盘输入的cmd提供可用于存储的buffer;*还未调试*

## update date:2020-07-17
- 增加TSS,主要是为了之后特权级切换,用户级线程实现

## update date:2020-10-22
回归。
- 增加process.c及头文件，主要涉及用户进程
- 用户进程还未调试；在这之前线程部分的bug仍然存在，心态爆炸
- 修复了一些bug

## update date:2020-10-29
- 修复涉及thread.c | sync.c的一些bug；
- 回顾了信号量与锁的实现
- 仍在收拾烂摊子，似乎很久之前中断打印的问题导致了现在一连串的毛病

## update date:2020-11-01
- 线程调度，线程同步这一部分告一段落，终于修复了bug

## update date:2020-11-02
- 改善process.c源码，修改bug中......

## update date:2020-11-03
- 涉及用户进程的源码已经补充完整，准备调试

## update date:2020-11-05
- 日常心态爆炸，明明应该没问题，但每次执行`process_exec()`前开中断就会出错，无语了，bug还没找到

## update date:2020-11-06
- 用户进程问题解决！！！
>原因1：错把bitmap结构体的一个成员类型写成`uint32_t`<br>
>原因2：因为在bootloader中的页表没有先清零，而进程页表在创建时已经清0，导致两份数据不一致，造成CR3加载错误

## update date:2020-11-08
- 新增加**系统调用**功能，目前系统调用功能表里只有`get_pid()`，后续还会添加的
- 系统调用部分编译通过，调试无误
- 预告：接下来会去实现`printf()`，为用户进程提供打印接口

## update date:2020-11-10
- 新增加了stdio.c，主要是对`itoa()` `sprintf()` `printf()`的实现
- 调试还没有结束，出现了一些bug
- 今天发现了一个棘手的问题，有关于用户进程内定义变量为什么会抛出异常......感觉又要调很久唉

## update date:2020-11-11
- 增加了内存管理部分：原先的内存仅支持以页框为单位分配，<br>本次添加了粒度更细的内存分配；通过数据结构管理已分配内存的每个内存块
- 代码还未调试，之后的内存释放是重头戏
- 昨天的问题描述有误，还未调试

## update date:2020-11-12
- 增加了sys_free(),提供内存释放
- 基本完成了内存管理的所有内容，但还没有调试

## update date:2020-11-13
- 发现了多进程调度存在的隐藏bug：页表好像无法正常切换；但是到目前为止依然不能定位切换失败的原因
- `printf()`倒是没有出现什么问题
- debug后发现两个进程运行前，页表地址没有问题，但是第二个进程的页表莫名其妙增加了一些映射？这是什么鬼
