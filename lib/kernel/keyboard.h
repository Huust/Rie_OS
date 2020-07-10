#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "./interrupt.h"
#include "./print.h"
#include "./io.h"

#define esc '\033'
#define del '\0177'
#define backspace '\b'
#define tab '\t'
#define enter '\n'  //fixme:注意一下enter不能使用\r\n;所以后面手动实现回车换行

#define placeholder 0
#define left_ctrl placeholder
#define left_shift placeholder
#define right_shift placeholder
#define left_alt placeholder
#define capslock placeholder



void keyboard_init(void);

#endif