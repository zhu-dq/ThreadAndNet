#include "zdq_thread_pool.h"
#include <iostream>
#include <string>
#include <unistd.h>
ZDQ::ThreadPool tp;
void func1()
{
    std::cout<<"func1"<<std::endl;    
    sleep(10);
    std::cout<<"func1 ending"<<std::endl;    
}

void func2(std::string str)
{
    std::cout<<"func2 has "<<str<<std::endl;
}

void func3(int num)
{
    int i = 0;
    while(i!=5)
    {
        num +=num;
        std::cout<<"current num "<<num<<std::endl;
        sleep(1);
        ++i;
    }
}

void func4()
{
     std::cout<<"主线程 * "<<std::endl;
    int i = 0;
    while(i < 20)
    {
        std::cout<<"** "<<std::endl;
        sleep(1);
        ++i;
    }
}
void test_thread_pool()
{
    tp.addTask(std::bind(func2,"dog"));
    tp.addTask(std::bind(func2,"miao miao"));
    tp.addTask(std::bind(func3,4));
    tp.addTask(func1);
    tp.addTask(std::bind(func2,"xiao wu"));
    tp.addTask(std::bind(func2,"xiao liu"));
    func4();
}

int main()
{
    tp.start(10,3);
    test_thread_pool();
    tp.stop();
    return 0;
}
