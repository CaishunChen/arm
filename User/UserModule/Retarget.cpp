/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
/*
*********************************************************************************************************
*	                                  
*	模块名称 : printf模块    
*	文件名称 : bsp_printf.cpp
*	版    本 : V2.0
*	说    明 : 实现printf和scanf函数重定向到串口1，即支持printf信息到USART1
*				实现重定向:
*				这个cpp模块无对应的h文件。
*
*	修改记录 :
*		版本号  日期         作者    说明
*		v1.0    2016-04-29   XSL  ST固件库V1.0.2版本。
*
*********************************************************************************************************
*/
#if 1

#ifdef __cplusplus
extern "C" {
#endif
#include "uctsk_Debug.h"
#ifdef __cplusplus
}
#endif
//-------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#pragma import(__use_no_semihosting_swi)

/* cpp文件中的中断函数的定义需要加上这个声明，中断时才会进入该中断函数 */
#define ARMAPI	extern "C"

namespace std{
    
    struct __FILE
    {
        int handle;

        /* Whatever you require here. If the only file you are using is */
        /* standard output using printf() for debugging, no file handling */
        /* is required. */
    };

    FILE __stdout;
    FILE __stdin;
    FILE __stderr;
    
    // You might also have to re-implement fopen() and related functions 
    // if you define your own version of __FILE
    // 有兴趣的可以定义自己的 __FILE，实现自己的 fopen，fwrite, fread
	// 因为库里面的 fopen 是弱符号，你的定义可以覆盖库的定义
FILE *fopen(const char * __restrict /*filename*/,
                           const char * __restrict /*mode*/)
    {
        // usart1<<"\n\r fopen. \n\r";
        return NULL;
    }

    //-----------------------------------------------------------
    int fputc(int ch, std::FILE *f) {
			  //-----用户代码-----
			  DebugOut((int8_t*)&ch,1);
			  //------------------
        return (ch);
    }
    
    //-----------------------------------------------------------
    int fgetc(FILE *f) {
        /* Your implementation of fgetc(). */
        //usart1<<"\n\r fgetc \n\r";
        return 0;
    }
    
    /*
	检查是流否有错误，如果没有错误，返回 0
    */
    int ferror(FILE *stream)
    {
        /* Your implementation of ferror(). */
        return 0;
    }

    long int ftell(FILE *stream){
        /* Your implementation of ftell(). */
			  //-----用户代码-----
			  //DebugOutStr((int8_t*)"ftell\n\r");
			  //------------------
        return 0;
    }
    
    int fclose(FILE *f){
        /* Your implementation of fclose(). */
			  //-----用户代码-----
			  //DebugOutStr((int8_t*)"\n\r fclose \n\r");
			  //------------------
        return 0;
    }
    
    int fseek(FILE *f, long nPos, int nMode){
        /* Your implementation of fseek(). */
			  //-----用户代码-----
			  //DebugOutStr((int8_t*)"fseek\n\r");
			  //------------------
        return 0;
    }
    
    /*
	对于输出流，把缓冲区中的内容全部发送出去
	这里用的是自己定义的 FILE 结构体，没有缓冲区，
	所以什么都不做就行了。
    */
    int fflush(FILE *f){
        /* Your implementation of fflush(). */
        return 0;
    }

    
    /*
默认的 _sys_exit 使用了 semihosted calls
	所以必须重新实现它。
	declared in <rt_sys.h>    */
    ARMAPI void  _sys_exit(int) {
        /* declared in <stdlib.h> */
        abort();
        while(1);
    }

    /*
	这个函数是C/C++标准库用来打印必要的调试信息的
	最好重新实现它，把调试信息发送到串口。
	this function is declared in <rt_sys.h>    */
    ARMAPI void _ttywrch(int ch) {
			  //-----用户代码-----
        DebugOut((int8_t*)&ch,1);
			  //------------------
        return ; 
    }
    
}
#endif
