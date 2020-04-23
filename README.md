# Rie_OS
Just for fun,a unix-like toy operating system.

## update date:2020-04-02(first commit)
第一次提交，算是把实模式过渡到保护模式了；<br>
环境从windows搬到linux花费了点时间。<br>
比较匆忙，很多细节(尤其是内存合理规划)都没有注意；<br>
之后会更新loader，添加分页机制，开启虚拟内存。<br>
加油冲冲冲

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
撒花！bootloader宣布完结（已经调试通过，但代码结构和一些函数后期会修改） <br>
之后就是开始c与assemble的联合使用了；首先是`put_char()`以及*中断*的实现

