;loader update date:2020-04-02
;loader update date:2020-04-19
;in loader.asm,we need to:
;create gdt||open A20 address||set PE = 1
;create page table||use virtual memory
;load the kernel      

LOADER_IN_MEM equ 0x900
LOADER_START_SECTOR equ 0x02
KERNEL_ENTRY equ 0xc0001500

section loader vstart=LOADER_IN_MEM

DESC_G_4K equ 1_00000000000000000000000b
DESC_D_32 equ 1_0000000000000000000000b
DESC_L equ 0_000000000000000000000b
DESC_AVL equ 0_00000000000000000000b
DESC_LIMIT_CODE2 equ 1111_0000000000000000b
DESC_LIMIT_DATA2 equ DESC_LIMIT_CODE2
DESC_LIMIT_VIDEO2 equ 0000_000000000000000b
DESC_P equ 1_000000000000000b
DESC_DPL_0 equ 00_0000000000000b
DESC_DPL_1 equ 01_0000000000000b
DESC_DPL_2 equ 10_0000000000000b
DESC_DPL_3 equ 11_0000000000000b
DESC_S_CODE equ 1_000000000000b
DESC_S_DATA equ DESC_S_CODE
DESC_S_sys equ 0_000000000000b
DESC_TYPE_CODE equ 1000_00000000b
DESC_TYPE_DATA equ 0010_00000000b
;----------------
;desc_code_high4
;----------------
DESCRIPTOR_CODE_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + \
DESC_L + DESC_AVL + DESC_LIMIT_CODE2 + \
DESC_P+DESC_DPL_0 + DESC_S_CODE +\
DESC_TYPE_CODE + 0x00
;----------------
;desc_data_high4
;----------------
DESCRIPTOR_DATA_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 +\
DESC_L + DESC_AVL + DESC_LIMIT_DATA2 + \
DESC_P + DESC_DPL_0 + DESC_S_DATA + \
DESC_TYPE_DATA + 0x00
;----------------
;desc_video_high4
;----------------
DESCRIPTOR_VIDEO_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 +\
DESC_L + DESC_AVL + DESC_LIMIT_VIDEO2 + DESC_P + \
DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x00
;----------------
;selector list
;----------------
SELECTOR_CODE equ 0000_0000_0000_1000B
SELECTOR_DATA equ 0000_0000_0001_0000B
SELECTOR_VIDEO equ 0000_0000_0001_1000B

;-------------实模式过渡到保护模式-------------
;从mbr中跳到0x0b09(gdt_init)

GDT_BASE:
            ;gdt1_empty
            dd 0
            dd 0
            ;gdt2_code
            dd 0x0000FFFF
            dd DESCRIPTOR_CODE_HIGH4
            ;gdt3_data
            dd 0x0000FFFF
            dd DESCRIPTOR_DATA_HIGH4
            ;gdt4_video 
            dd 0x80000007
            dd (DESCRIPTOR_VIDEO_HIGH4|1011B)   ;原来地址写错了这里用按位或做补正
            ;最后预留60个段描述符空位
            times 60 dq 0
            ;创建selector
            GDT_LIMIT equ ($-GDT_BASE-1)

gdt_ptr dw GDT_LIMIT
        dd GDT_BASE

gdt_init:
;打开a20地址线,初始化gdt，pe位置1
in al,0x92
or al,0000_0010B
out 0x92,al

lgdt [gdt_ptr]
;设置cr0寄存器，开启保护模式
mov eax,cr0
or eax,0x00000001
mov cr0,eax
;刷新流水线；更新CS的值
jmp dword SELECTOR_CODE:p_mode_start

[bits 32]
p_mode_start:
;寄存器初始化
mov ax,SELECTOR_DATA
mov ds,ax
mov es,ax
mov ss,ax
mov ax,SELECTOR_VIDEO
mov gs,ax


mov eax, 0x9
mov ebx, 0x70000
mov ecx, 200
call readDisk32      ;页表开启前加载内核



;------------------为页表的开启做准备-----------------------
;流程：创建页目录表和页表；cr3寄存器存放页目录表基址；cr0寄存器pg位置位，开启页表
;最高1G的size分配给内核，低3G分配给用户
;最后一个目录项指向PDT本身的起始地址
PDE_BASE_ADD equ 0x100000   ;0x100000=0xfffff+1,也就是高于1MB地址空间的第一个位置
PTE_BASE_ADD equ 0x101000   ;第一个页表的位置

;--------初始化pde---------
mov ecx,4096
.pde_init:
mov esi,0
mov byte [PDE_BASE_ADD + esi],0
inc esi
loop .pde_init
mov eax,PTE_BASE_ADD
or eax,(111B)
mov ebx,PDE_BASE_ADD
mov [ebx],eax
;3GB开始的另一个PDE也要指向第一个页表
mov [ebx+4*768],eax
;additional_init：除了第一个和3GB开始的另一个，剩余pde的初始化
mov ecx,767
.pde_additional_init_1:
add eax,0x1000
add ebx,4
mov [ebx],eax
loop .pde_additional_init_1
add eax,0x1000
add ebx,4
mov ecx,254
.pde_additional_init_2:
add eax,0x1000
add ebx,4
mov [ebx],eax
loop .pde_additional_init_2


;--------初始化pte（指向低1MB）---------
mov ecx,256
mov ebx,PTE_BASE_ADD
mov eax,0
or eax,(111B)
.pte_init:
mov [ebx],eax
add ebx,4
add eax,0x1000
loop .pte_init


mov eax,PDE_BASE_ADD
mov cr3,eax


;reload gdt,也就是说把gdt中的段描述符加3GB使得开启页表后的虚拟地址是最高1GB的地址
sgdt [gdt_ptr]
mov ebx, [gdt_ptr + 2];获得GDT_BASE
or dword [ebx + 0x18 + 4], 0xc0000000
add dword [gdt_ptr + 2], 0xc0000000
mov esp,0x900 
add esp, 0xc0000000

;set pg in cr0,paging begin
mov eax,cr0
or eax,(1B<<31)
mov cr0,eax
lgdt [gdt_ptr]  ;here to update gdt address


jmp SELECTOR_CODE:enter_kernel

enter_kernel:
call distribute_kernel  ;内核展开
mov esp, 0xc009f000     ;设置kernel的堆栈的物理地址为0x9f000
jmp KERNEL_ENTRY


;------------------加载内核-----------------------
;内核文件名为kernel.bin初步暂定存放在９扇区
;内核还没壮大，所以暂时读取8扇区(8*512 == 1024*4)
;内核加载分为１从磁盘载入２按照elf格式展开
;我们的内核加载放在分页机制开启前；因此这里会以函数的形式存在
;1MB空间中0x7e00~0x9fbff空闲，因此我们把内核镜像加载到0x70000
KERNEL_BASE_ADD equ 0x70000
PT_NULL equ 0

;--------32位下读取磁盘内容---------
;para:eax,ebx,ecx
;eax:LBA格式下从哪个扇区开始读
;ebx:在ds为默认段寄存器情况下的段内地址偏移
;ecx:有效位是cx,控制最终读取循环次数(每次2Bytes)
readDisk32: 
    mov esi, eax
    mov di, cx
    mov dx, 0x1f2
    mov al, cl
    out dx, al
    mov eax, esi
    mov dx, 0x1f3
    out dx, al
    mov cl, 8
    shr eax, cl
    mov dx, 0x1f4
    out dx, al
    shr eax, cl
    mov dx, 0x1f5
    out dx, al
    shr eax, cl
    and al, 0x0f
    or al, 0xe0
    mov dx, 0x1f6
    out dx, al
    mov dx, 0x1f7
    mov al, 0x20
    out dx, al
.not_ready:
    nop
    in al, dx
    and al, 0x88
    cmp al, 0x08
    jnz .not_ready
    mov ax, di
    mov dx, 256
    mul dx
    mov cx, ax
    mov dx, 0x1f0
.go_on_read:
    in ax, dx
    mov [ebx], ax
    add ebx, 2
    loop .go_on_read
    ret


;------------内核展开-------------
distribute_kernel:
;reset register 
xor eax,eax
xor ebx,ebx
xor ecx,ecx
xor edx,edx
;read info in elf
mov ebx,[KERNEL_BASE_ADD + 28]  ;first program header addr
add ebx,KERNEL_BASE_ADD
mov dx,[KERNEL_BASE_ADD + 42]   ;entry size
mov cx,[KERNEL_BASE_ADD + 44]   ;program header number

.handle_segment:
cmp byte [ebx],PT_NULL
je .PT_NULL_HANDLER    
;创建一个类似strcpy的函数
push dword [ebx + 16]   ;参数3：size
mov eax,[ebx + 4]
add eax,KERNEL_BASE_ADD
push eax                ;参数2：源地址
push dword [ebx + 8]    ;参数1：目标地址
call sim_strcpy
add esp,12
.PT_NULL_HANDLER:
add ebx,edx
loop .handle_segment
ret

sim_strcpy:
cld
push ebp
mov ebp,esp
push ecx
mov ecx,[ebp+16]
mov esi,[ebp+12]
mov edi,[ebp+8]
rep movsb
pop ecx
pop ebp
ret