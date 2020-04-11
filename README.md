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
