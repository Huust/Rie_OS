# bochs -f /home/huust/bochs_config/bochsrc
LIB_DIR = ./lib
BUILD_DIR = ./lib/kernel/build
KERNEL_DIR = ./lib/kernel
ENTRY_ADDR = 0xc0001500
ASM = nasm
CC = gcc
LD = ld
ASMFLAG = -f elf
CCFLAG = -m32 -I $(LIB_DIR) -I $(KERNEL_DIR) -c -fno-stack-protector
LDFLAG = -m elf_i386 -Ttext $(ENTRY_ADDR) -e main
# OBJS中.o文件的顺序就是最终寻找依赖关系的顺序,也是最后的编译顺序
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/interrupt.o\
$(BUILD_DIR)/debug.o $(BUILD_DIR)/print.o $(BUILD_DIR)/kernel.o\
$(BUILD_DIR)/string.o $(BUILD_DIR)/bitmap.o $(BUILD_DIR)/memory.o\
$(BUILD_DIR)/list.o $(BUILD_DIR)/timer.o $(BUILD_DIR)/thread.o \
$(BUILD_DIR)/switch_to.o $(BUILD_DIR)/loadcr3.o $(BUILD_DIR)/sync.o $(BUILD_DIR)/console.o \
$(BUILD_DIR)/all_init.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/process.o \
$(BUILD_DIR)/tss.o $(BUILD_DIR)/syscall.o $(BUILD_DIR)/stdio.o

#gcc编译
#基本上依赖文件都包含stdint.h，所以不需要额外以来该头文件
#注意$@和$<等伪指令是包含文件路径的，所以文件存放位置已经确定
$(BUILD_DIR)/main.o:$(KERNEL_DIR)/main.c $(KERNEL_DIR)/print.h $(KERNEL_DIR)/interrupt.h $(KERNEL_DIR)/all_init.h	#main.c包含的.h头文件
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/interrupt.o:$(KERNEL_DIR)/interrupt.c $(KERNEL_DIR)/global.h $(KERNEL_DIR)/print.h $(KERNEL_DIR)/io.h $(KERNEL_DIR)/interrupt.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/debug.o:$(KERNEL_DIR)/debug.c $(KERNEL_DIR)/print.h $(KERNEL_DIR)/interrupt.h $(KERNEL_DIR)/debug.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/string.o:$(LIB_DIR)/string.c $(KERNEL_DIR)/debug.h $(KERNEL_DIR)/global.h $(LIB_DIR)/string.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/bitmap.o:$(KERNEL_DIR)/bitmap.c $(LIB_DIR)/string.h $(KERNEL_DIR)/bitmap.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/memory.o:$(KERNEL_DIR)/memory.c $(KERNEL_DIR)/bitmap.h $(KERNEL_DIR)/print.h $(LIB_DIR)/string.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/list.o:$(KERNEL_DIR)/list.c $(KERNEL_DIR)/print.h $(KERNEL_DIR)/interrupt.h $(KERNEL_DIR)/debug.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/timer.o:$(KERNEL_DIR)/timer.c $(KERNEL_DIR)/memory.h $(KERNEL_DIR)/print.h $(KERNEL_DIR)/list.h $(LIB_DIR)/string.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/thread.o:$(KERNEL_DIR)/thread.c $(KERNEL_DIR)/memory.h $(KERNEL_DIR)/print.h $(KERNEL_DIR)/list.h $(LIB_DIR)/string.h $(KERNEL_DIR)/thread.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/all_init.o:$(KERNEL_DIR)/all_init.c $(KERNEL_DIR)/all_init.h $(KERNEL_DIR)/interrupt.h $(KERNEL_DIR)/memory.h $(KERNEL_DIR)/thread.h $(KERNEL_DIR)/timer.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/keyboard.o:$(KERNEL_DIR)/keyboard.c $(KERNEL_DIR)/print.h $(KERNEL_DIR)/interrupt.h $(KERNEL_DIR)/io.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/sync.o:$(KERNEL_DIR)/sync.c $(KERNEL_DIR)/list.h $(KERNEL_DIR)/interrupt.h $(KERNEL_DIR)/thread.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/console.o:$(KERNEL_DIR)/console.c $(KERNEL_DIR)/print.h $(KERNEL_DIR)/sync.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/tss.o:$(KERNEL_DIR)/tss.c $(KERNEL_DIR)/thread.h $(KERNEL_DIR)/memory.h $(KERNEL_DIR)/global.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/process.o:$(KERNEL_DIR)/process.c $(KERNEL_DIR)/thread.h $(KERNEL_DIR)/memory.h $(KERNEL_DIR)/global.h $(KERNEL_DIR)/tss.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/syscall.o:$(KERNEL_DIR)/syscall.c $(KERNEL_DIR)/thread.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/stdio.o:$(LIB_DIR)/stdio.c $(KERNEL_DIR)/syscall.h $(KERNEL_DIR)/debug.h $(LIB_DIR)/string.h
	$(CC) $(CCFLAG) -o $@ $<

#nasm编译
$(BUILD_DIR)/kernel.o:$(KERNEL_DIR)/kernel.asm
	$(ASM) $(ASMFLAG) -o $@ $<

$(BUILD_DIR)/print.o:$(KERNEL_DIR)/print.asm
	$(ASM) $(ASMFLAG) -o $@ $<

$(BUILD_DIR)/switch_to.o:$(KERNEL_DIR)/switch_to.asm
	$(ASM) $(ASMFLAG) -o $@ $<

$(BUILD_DIR)/loadcr3.o:$(KERNEL_DIR)/loadcr3.asm
	$(ASM) $(ASMFLAG) -o $@ $<

# 链接
$(KERNEL_DIR)/kernel.bin:$(OBJS)
	$(LD) $(LDFLAG) -o $@ $^

.PHONY :hd clean all

hd:
	dd if=$(KERNEL_DIR)/kernel.bin of=/home/huust/bochs_config/rieos_boot.img bs=512 count=200 seek=9 conv=notrunc

clean:
	cd $(BUILD_DIR) && rm -f ./*

build:$(KERNEL_DIR)/kernel.bin

#make all 之前最好clean以下build目录下的.o文件，防止因为更新时间导致少编译一些东西
all:clean build hd

