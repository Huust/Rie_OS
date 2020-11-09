#include "./stdio.h"

/* 模仿linux对可变参数的处理 
ap:argument pointer,指向参数的指针
first_arg:第一个参数
type:参数类型

va_start:将ap指针指向第一个参数
va_arg:每调用一次，指针值加4（指向后一个参数），获取内存中的值
va_end:可变参数结束，指针回收
*/
#define va_start(ap, first_arg) (ap = (va_list)&first_arg)
#define va_arg(ap, type) (*(type*)(ap += 4))    //加4是因为ap是char*
#define va_end(ap) (ap = NULL)

/* itoa
@param:
    integer:待转换的数值
    buf:二重指针，转换后存放字符串
    base:基数，就是要转换的进制
@notes:为什么使用二级指针
    如果是普通的一级指针，那么每次递归调用压入的是字符串起始地址，导致if-else语句块
    中buf++并不能在多个函数作用域中维持，每次执行都是对同一块内存(buf+1)写入
*/
void itoa(int32_t integer, char** const buf, uint8_t base)
{
    if (integer < 0) {
        integer = 0 - integer;
        *(*buf)++ = '-';    
    }
    
    uint8_t m = integer % base;    //mod
    int32_t i = integer / base;    //int

    if (i != 0) {   /* i==0表明此时的integer已经小于进制base */
        itoa(i, buf, base);
    }

    if (m < 10) {*(*buf)++ = '0' + m;}  //*p++和*(p++)是一样的
    else {*(*buf)++ = m - 10 + 'A';}
}

uint32_t mySprintf(char* buf, const char* format, va_list ap)
{
    char* buf_ptr = buf;
    const char* format_ptr = format;
    uint32_t temp_int = 0;
    char temp_ch = 0;
    char* temp_str = NULL;

    while (*format_ptr) {
        if (*format_ptr != '%') {
            (*buf_ptr++) = (*format_ptr++);
            continue;
        } else {
            format_ptr++;   // 指向%后的格式字符
            switch (*format_ptr) {
                case 'x' :
                    temp_int = va_arg(ap, int);
                    itoa(temp_int, &buf_ptr, 16);
                    format_ptr++;   // 指向格式字符后一位的字符
                    break;
                case 'd' :
                    temp_int = va_arg(ap, int);
                    itoa(temp_int, &buf_ptr, 10);
                    format_ptr++;   // 指向格式字符后一位的字符
                    break;                
                case 'c' :
                    temp_ch = va_arg(ap, char);
                    *buf_ptr = temp_ch;
                    buf_ptr++;
                    format_ptr++;
                    break;
                case 's' :
                    temp_str = va_arg(ap, char*);
                    // if (*temp_str) {*buf_ptr++ = *temp_str++;}
                    rie_strcpy(buf_ptr, temp_str);
                    buf_ptr += rie_strlen(temp_str);
                    format_ptr++;
                    break;
                default :
                    PANIC("fail to parse!!!");
            }
        }
    }
    return rie_strlen(buf);
}

int printf(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    char buf[1024] = {0,};
    mySprintf(buf, format, ap);   //内部调用va_arg
    va_end(ap);
    return write(buf);
}