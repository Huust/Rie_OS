;loader update date:2020-04-02
;in this loader,we need to:create gdt||open A20 address||set PE = 1
;                       create page table||use virtual memory

;初始化保护模式之前先显示点东西通知下
CREATE_GDT:
            gdt1_empty:
            dd 0x00000000
            dd 0x00000000
            gdt2_code:
            dd 0x0000FFFF
            dd DESC_CODE_HIGH4
            gdt3_data:
            dd 0x0000FFFF
            dd DESC_DATA_HIGH4
            gdt4_video:
            dd 0x80000007
            dd DESC_VIDEO_HIGH4
            ;最后预留60个段描述符空位
            times 60 dq 0
            ;创建selector
            gdt_size equ $-CREATE_GDT
            gdt_address equ CREATE_GDT
            gdt_limit equ gdt_size-1

            SELECTOR_CODE equ 0000_0000_0000_1000B
            SELECTOR_DATA equ 0000_0000_0001_0000B
            SELECTOR_VIDEO equ 0000_0000_0001_1000B
            ;看到过更好的实现方法，是将一些宏放到.inc中
            ; SELECTOR_CODE equ (0x0001<<3) + TI_GDT + RPL0
            ; SELECTOR_DATA equ (0x0002<<3) + TI_GDT + RPL0
            ; SELECTOR_VIDEO equ (0x0003<<3) + TI_GDT + RPL0

            ;以下创建选择子的方法没有任何意义；因为选择子交给段寄存器，在loader中是不需要占用内存的
            ; create_selector:
            ; dw 0000_0000_0000_1000B
            ; dw 0000_0000_0001_0000B
            ; dw 0000_0000_0001_1000B
            



WRITE_TO_GDTR:
            dw  gdt_limit   ;limit界限即为size_offset
            dd  gdt_address



;打开a20地址线,初始化gdt，pe位置1
in al,0x92
or al,0000_0001B
out 0x92,al

lgdt [WRITE_TO_GDTR]

mov eax,cr0
or eax,0x00000001
mov cr0,eax

;涉及loader开启保护模式后显示部分的内容
;主要就是利用显卡打印一些字符通知已经进入保护模式了




;------------------为页表的开启做准备-----------------------
;流程：创建页目录表和页表；cr3寄存器存放页目录表基址；cr0寄存器pg位置位，开启页表
;最高1G的size分配给内核，低3G分配给用户
;最后一个目录项指向PDT本身的起始地址
set_up_page vstart = 0x100000:
;allocate mem
dd (0x101000 || 111B)
times 766 dd 0x00
dd (0x101000 || 111B)
times 256 dd 0x00
dd (0x00 || 111B)
times 1023 dd 0x00
;cr3 load address
mov eax,cr3
or eax,set_up_page
mov cr3,eax
;reload gdt
sgdt [gdt_ptr]
mov ebx, [gdt_ptr + 2]
or dword [ebx + 0x18 + 4], 0xc0000000
add dword [gdt_ptr + 2], 0xc0000000
add esp, 0xc0000000
;set pg in cr0,paging begin
mov eax,cr0
or eax,(1B<<31)
mov cr0,eax
lgdt [gdt_ptr]  ;here to update gdt address



;------------------加载内核-----------------------
;内核文件名为kernel.bin初步暂定存放在９扇区
;内核加载分为１从磁盘载入２按照elf格式展开
;我们的内核加载放在分页机制开启前；因此这里会以函数的形式存在
;1MB空间中0x7e00~0x9fbff空闲，因此我们把内核镜像加载到0x70000
KERNEL_BASE_ADD equ 0x70000
PT_NULL equ 0

distribute_kernel:
;reset register 
xor eax,eax
xor ebx,ebx
xor ecx,ecx
xor edx,edx
;read info in elf
mov ebx,[KERNEL_BASE_ADD + 28]
add ebx,KERNEL_BASE_ADD ;ebx被更改为第一个program header的地址
mov dx,[KERNEL_BASE_ADD + 42]   ;entry size
mov cx,[KERNEL_BASE_ADD + 44]   ;program header number

.handle_segment:
cmp byte [ebx+0],PT_NULL
je .PT_NULL_HANDLER    
;创建一个类似strcpy的函数
push dword [ebx + 16] 
mov eax,[ebx + 4]
add eax,KERNEL_BASE_ADD
push eax
push dword [ebx + 8]
call sim_strcpy
add esp 12
.PT_NULL_HANDLER:   ;语句块复用trick：可以同时用作正常执行或是je成功的跳转区
    add ebx,edx
loop .handle_segment
ret

sim_strcpy:
push ebp
mov ebp esp
push ecx
mov ds,cs
mov es,ds
mov ecx,[ebp+16]
mov esi,[ebp+8]
mov edi,[ebp+4]
cld
rep movsb
pop ecx
pop ebp
ret
