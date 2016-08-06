#include "zdq_buffer.h"
#include <iostream>
#include <string>
using namespace std;

void test_size( const ZDQ::Buffer & b)
{
    cout<<b.readBuffSize()<<endl;
    cout<<b.writeBuffSize()<<endl;
}

void test_getAllAsString(ZDQ::Buffer & b)
{
    cout<<b.retrieveAllAsString()<<endl;
}

void test_getAsString(ZDQ::Buffer & b,size_t pos = 0)
{
    cout<<b.retrieveAsString(pos)<<endl;
}

void test_append(ZDQ::Buffer & b,std::string str)
{   
    b.append(str.c_str(),str.size());
}

void test_movetohead(ZDQ::Buffer & b)
{   
    std::string str("nihao");
    int i =0;
    while(i<500)
    {
        b.append(str.c_str(),str.size());
        ++i;
    }
    test_size(b);
    cout<<"========"<<endl;
    test_getAsString(b,1025);
    test_size(b);
}
int main()
{
    ZDQ::Buffer b;   
    test_size(b);
    test_append(b,"nihao");
    test_size(b);
    test_getAsString(b,2);
    test_size(b);
    test_getAllAsString(b);
    test_size(b);   
    test_movetohead(b);
    return 0;
}
