;中断创建与开启的流程:
;编写中断处理程序,并且获得每个处理程序的地址(kernel.asm)
;编写idt,将处理程序地址填入idt中(interrupt.c)
;
%define no_error_code push 0
%define exist_error_code nop
extern rie_puts

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
push intr_info
call rie_puts
add esp,4
;向主片和从片发送结束指令
mov al,0x20
out 0xa0,al
out 0x20,al
;弹出错误码,因为iret弹栈时忽视error_code存在
add esp,4
iret

section .data
dd intr_number_%1

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
VECTOR 0x20,no_error_code