;--------------实现基本字符（串）的打印功能-------------
;整体流程：
;所有寄存器压栈处理　获取光标位置　获取需要打印的字符　判断打印的字符是否是回车换行、空格等特殊字符
;如果是，则进入特殊处理，如果不是，则调用显存输出到当前光标处
;判断是否需要滚屏　更新光标位置　寄存器弹栈，退出

VIDEO_RPI equ 0x0000
VIDEO_TI equ 0x0000
VIDEO_INDEX equ 0x0003
VIDEO_SELECTOR equ (VIDEO_INDEX<<3|VIDEO_TI|VIDEO_RPI)
mov ax,VIDEO_SELECTOR
mov gs,ax

;-------rie_puts()---------
[bits 32]

global rie_putc
rie_putc:
pushad
;locate the cursor
;high 8-bit
mov dx,0x03d4
mov al,0x0e
out dx,al
mov dx,0x03d5
in al,dx
mov ah,al
;low 8-bit
mov dx,0x03d4
mov ax,0x0f
out dx,al
mov dx,0x03d5
in al,dx
;load the location of cursor into bx
mov bx,ax
mov ebp,esp
mov ecx,[ebp + 36]
;回车carriage_return:0x0d   换行line_feed:0x0a    
;后退backspace:0x08
cmp cl, 0x0d
jz .is_carriage_ret
cmp cl, 0x0a
jz .is_line_feed
cmp cl, 0x08
jz .is_backspace

jmp .is_common_char


;-------------灵活利用bx寄存器做相应的字符处理----------------

.is_carriage_ret:
mov ax,bx
mov dl,80
div dl
and ax,(1111_1111_0000_0000B)
shr ax,8
sub bx,ax
jmp .update_cursor

.is_line_feed:
;需要考虑roll_screen的情况
mov dx,bx
add dx,80
cmp dx,2000
jnb .roll_screen
mov bx,dx
cmp bx,2000
jb .update_cursor

.is_backspace:
dec bx
shl bx,1
mov byte [gs:bx],0x20    ;asci(ox20) == 0,代表空字符
mov byte [gs:bx+1],0x07
shr bx,1
jmp .update_cursor

.is_common_char:
;由bx找到实际显存地址
;写入两字节的字符信息后把bx除２转回成索引值index
shl bx,1
mov byte [gs:bx],cl
mov byte [gs:bx+1],0x07
shr bx,1
inc bx
cmp bx,2000
jb .update_cursor
cmp bx,2000 ;再执行cmp的原因是不确定eflag已刷新
sub bx,80
jnb .roll_screen


.roll_screen:
;滚屏是(1~24)-->(0~23)并将最后一行清空
cld
mov ecx,(24*80)
;reg已经全部压栈,可修改ds与es
mov ax,gs
mov ds,ax
mov es,ax
mov esi,80
mov edi,0
rep movsw
inc edi
mov ecx,80
.clear_last_line:
mov dword [gs:edi],0x0720
add edi,2
loop .clear_last_line


.update_cursor:
;update cursor功能是将新的bx值重新返还
;给locate_cursor寄存器，以确保光标位置正确
;high 8-bit
mov dx,0x03d4
mov al,0x0e
out dx,al
mov dx,0x03d5
mov al,bh
out dx,al
;low 8-bit
mov dx,0x03d4
mov ax,0x0f
out dx,ax
mov dx,0x03d5
mov al,bl
out dx,al


.putchar_terminate:
popad
ret

;-------rie_puts()---------
[bits 32]
global rie_puts
rie_puts:
push ebp 
push ecx
push ebx
mov ebp,esp
xor ecx,ecx
mov ebx,[ebp + 16]
.str_handler:
mov cl,[ebx]
cmp cl,0   ;判断是否为'\0'
jz .str_over
inc ebx
push ecx    ;压入rie_putc的字符参数
call rie_putc
add esp,4   ;因为是函数外部压栈所以需要手动调整esp的值
jmp .str_handler

.str_over:
pop ebx
pop ecx
pop ebp
ret


;-------rie_puti()---------
[bits 32]
global rie_puti
rie_puti:
push ebp
push ecx
push ebx
mov ebp,esp
xor ecx,ecx
mov ebx,[ebp + 16]
.str_handler:
mov cl,[ebx]
cmp cl,0   ;判断是否为'\0'
jz .str_over
inc ebx
push ecx    ;压入rie_putc的字符参数
call rie_putc
add esp,4   ;因为是函数外部压栈所以需要手动调整esp的值
jmp .str_handler

.str_over:
pop ebx
pop ecx
pop ebp
ret