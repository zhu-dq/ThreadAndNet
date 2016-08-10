//
// Created by zhudanqi on 16-8-10.
//

#include "zdq_timer.h"
#include <iostream>
#include <functional>
using namespace std;
using namespace ZDQ;
 void func()
 {
     cout<<"哈皮很o_O//4秒"<<endl;
 }

void func1(Timer * t)
{
    cout<<"不开心  他们要停了我 5_5//6秒"<<endl;
    t->cannelTimer();
}

void test_case1()
{
    Timer t;
    t.setTimer(2,4);//延迟2秒，然后每隔4秒触发一次
    t.setTimerCallback(func);
    t.runTimer();//一直在这里面循环
    t.cannelTimer();
}

void test_case2()
{
    Timer t;
    t.setTimer(5,6);
    t.setTimerCallback(bind(func1,&t));
    t.runTimer();//调用func1后终止
}
int main()
{
    //test_case1();//ok
    test_case2();//ok
    return 0;
}
