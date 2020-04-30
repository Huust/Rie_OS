#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "stdint.h"
#define RPL0 0
#define RPL1 1
#define RPL2 2
#define RPL3 3

#define TI_GDT 0
#define TI_LDT 1
//选择子
#define SELECTOR_CODE ((1 << 3) + (TI_GDT << 2) + RPL0)
#define SELECTOR_DATA ((2 << 3) + (TI_GDT << 2) + RPL0)
#define SELECTOR_STACK SELECTOR_DATA
#define SELECTOR_GS ((3 << 3) + (TI_GDT << 2) + RPL0)

#define IDT_DESC_P 1
#define IDT_DESC_DPL0 0
#define IDT_DESC_DPL3 3
#define IDT_DESC_32_TYPE 0xE 
#define IDT_DESC_16_TYPE 0x6
//8位属性值
#define IDT_DESC_PROPERTY_DPL0 \
((IDT_DESC_P << 7) + (IDT_DESC_DPL0 << 5) + IDT_DESC_32_TYPE)
#define IDT_DESC_PROPERTY_DPL3 \
((IDT_DESC_P << 7) + (IDT_DESC_DPL3 << 5) + IDT_DESC_32_TYPE)

#endif