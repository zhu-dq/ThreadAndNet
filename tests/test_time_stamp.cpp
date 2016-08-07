#include "zdq_timestamp.h"
#include <iostream>
using namespace std;
using namespace ZDQ;
void test_timeValue()
{
    Timestamp t(1);
    cout<<"int64_t : "<<t.getMicroSeconds()<<endl;
    cout<<"double : "<<t.nowMicroSeconds()<<endl;
    Timestamp temp = t.now();
    cout<<"Timestamp : "<<temp.toString()<<"//toString() is ok"<<endl;
    if(t.valid())
        cout<<"t is valid"<<endl;
    Timestamp t1(1);
    Timestamp t2(2);
    if(t1<t2)
        cout<<"operator < is ok"<<endl;
    if(t2 == t2)
        cout<<"operator == is ok"<<endl;
    Timestamp t3 = t.now();
    Timestamp t4 = t.nowAfter(10);
    cout<<"==========="<<endl;
    cout<<"t4.getMicroSeconds()-t3.getMicroSeconds() = "<<t4.getMicroSeconds()-t3.getMicroSeconds()<<" 毫秒"<<endl;
    if((t4.getMicroSeconds()-t3.getMicroSeconds())/ZDQ::Timestamp::kMicroSecondsPerSecond == 10 )
        cout<<"nowAfter() is ok"<<endl;
}

int main()
{
    test_timeValue();
    return 0;
}
