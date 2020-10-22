[bits 32]
global switch_to
section .text

switch_to:
;fixme:我怀疑这个压栈顺序反了
push ebp
push ebx
push edi
push esi

mov eax, [esp+20]   ;eax获得第一个参数内容(cur_thread指针)
mov [eax], esp      ;当前esp值存到cur_thread指向的内存(即:thread_pcb结构体
                    ;的第一个元素stack_ptr)

mov eax, [esp+24]   ;获取next_thread指向的内容
mov esp, [eax]      ;同理,next_thread首地址内容为栈顶指针

pop esi
pop edi
pop ebx
pop ebp

ret     ;此时指针指向thread_stack结构体中的eip函数指针
        ;ret执行后将eip指针内容当做返回地址,实现函数跳转
;fixme:那中断怎么退出？真就中断嵌套中断？？？
;fixme:作者认为第二次调用某一线程是nonsense空栈帧下面就是schedule返回地址，
;但这很明显出错，一方面中断压栈那部分内容到底存到了那里？以及之前调用某些函数就不要压栈了？？？