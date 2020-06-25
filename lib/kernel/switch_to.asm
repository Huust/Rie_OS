[bits 32]
global switch_to
section .text

switch_to:
;fixme:我怀疑这个压栈顺序反了
push ebp
push ebx
push edi
push esi

mov eax, [esp+20]
mov [eax], esp

mov eax, [esp+24]
mov esp, [eax]

pop esi
pop edi
pop ebx
pop ebp

ret     ;这个ret的作用是切换到下一个线程
;fixme:那中断怎么退出？真就中断嵌套中断？？？
;fixme:作者认为第二次调用某一线程时nonsense空栈帧下面就是schedule返回地址，
;但这很明显出错，一方面中断压栈那部分内容到底存到了那里？以及之前调用某些函数就不要压栈了？？？