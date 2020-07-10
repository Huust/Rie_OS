#include "./keyboard.h"

#define KBD_BUF_PORT 0x60
#define KBD_IRQ 0x01

//创建一个static类的全局二维数组,充当通码和ascii码的映射表

static char keymap[][2] = {
/* 扫描码   未与shift组合  与shift组合*/
/* 0x00 */	{0,	0},		
/* 0x01 */	{esc,	esc},
/* 0x02 */	{'1',	'!'},
/* 0x03 */	{'2',	'@'},		
/* 0x04 */	{'3',	'#'},		
/* 0x05 */	{'4',	'$'},		
/* 0x06 */	{'5',	'%'},		
/* 0x07 */	{'6',	'^'},		
/* 0x08 */	{'7',	'&'},		
/* 0x09 */	{'8',	'*'},		
/* 0x0A */	{'9',	'('},		
/* 0x0B */	{'0',	')'},		
/* 0x0C */	{'-',	'_'},		
/* 0x0D */	{'=',	'+'},		
/* 0x0E */	{backspace, backspace},	
/* 0x0F */	{tab,	tab},		
/* 0x10 */	{'q',	'Q'},		
/* 0x11 */	{'w',	'W'},		
/* 0x12 */	{'e',	'E'},		
/* 0x13 */	{'r',	'R'},		
/* 0x14 */	{'t',	'T'},		
/* 0x15 */	{'y',	'Y'},		
/* 0x16 */	{'u',	'U'},		
/* 0x17 */	{'i',	'I'},		
/* 0x18 */	{'o',	'O'},		
/* 0x19 */	{'p',	'P'},		
/* 0x1A */	{'[',	'{'},		
/* 0x1B */	{']',	'}'},		
/* 0x1C */	{enter,  enter},
/* 0x1D */	{left_ctrl, left_ctrl},
/* 0x1E */	{'a',	'A'},		
/* 0x1F */	{'s',	'S'},		
/* 0x20 */	{'d',	'D'},		
/* 0x21 */	{'f',	'F'},		
/* 0x22 */	{'g',	'G'},		
/* 0x23 */	{'h',	'H'},		
/* 0x24 */	{'j',	'J'},		
/* 0x25 */	{'k',	'K'},		
/* 0x26 */	{'l',	'L'},		
/* 0x27 */	{';',	':'},		
/* 0x28 */	{'\'',	'"'},		
/* 0x29 */	{'`',	'~'},		
/* 0x2A */	{left_shift, left_shift},	
/* 0x2B */	{'\\',	'|'},		
/* 0x2C */	{'z',	'Z'},		
/* 0x2D */	{'x',	'X'},		
/* 0x2E */	{'c',	'C'},		
/* 0x2F */	{'v',	'V'},		
/* 0x30 */	{'b',	'B'},		
/* 0x31 */	{'n',	'N'},		
/* 0x32 */	{'m',	'M'},		
/* 0x33 */	{',',	'<'},		
/* 0x34 */	{'.',	'>'},		
/* 0x35 */	{'/',	'?'},
/* 0x36	*/	{right_shift, right_shift},	
/* 0x37 */	{'*',	'*'},    	
/* 0x38 */	{left_alt, left_alt},
/* 0x39 */	{' ',	' '},		
/* 0x3A */	{capslock, capslock}
/*其它按键暂不处理*/
};



//几个特殊符的flag标志位
uint8_t shift_flag = 0;
uint8_t ctrl_flag = 0;
uint8_t alt_flag = 0;
uint8_t capslock_flag = 0;      //flag == 1代表灯亮(大写激活)

uint8_t extern_flag = 0;

/*
@notes:
    键盘驱动遵循第一套扫描码;支持0x39内的扫描码
    也就是不支持小键盘,Fn功能键,以及一些功能键(比如screenroll)
*/

static void kbd_intr_handler(uint8_t intr_num)
{
    uint8_t scan_code = inb(KBD_BUF_PORT);
    if (scan_code == 0xe0) {    //右alt | ctrl
        extern_flag = 1;
        return ;
    }


    //判断是否与alt | ctrl有关
    if (scan_code&0x7f == 0x1d || scan_code&0x7f == 0x38) {      
        
        if (extern_flag) {extern_flag = 0;}
        
        if ((scan_code & 0x80) == 0x80) {   //断码情况
            scan_code &= 0x7f;
            if (scan_code == 0x1D) {ctrl_flag = 0;}
            elseif (scan_code == 0x38) {alt_flag = 0;}

            return ;
        }

        if (scan_code == 0x1D) {ctrl_flag = 1;}
        elseif (scan_code == 0x38) {alt_flag = 1;}

        return ;
    }

    //判断是否和shift有关
    if (scan_code == 0x2a || scan_code == 0x36) {
        shift_flag = 1;
        return ;
    } elseif ((scan_code & 0x7f) == 0x2a ||(scan_code & 0x7f) == 0x36) {
        shift_flag = 0;
        return ;
    }

    //判断是否和capslock有关
    //capslock:摁下即为大写激活;但是想要取消大写只摁下去没有用(必须松开后)
    if (scan_code == 0x3a) {
        if (capslock_flag == 0) {capslock_flag = 1;}
        return ;
    }
    elseif ((scan_code & 0x80 == 0x80)&&((scan_code & 0x7f) == 0x3a)) {
        if (capslock_flag == 1) {capslock_flag = 0;}
        return ;
    }


    //根据标志位开始正式做判断
    //若scancode为字母
    if ((0x10 <= scan_code && scan_code <= 0x19)
    ||(0x1e <= scan_code && scan_code <= 0x26)
    ||(0x2c <= scan_code && scan_code <= 0x32)) {

        if (capslock_flag == shift_flag) {
            rie_putc(keymap[scan_code][0]);
            return ;
        } else {
            rie_putc(keymap[scan_code][1]);
            return ;
        }
        
    } else {
        rie_putc(keymap[scan_code][shift_flag]);
        return ;
    }

}


//测试键盘按键扫描码
static void kbd_test(uint8_t intr_num)
{
    rie_putc('h');
    inb(KBD_BUF_PORT);      //没有传递返回值,但因为是读端口所以存放在了ax寄存器中
}

void keyboard_init()
{
    intr_handler_register(KBD_IRQ, kbd_intr_handler);
    rie_puts("keyboard init done\r\n");
}
