//
// Created by zhudanqi on 16-8-13.
//

#include "zdq_atomic.h"
#include <assert.h>
//原子性操作的测试函数
int main()
{
    {//64位的原子性操作
        ZDQ::AtomicInt64 a0;
        //断言测试一个条件并可能使程序终止
        assert(a0.get() == 0);//首先肯定会被置为0
        assert(a0.getAndAdd(1) == 0);//加1，先获取然后加，还是0
        assert(a0.get() == 1);//此时变为1
        assert(a0.addAndGet(2) == 3);//先加然后获取，所以是3
        assert(a0.get() == 3);
        assert(a0.incrementAndGet() == 4);//先加然后获取
        assert(a0.get() == 4);
        a0.incrementAndGet();
        assert(a0.get() == 5);
        assert(a0.addAndGet(-3) == 2);
        assert(a0.getAndSet(100) == 2);
        assert(a0.get() == 100);
    }

    {//32位的原子性操作
        ZDQ::AtomicInt32 a1;
        assert(a1.get() == 0);
        assert(a1.getAndAdd(1) == 0);
        assert(a1.get() == 1);
        assert(a1.addAndGet(2) == 3);
        assert(a1.get() == 3);
        assert(a1.incrementAndGet() == 4);
        assert(a1.get() == 4);
        a1.incrementAndGet();
        assert(a1.get() == 5);
        assert(a1.addAndGet(-3) == 2);
        assert(a1.getAndSet(100) == 2);
        assert(a1.get() == 100);
    }
}