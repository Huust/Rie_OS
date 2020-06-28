;--------------实现基本字符（串）的打印功能-------------
;整体流程：
;所有寄存器压栈处理　获取光标位置　获取需要打印的字符　判断打印的字符是否是回车换行、空格等特殊字符
;如果是，则进入特殊处理，如果不是，则调用显存输出到当前光标处
;判断是否需要滚屏　更新光标位置　寄存器弹栈，退出

VIDEO_RPI equ 0x0000
VIDEO_TI equ 0x0000
VIDEO_INDEX equ 0x0003
VIDEO_SELECTOR equ (VIDEO_INDEX<<3)+VIDEO_TI+VIDEO_RPI

;-------rie_putc()---------
[bits 32]

global rie_putc
rie_putc:
pushad
mov ax,VIDEO_SELECTOR
mov gs,ax
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
mov al,0x0f
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


;-------------灵活利用bx寄存器对rie_putc传入的字符处理----------------

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
xor dx,dx
mov dx,bx
add dx,80
cmp dx,2000
jnb .roll_screen
xor bx,bx
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
mov ecx,(24*40)     ;循环次数取决于每次mov w/d
;这里的trick是因为ds,es==0,所以直接用esi,edi表示
;也表明:任何段选择子和偏移量都可以任意搭配,哪种搭配都可以表示同一地址
mov esi,0xc00b80a0
mov edi,0xc00b8000
rep movsd

mov edi,3840
mov ecx,80
.clear_last_line:
mov word [gs:edi],0x0720
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
mov al,0x0f
out dx,al
mov dx,0x03d5
mov al,bl
out dx,al


.putchar_terminate:
popad
ret

;---------------rie_puts()-----------------
;实现思路比较简单,主要就是在编译时,编译器会将字符串单独放在
;一个内存块中,所以压栈时压的是字符串指针;这样就变成了多次put_char了
;另外,这段代码配合编译器为字符串自动加0的功能,获得字符串的结束位置
[bits 32]
global rie_puts
rie_puts:
push ecx
push ebx
xor ecx,ecx
xor ebx,ebx
mov ebx,[esp + 12]
.str_handler:
mov cl,[ebx]
cmp cl,0   ;判断是否为'\0'
jz .str_over
push ecx    ;压入rie_putc的字符参数
call rie_putc
add esp,4   ;因为是函数外部压栈所以需要手动调整esp的值
inc ebx
jmp .str_handler

.str_over:
pop ebx
pop ecx
ret


;---------------rie_puti()-----------------
;实现思路:
;不同于rie_puts(),打印uint32_t类型还需要做ascii码转换
;具体而言就是对于32位数分成8个4bit(正好是16进制的一位)
;每一小块都要转换成ascii码并且存放到buffer中;
;最后还需要将多余的前导0去除;最终实现16进制格式的字符串打印
[bits 32]
global rie_puti
print_buffer dq 0
prefix_0x db "0x",0
rie_puti:
push prefix_0x
call rie_puts
add esp,4
pushad
mov ebp,esp
mov edi,[ebp+36]    ;获取打印参数
mov ecx,8
mov esi,0
.handle_integer:
mov eax,edi
and eax, 0x0000000f
cmp eax,9
jna .is_number
add al,'W'
jmp .store_in_buffer

.is_number:
add al,'0'

.store_in_buffer:
mov [print_buffer+esi],al
shr edi,4
inc esi
loop .handle_integer

mov ecx,8
mov esi,7
xor ebx,ebx
;设立标志位表示前导0已经结束
;判断方法是当第一次检测到不为0时bh设为1,这样之后
;即使bl为0,因为是根据ebx判断的,所以也不满足je条件
.print_integer:
cmp esi,-1
je .full_0
mov bl,[print_buffer+esi]   ;注意,这一步千万别把整数和ascii码混淆
dec esi
cmp ebx,0x30    ;这里是ascii(0) = 0x30,而不是0
je .handle_0
and bx,(0x00ff)
push ebx
call rie_putc
add esp,4
mov bh,1
loop .print_integer
popad
ret

.full_0:
push '0'
call rie_putc
add esp,4
popad
ret

.handle_0:
dec ecx
jmp .print_integer