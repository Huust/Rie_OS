;中断创建与开启的流程:
;编写中断处理程序,并且获得每个处理程序的地址(kernel.asm)
;编写idt,将处理程序地址填入idt中(interrupt.c)
;
%define no_error_code push 0
%define exist_error_code nop
extern rie_puts
extern handler_table
extern intr_handler
[bits 32]
section .data   ;此处data与下部分data在编译时合成一个
                ;segment;从而使得dd的内容就接在
                ;handler_entry_table的后面
intr_info db 0x0d,0x0a,"interrupt occur",0x00
global handler_entry_table
handler_entry_table:

;宏指令控制多组中断处理程序流程
%macro VECTOR 2

section .text
intr_number_%1:
%2
;向主片和从片发送结束指令
mov al,0x20
out 0xa0,al
out 0x20,al
push gs
push ds
push es
push fs
pushad
push %1
call [handler_table+(4*%1)]
add esp,4   ;针对push %1
popad
pop fs
pop es
pop ds
pop gs
;弹出错误码,因为iret弹栈时忽视error_code存在
add esp,4   ;针对push %2
iretd

section .data
dd intr_number_%1   ;这里保留空间存放地址
                    ;跟在handler_entry_table构成数组，中断
                    ;就是首先按照这个地址进入的，接着
                    ;call handler_table中的中断处理函数

%endmacro


VECTOR 0x00,no_error_code
VECTOR 0x01,no_error_code
VECTOR 0x02,no_error_code
VECTOR 0x03,no_error_code
VECTOR 0x04,no_error_code
VECTOR 0x05,no_error_code
VECTOR 0x06,no_error_code
VECTOR 0x07,no_error_code
VECTOR 0x08,exist_error_code    ;error_code
VECTOR 0x09,no_error_code
VECTOR 0x0a,no_error_code
VECTOR 0x0b,no_error_code
VECTOR 0x0c,no_error_code
VECTOR 0x0d,no_error_code
VECTOR 0x0e,no_error_code
VECTOR 0x0f,no_error_code
VECTOR 0x10,no_error_code
VECTOR 0x11,exist_error_code    ;error_code
VECTOR 0x12,no_error_code
VECTOR 0x13,no_error_code
VECTOR 0x14,no_error_code
VECTOR 0x15,no_error_code
VECTOR 0x16,no_error_code
VECTOR 0x17,no_error_code
VECTOR 0x18,no_error_code
VECTOR 0x19,no_error_code
VECTOR 0x1a,no_error_code
VECTOR 0x1b,no_error_code
VECTOR 0x1c,no_error_code
VECTOR 0x1d,no_error_code
VECTOR 0x1e,no_error_code
VECTOR 0x1f,no_error_code
VECTOR 0x20,no_error_code       ;0x20==32,这个专门留给IRQ0时钟
                                ;之所以只到0x20是因为目前只用到
                                ;IRQ0时钟，需要其它INTR以后加上