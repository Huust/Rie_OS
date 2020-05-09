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
$(BUILD_DIR)/debug.o $(BUILD_DIR)/print.o $(BUILD_DIR)/kernel.o 

#gcc编译
$(BUILD_DIR)/main.o:$(KERNEL_DIR)/main.c $(KERNEL_DIR)/print.h $(KERNEL_DIR)/interrupt.h $(KERNEL_DIR)/debug.h	#main.c包含的.h头文件
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/interrupt.o:$(KERNEL_DIR)/interrupt.c $(KERNEL_DIR)/global.h $(KERNEL_DIR)/print.h $(KERNEL_DIR)/io.h $(KERNEL_DIR)/interrupt.h
	$(CC) $(CCFLAG) -o $@ $<

$(BUILD_DIR)/debug.o:$(KERNEL_DIR)/debug.c $(KERNEL_DIR)/print.h $(KERNEL_DIR)/interrupt.h $(KERNEL_DIR)/debug.h
	$(CC) $(CCFLAG) -o $@ $<

#nasm编译
$(BUILD_DIR)/kernel.o:$(KERNEL_DIR)/kernel.asm
	$(ASM) $(ASMFLAG) -o $@ $<

$(BUILD_DIR)/print.o:$(KERNEL_DIR)/print.asm
	$(ASM) $(ASMFLAG) -o $@ $<

# 链接
$(KERNEL_DIR)/kernel.bin:$(OBJS)
	$(LD) $(LDFLAG) -o $@ $^

.PHONY :hd clean all

hd:
	dd if=$(KERNEL_DIR)/kernel.bin of=/home/huust/bochs_config/rieos_boot.img bs=512 count=200 seek=9 conv=notrunc

clean:
	cd $(BUILD_DIR) && rm -f $(KERNEL_DIR)/*

build:$(KERNEL_DIR)/kernel.bin

all:build hd

