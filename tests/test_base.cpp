#include "zdq_thread.h"
#include "zdq_mutex.h"
#include "zdq_condition.h"
#include "zdq_blocking_queue.h"
#include <vector>
#include <unistd.h>
#include <string>
#include <time.h>
#include <assert.h>
ZDQ::MutexLock mutex;
ZDQ::MutexLock condition_mutex;
ZDQ::Condition con(condition_mutex);
ZDQ::BlockingQueue<std::string> b_queue_; 
std::vector<int> v;
bool isCondition = false;
//=========================================================
void threadFunc1()
{
    for(int i = 0;i<10;++i)
    {
        std::cout<<i<<std::endl;
        sleep(1);
    }
}

void threadFunc2(std::string expr)
{
    std::srand((unsigned int)time(NULL));
    for(int i = 0;i<5;++i)
    {
        std::cout<<expr<<std::endl;
        sleep(std::rand()%4);
    }
}
void threadFunc3()
{
    for(int i =0;i<10000*10000;++i)
    {
        v.push_back(i);
    }
}
void threadFunc4()
{
    
    //for(int i =0;i<10000*10000;++i)
    for(int i =0;i<1000*1000;++i)
    {
        ZDQ::MutexLockGuard  g(mutex);
        v.push_back(i);
    }
}

void conditionFunc1()
{
    ZDQ::MutexLockGuard g(condition_mutex);//必须
    while(!isCondition)
    {
        con.wait();
    }
    assert(isCondition);
    std::cout<<"i'm second"<<std::endl;
}

void conditionFunc2()
{
    {
        ZDQ::MutexLockGuard g(condition_mutex);//非必须
        isCondition = true;    //先改状态
    }
    con.notify();
    std::cout<<"i'am first \n"<<std::endl;
}

void BlockingQueueFunc1()
{
    std::cout<<b_queue_.take()<<std::endl;
    std::cout<<" ending~ "<<std::endl;
}

void BlockingQueueFunc2()
{
    std::cout<<"我数1 2 3 "<<std::endl;
    sleep(1);
    std::cout<<"1"<<std::endl;
    sleep(1);
    std::cout<<"2"<<std::endl;
    sleep(1);
    std::cout<<"3"<<std::endl;
    sleep(1);
    b_queue_.put("数对了～");
}

//======================================================
void test_thread()
{
    ZDQ::Thread t1(threadFunc1);
    t1.start();
    ZDQ::Thread t2(std::bind(threadFunc2,"threadfunc2"));
    t2.start();
    t1.join();
    t2.join();
}
void test_mutexlock()
{
    /*
     //不加锁，线程争用
    ZDQ::Thread t1(threadFunc3);
    t1.start();
    ZDQ::Thread t2(threadFunc3);
    t2.start();
    t1.join();
    t2.join();
    */
    //加锁
    ZDQ::Thread t1(threadFunc4);
    t1.start();
    ZDQ::Thread t2(threadFunc4);
    t2.start();
    t1.join();
    t2.join();
}
void test_condition()
{
    ZDQ::Thread t1(conditionFunc1);
    t1.start();
    ZDQ::Thread t2(conditionFunc2);
    t2.start();
    t1.join();
    t2.join();
}
void testBlockingQueue()
{
    ZDQ::Thread t1(BlockingQueueFunc1);
    t1.start();
    ZDQ::Thread t2(BlockingQueueFunc2);
    t2.start();
    t1.join();
    t2.join();
}
//==============================================
int main()
{
    std::cout<<"**test thread**"<<std::endl;
    test_thread();  //ok
    std::cout<<"**test mutex**"<<std::endl;
    test_mutexlock();//ok
    std::cout<<"**test condition**"<<std::endl;
    test_condition();//ok
    std::cout<<"**test BlockingQueue**"<<std::endl;
    testBlockingQueue();//ok
    return 0;
}
