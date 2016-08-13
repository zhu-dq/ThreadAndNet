/*
 * 封装一些原子性操作
 * 核心：原子性操作：CAS FAA TAS
 * 作者:zhu-dq
 */
#ifndef ZDQ_ATOMIC_H
#define ZDQ_ATOMIC_H

#include <stdint.h>
#include "zdq_noncopyable.h"

namespace ZDQ{
    template <typename T >
    class AtomicIntegerT : ZDQ::NonCopyable{
    public:
        AtomicIntegerT():value_(0){}
        T get()
        {
            //判断value的值是否为零，如果为0则赋值0.返回的是未赋值之前的值，其实就是获取value的值
            return __sync_val_compare_and_swap(&value_,0,0);//原子性操作CAS
        }

        T getAndAdd(T x)//"i++"
        {
            //先获取然后执行加的操作，返回的是未修改的值
            return __sync_fetch_and_add(&value_,x);//原子性操作FAA
        }

        T addAndGet(T x)//"++i"
        {
            return getAndAdd(x)+x;
        }

        T incrementAndGet()// ++i
        {
            return addAndGet(1);
        }

        T decrementAndGet()// --i
        {
            return addAndGet(-1);
        }

        T getAndSet(T newValue)
        {
            //返回原来的值然后设置成新值
            return __sync_lock_test_and_set(&value_, newValue);//原子性操作TAS
        }

    private:
        volatile T value_;
    };
    typedef ZDQ::AtomicIntegerT<int32_t > AtomicInt32;
    typedef ZDQ::AtomicIntegerT<int64_t > AtomicInt64;
}

#endif


/*
 * ===============================NODE volatile=============================
 * volatile关键字确保指令不会因编译器的优化而省略(防止编译器对代码进行优化)
 * [每次都从内存读取数据而不是使用保存在寄存器中的备份]
 * 是为了避免其他线程已经将该值修改
 */