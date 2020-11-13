#include "./stdio.h"
#include "./kernel/interrupt.h"
#include "./kernel/global.h"
#include "./string.h"
#include "./kernel/syscall.h"
#include "./kernel/print.h"

/* 模仿linux对可变参数的处理 
ap:argument pointer,指向参数的指针
first_arg:第一个参数
type:参数类型

va_start:将ap指针指向第一个参数
va_arg:每调用一次，指针值加4（指向后一个参数），获取内存中的值
va_end:可变参数结束，指针回收
*/
#define va_start(ap, v) ap = (va_list)&v  // 把ap指向第一个固定参数v
#define va_arg(ap, t) *((t*)(ap += 4))	  // ap指向下一个参数并返回其值
#define va_end(ap) ap = NULL		  // 清除ap


/* itoa
@param:
    integer:待转换的数值
    buf:二重指针，转换后存放字符串
    base:基数，就是要转换的进制
@notes:为什么使用二级指针
    如果是普通的一级指针，那么每次递归调用压入的是字符串起始地址，导致if-else语句块
    中buf++并不能在多个函数作用域中维持，每次执行都是对同一块内存(buf+1)写入
*/
static void itoa(uint32_t value, char** buf_ptr_addr, uint8_t base) {
   uint32_t m = value % base;	    // 求模,最先掉下来的是最低位   
   uint32_t i = value / base;	    // 取整
   if (i) {			    // 如果倍数不为0则递归调用。
      itoa(i, buf_ptr_addr, base);
   }
   if (m < 10) {      // 如果余数是0~9
      *((*buf_ptr_addr)++) = m + '0';	  // 将数字0~9转换为字符'0'~'9'
   } else {	      // 否则余数是A~F
      *((*buf_ptr_addr)++) = m - 10 + 'A'; // 将数字A~F转换为字符'A'~'F'
   }
}


/* 将参数ap按照格式format输出到字符串str,并返回替换后str长度 */
uint32_t vsprintf(char* str, const char* format, va_list ap) {
   char* buf_ptr = str;
   const char* index_ptr = format;
   char index_char = *index_ptr;
   int32_t arg_int;
   char* arg_str;
   while(index_char) {
      if (index_char != '%') {
	 *(buf_ptr++) = index_char;
	 index_char = *(++index_ptr);
	 continue;
      }
      index_char = *(++index_ptr);	 // 得到%后面的字符
      switch(index_char) {
	 case 's':
	    arg_str = va_arg(ap, char*);
	    rie_strcpy(buf_ptr, arg_str);
	    buf_ptr += rie_strlen(arg_str);
	    index_char = *(++index_ptr);
	    break;

	 case 'c':
	    *(buf_ptr++) = va_arg(ap, char);
	    index_char = *(++index_ptr);
	    break;

	 case 'd':
	    arg_int = va_arg(ap, int);
      /* 若是负数, 将其转为正数后,再正数前面输出个负号'-'. */
	    if (arg_int < 0) {
	       arg_int = 0 - arg_int;
	       *buf_ptr++ = '-';
	    }
	    itoa(arg_int, &buf_ptr, 10); 
	    index_char = *(++index_ptr);
	    break;

	 case 'x':
	    arg_int = va_arg(ap, int);
	    itoa(arg_int, &buf_ptr, 16); 
	    index_char = *(++index_ptr); // 跳过格式字符并更新index_char
	    break;
      }
   }
   return rie_strlen(str);
}


/* 格式化输出字符串format */
uint32_t printf(const char* format, ...) {
   va_list args;
   va_start(args, format);	       // 使args指向format
   char buf[1024] = {0};	       // 用于存储拼接后的字符串
   vsprintf(buf, format, args);
   va_end(args);
   return write(buf); 
}