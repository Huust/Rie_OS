;--------------实现基本字符（串）的打印功能-------------
;整体流程：
;所有寄存器压栈处理　获取光标位置　获取需要打印的字符　判断打印的字符是否是回车换行、空格等特殊字符
;如果是，则进入特殊处理，如果不是，则调用显存输出到当前光标处
;判断是否需要滚屏　更新光标位置　寄存器弹栈，退出

VIDEO_RPI equ 0x0000
VIDEO_TI equ 0x0000
VIDEO_INDEX equ 0x0003
VIDEO_SELECTOR equ (VIDEO_INDEX<<3|VIDEO_TI|VIDEO_RPI)
;create the selector and give to gs
mov ax,VIDEO_SELECTOR
mov gs,ax   ;这里赋值的原因请page287
rie_putchar:
;push all to enter the function
pushad
;locate the cursor
mov dx,0x03d4
mov al,0x0e
out dx,al
mov dx,0x03d5
in al,dx
mov ah,al
;low 8-bit
mov dx,0x03d4
mov ax,0x0f
out dx,ax
mov dx,0x03d5
in al,dx
;load the location of cursor into bx
mov bx,ax
;
push ebp
mov ebp,esp
mov ecx,[ebp + 36]
;judge carriage_return   line_feed    backspace   common characters
;CR 是 0x0d,LF 是 0x0a,BS 是 0x08
cmp cl, 0x0d
jz .is_carriage_ret
cmp cl, 0x0a
jz .is_line_feed
cmp cl, 0x08
jz .is_backspace

jz .is_common_char


;-------------灵活利用bx寄存器做相应的字符处理----------------

.is_line_feed:
.is_carriage_ret:
xor dx, dx
mov ax, bx
mov si, 80
div si
sub bx, dx
.is_carriage_ret_end:
add bx, 80
cmp bx, 2000    
.is_line_feed_end:
jl .set_cursor

.is_backspace:
;写法类似common_char，但是最后不需要更改bx的值，也不需要更新cursor位置
shl bx
mov [gs:bx],0x20    ;ox20代表０，也可以用空格表示这是一个空字符
inc bx
mov [gs:bx],0x07
dec bx
shr bx
;update_cursor 可执行可不执行，因为backspace并没有改变cursor的位置
.is_common_char:
;由bx找到实际显存地址，写入两字节的字符信息后把bx除２转回成索引值index
shl bx
mov [gs:bx],cl
inc bx
mov [gs:bx],0x07
dec bx  ;其实这里如果不减１也可以，因为下一步右移除２自动去余数
shr bx
inc bx
;bx加一后需要更新cursor位置
cmp bx,2000
jl .update_cursor



.update_cursor:
;update cursor功能是将新的bx值重新返还给locate_cursor寄存器，以确保光标位置正确
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