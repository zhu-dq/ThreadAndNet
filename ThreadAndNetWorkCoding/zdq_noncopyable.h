/*
 *  不可复制类
 *  核心：私有化复制构造函数和重载运算符
 *  作用：继承该类的派生类都不具有值拷贝功能
 *  作者：zhu-dq
 */
#ifndef _ZDQ_NONCOPYABLE_H
#define _ZDQ_NONCOPYABLE_H

namespace ZDQ{
class NonCopyable{
    protected:

        NonCopyable(){}
        ~NonCopyable(){}

    private:

        NonCopyable(const  NonCopyable &);//复制构造
        NonCopyable&  operator=(const  NonCopyable &);//重载运算符
};
}

#endif
