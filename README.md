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
中断部分代码完成,调试debug中...... <br>
在调试中断的时候发现了`rie_putc()` `rie_puts()`潜在的bug,主要是因为**寄存器使用不规范,一些值没有清空导致的**<br>
此外`rie_puts()`仍然有一个令人头疼的bug无法fix,心烦.
