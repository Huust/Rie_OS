;mbr主引导程序
;update date:2020-03-30
;second update:2020-04-16
LOADER_IN_MEM equ 0x900    ;loader.asm will be loaded from 0x80000
LOADER_START_SECTOR equ 0x02  ;loader.asm in disk;mbr is set in sector 0


section mbr vstart=0x7c00:
mov ax,cs   ;the moment cs==0
mov ss,ax
mov es,ax
mov ds,ax
mov fs,ax
;
mov ax,0xb800
mov gs,ax
;- why sp is given the value 0x7c00
;- because stack goes downward and code goes upward
mov sp,0x7c00 

;bios int 0x10 to clear the screen
mov ah,0x06 ;ah是功能编码，0x06向上滚屏；之前忽略了这个参数
mov al,0   
mov ch,0   
mov cl,0   
mov dh,25  
mov dl,85  
mov bh,0x07
int 0x10

mov byte [gs:0],'R'
mov byte [gs:1],0x07
mov byte [gs:2],'i'
mov byte [gs:3],0x07
mov byte [gs:4],'e'
mov byte [gs:5],0x07
mov byte [gs:6],'O'
mov byte [gs:7],0x07
mov byte [gs:8],'S'
mov byte [gs:9],0x07

;-----------------显示部分结束，开始处理loader--------------

;成功调用call后loader被写入内存；之后jmp到loader所在位置开始执行
call loadin_loader
mov ax,0xb06
jmp ax  ;LOADER_IN_MEM地址+gdt_size

loadin_loader:
;0x1f2:number of disk sections to write   0x1f3-0x1f5:LBA0-23   
;0x1f6:LBA24-27   0x1f7:command&status register
mov dx,0x1f2
mov al,4    ;1-->4,考虑到loader的大小超过了512kb
out dx,al

mov dx,0x1f3
mov al,2
out dx,al

mov dx,0x1f4
mov al,0
out dx,al

mov dx,0x1f5
mov al,0
out dx,al

mov dx,0x1f6
mov al,0xe0
out dx,al

mov dx,0x1f7
mov al,0x20
out dx,al

;采用轮询方法等待状态寄存器(0x17f端口)3,7位满足要求
.QuestForRead:
in al,dx
and al,0x88 
cmp al,0x08
jnz .QuestForRead

mov bx,LOADER_IN_MEM
mov dx,0x1f0
mov cx,1024 ;256-->1024,考虑到loader的大小超过了512kb

;将读取的数据写入内存
.WriteToMem:
in ax,dx    ;注意，在读取16位数据时，应该是每次in指令，自动向后获取新的16bit数据
mov [bx],ax
add bx,2
loop .WriteToMem
ret

times 510-($-$$) db 0   ;$:addr of this line $$:addr of the start of the section
                        ;510=512-(0x55+0xaa:which means 2bytes)
magic_number:
db 0x55,0xaa


