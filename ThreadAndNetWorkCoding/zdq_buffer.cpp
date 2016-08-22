#include "zdq_buffer.h"
#include <sys/uio.h>
ssize_t ZDQ::Buffer::readFd(int fd, int *savedErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writeBuffSize();
    vec[0].iov_base = &v_buff_[writeIndex_];
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    // 当buffer空间足够大,不需要借助extrabuf.
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);
    if (n < 0)
        *savedErrno = errno;
    else if (n <= writable)
        writeIndex_ += n;
    else
    {
        writeIndex_ = v_buff_.size();
        append(extrabuf, n - writable);
    }
    return n;
}

/*
 *======================NODE1 scatter/gather I/O===============================
 * scatter/gather方式是与block dma方式相对应的一种dma方式。
 * 在dma传输数据的过程中，要求源物理地址和目标物理地址必须是连续的。但在有的计算机体系中，如IA，
 * 连续的存储器地址在物理上不一定是连续的，则dma传输要分成多次完成。
 * 如果传输完一块物理连续的数据后发起一次中断，同时主机进行下一块物理连续的传输，则这种方式即为block dma方式。
 * scatter/gather方式则不同，它是用一个链表描述物理不连续的存储器，然后把链表首地址告诉dma master。
 * dma master传输完一块物理连续的数据后，就不用再发中断了，而是根据链表传输下一块物理连续的数据，最后发起一次中断。
 * 很显然scatter/gather方式比block dma方式效率高。
 *==================== NODE2 readv/writev======================================
 * 1. readv和write函数用于在一次函数调用中读，写多个非连续缓冲区
 * 2.
 *      #include<sys/uio.h>
 *      ssize_t readv(int filedes, const struct iovec *iov, int iovcnt);
 *      ssize_t writev(int filedes, const struct iovec *iov, int iovcnt);
 *      返回值：若成功则返回已读，写的字节数，若出错则返回-1。
 *      filedes : 任何描述符
 *      iov : 指向某个iovc结构数组，
 *            struct iovec{
 *            void *iov_base; //starting address of buffer
 *            size_t iov_len; //size of buffer
 *            }
 *      iovcnt : iovc数组元素个数
 * 详见UNP1:P306
 */
