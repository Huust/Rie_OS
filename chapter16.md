# 系统调用
系统调用是出于安全性的角度：OS对用户进程极其不放心，但为了提高用户进程的执行力（处理各种任务，减少对用户进程的约束），OS为用户提供了一组借接口，即系统调用
`balance(computer_security, process_flexibility) -> syscall`

## 实现的基本原理
- 模仿linux的`syscall()`，传入参数为子功能号+你需要传递给系统调用的参数
- 但是我们要使用的是`_syscall()`，该函数已被linux舍弃，因为**它支持可变参数数量有限（最多7个），并且还需要传入每一个参数的类型，较为复杂；此外该函数存在安全漏洞**

## 系统调用实现框架
- 1.设计宏函数_syscall，该函数由用户进程调用，函数的核心代码是**将子功能号传入eax，执行int 0x80中断**
- 2.因为第一步中执行int 0x80软件中断，**所以要在idt中先存入0x80位置的中断入口函数**
- 3.中断入口函数将子功能号传入，通过子功能号做索引偏移，找到实际执行具体系统调用的那个函数的入口
- 4.具体实现的那个函数是属于内核态的
>以某个process调用get_pid为例：`get_pid()`-->`syscall()`-->`mov eax, 子功能号; int 0x80`-->**进入int 0x80的入口函数**-->**通过索引（比如一个函数指针数组）找到具体的实现函数`sys_getpid()`**-->返回pid

>1.`get_pid()`加上前缀`sys_`代表这是处在内核态的具体实现的函数，而`get_pid()`只是用户态调用的第一层函数
>2.`int 0x80`是软中断。首先，int是x86原生支持的汇编指令，该指令执行后将去idt中找到int后面数值所对应的中断入口函数并且执行；软中断可以自己触发，而硬件中断是硬件达到某个状态时，首先通知8259A，再由8259A通知CPU去执行对应位置的中断函数。

## 具体实现步骤
- 1.在idt中加入0x80号中断，为了执行int 0x80时可以跳转到统一的syscall入口函数
>这里有一个问题：**为什么设置`syscall_entry()`对应的IDT中描述符的DPL为3，而不是0？明明已经时处在中断中的呀**
>解答：执行`syscall()`时处在中断中是正确的。但是，IDT中某个描述符属性位的DPL为3**不代表所对应的入口函数DPL为3，只代表这个IDT描述符的DPL为3**
>![](_v_images/20201108102727285_42218862.png)
图中不难看出，用户进程代码段处在R3，所以它只能访问处在R3的数据段（**这就解释了为什么DPL==3：你先要去IDT中访问那个描述符，如果描述符的DPL<3,那就无法访问了**）
访问了DPL==3的描述符后，**才是去R0执行中断入口函数**，这也符合要求，因为cpu允许通过门实现代码段从低到高的转移
若是因为外部原因（硬件中断等），则直接检查当前代码段CPL和中断入口函数的DPL（**而跳过检查IDT描述符的DPL这一步**）

- 2.设计宏函数，给用户进程使用
>宏函数需要做几件事情：
>申请0x80中断
>将参数压入寄存器而不是栈
>确保返回值正常返回
第二点的原因：**如果是压栈，则压入的是三特权级栈；随后进入R0态代码，你需要参数时就必须先切换到R3栈获取参数值；这样做比较费时**
第三点原因：**cdecl规定返回值用eax存储。因此自己写的汇编也应该尽量遵循这个ABI规则，用eax**

- 3.完善系统调用的中断入口函数(在kernel.asm中)



- 4.给pcb加上pid属性
>赋值时，可以使用static型变量，每次调用`allocate_pid()`时，自增1，又因为是static所以值会一直保持
>这也意味着，**线程/进程的创建顺序决定了它的pid的值**；比如main_thread的pid就是0,因为它是第一个被创建的
>为什么在对pid赋值时要上锁：理论上说，pcb_enroll()使用的位置中断还没有开启，应该不存在资源竞争的问题
>但是，在后面我们实现fork这些东西的时候，会通过进程创建进程，这时锁就很有必要了


# 让用户进程说话--实现`printf()`
- 1.回顾一下**可变参数**
>在实现断言函数时，我们将...省略号放到参数列表中，表示变长参数。但是函数的定义处，又有一个变量__VA_ARGS__，该变量**是一个字符串常量指针，它指向的就是可变参数的参数集合（就是你实际调用函数时写入的参数，把他们看成字符串后这个指针指向它）**
>因此condition前面加#将其转换成字符串

- 2.具体讲一讲C是如何支持可变参数的
>角度0：C99标准支持这种...省略号表示可变长参数的写法。因此才有了角度1

>角度1编译器的语法检查和编译成汇编：语法检查部分，个人认为应该是对可变参数的函数打了label，方便编译器在检查到该函数被调用时，避免因为参数个数而报错；
>编译成汇编：对于函数体的定义处编译，依然是打label；对于被调用处，**因为你调用这个函数，必然将所需要的参数都填在了括号内，所以编译器就可以根据这个来饥饿确定应该压多少次栈帧，内部实际调用的esp偏移量等等都可以确定了**

>角度2函数编写者：首先函数编写者肯定不知道可变参数的数量
>但是`printf()`中第一个必填的参数格式化字符串里面，**认为有多少个%就有多少个参数**
>有人会问：如果个数不匹配咋办？
>首先编译器也会根据%和参数个数判断你的调用是否正确
>就算编译器不检查，那实现也是你自己的事情，你想怎么办就怎么办

>角度3decl规则：根据decl规则，参数是从右向左依次压入的
>这就为printf()提供了便利：**因为我们的第一个参数format是已知的，函数体内你只要对第一个参数取地址，即可获得它所处在的位置；进而只要指针+4，就可以获得上一个栈帧的位置，也就是第二个参数**，这也正是通过可变参数实现printf的原理:读取可变参数的过程其实就是在堆栈中，使用指针,遍历堆栈段中的参数列表,从低地址到高地址一个一个地把参数内容读出来的过程.
>有人可能会问，如果我第二个参数是char型，但是栈帧固定4字节，那么我用把指针改为char*后再用\*号读取该字节值，会不会受到字节对齐方式的影响？
>**并不会。我们的代码是小端序，而指针指向的也是起始地址，改为char\*后刚好可以取低地址处的一个字节。但是如果是大端序就不对了**

## `printf()`的具体实现
`printf()`称为格式化输出函数；顾名思义，它是由两个最主要的函数构成：格式化函数`sprintf()`和输出函数`write()`
- 1.输出函数
>在将来我们的输出函数支持输出到文件描述符，但是现在还没有实现file system，所以只能通过syscall调用console_puts()实现控制台输出
>
- 2.格式化函数