/*
 * 创建buffer类，用于缓存I/O数据
 * 核心：vector<char>,主要利用其size()可以自动增长
 * 结构：[][][readIndex_][][][][][][][writeIndex_][][][][][][]
 * 为了合理利用内存，两个方面做了特殊考虑
 *      case1:readIndex_==writeIndex_ ,表示整个vector都空了，令readIndex_=writeIndex_=0;//即指向首部
 *      case2:readIndex_>move_line,即readIndex_前面有一片空白区域，浪费空间，即将已有数据整体前移
 * 作者:zhu-dq
 */

#ifndef ZDQ_BUFFER_H
#define ZDQ_BUFFER_H

#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
namespace ZDQ{
    class Buffer {
    public:
        static const size_t defaultSize = 1024;
        static const size_t move_line = 1024;
        explicit Buffer(size_t initSize = defaultSize):readIndex_(0),writeIndex_(0),v_buff_(initSize){}

        ~Buffer(){}

        size_t readBuffSize()const//可读buffer大小
        {
            return  writeIndex_ - readIndex_;
        }

        size_t writeBuffSize()const//可写buffer大小
        {
            return  v_buff_.size() - writeIndex_;
        }

        const char* peek()const//可读buff首位置
        {
            return &v_buff_[readIndex_];
        }

        char* peek()
        {
            return &v_buff_[readIndex_];
        }

        void movetohead()                //移动已有buff到首部
        {
            size_t  i = 0;
            while (readIndex_<writeIndex_)
                v_buff_[i++] = v_buff_[readIndex_++];
            readIndex_ = 0;
            writeIndex_ = i;
        }

        void retrieve(int len)//readIndex_右移len
        {
            assert(len<=readBuffSize());
            if(len < readBuffSize())
            {
                readIndex_+=len;
                if(readIndex_> move_line)  //case 2
                    movetohead();
            }
            else                    //case 1
                readIndex_=writeIndex_=0;
        }

        void append(const void* buf,int len) //添加数据
        {
            if(len>writeBuffSize())
                v_buff_.resize(len+writeIndex_);
            const char * index = static_cast<const char *>(buf);
            std::copy(index,index+len,&v_buff_[0]+writeIndex_);
            writeIndex_+=len;
        }

        std::string retrieveAsString(size_t len)//获取len大小的数据
        {
            assert(len<=readBuffSize());
            std::string result(peek(),len);
            retrieve(len);
            return result;
        }

        std::string retrieveAllAsString()//获取buff中的全部数据
        {
            return retrieveAsString(readBuffSize());
        }

        ssize_t readFd(int fd, int* savedErrno);
    private:
        std::vector<char> v_buff_;
        size_t readIndex_;
        size_t writeIndex_;
    };
}
#endif
