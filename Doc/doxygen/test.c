/** 
 * @file    test.c 
 * @brief   Brief description  
 * @author  徐松亮 许红宁
 * @version 1.0 
 * @date    2011.3.1 
 * @bug     It has not been implemented yet 
 * @warning No 
 */  
#include <iostream>  
using namespace std;  
/** 
 * A macro  
 */  
#define ABS(x) (((x)>0)?(x):-(x))  
/** 
 * A structure 
 * details 
 */  
struct Type {  
    int x;  /**< member */  
};  
typedef struct Object Object    /**< Ojbect type */  
/** 
 * Vehicle class 
 * @extends Object 
 */  
struct Vehicle {  
    Object base;        /**< Base */  
};  
/** An enum type. More details */
/** @brief Another enum, with inline docs */   
enum EType {  
    Val1,   /**< enum value 1 */  
    Val2    /**< enum value 2 */  
};  

///@brief 函数名称：main  
///@todo 代码实现的功能: 设置工区名称  
///@return 说明：bool  
///@retval 1. true 名字设置成功  (返回值说明(可选))  
///@retval 2. false 名字设置失败  
///@bug 此处的bug描述: 无  
bool main(void function (params)
{
	^
})  
{  
	while(1);
} 

打印字符串，不影响指令
 * <pre>
 * </pre>
打印字符串，忽视指令
@verbatim
@endverbatim

@可用\代替，但我倾向于用@。 
@param[in|out]  参数名及其解释 
@exception 用来说明异常类及抛出条件 
@return  对函数返回值做解释 
@note  表示注解，暴露给源码阅读者的文档 
@remark  表示评论，暴露给客户程序员的文档 
@since  表示从那个版本起开始有了这个函数 
@deprecated 引起不推荐使用的警告 
@see  表示交叉参考 