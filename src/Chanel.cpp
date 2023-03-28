#include "Chanel.h"

Chanel::Chanel(int fd, bool isConn):fd_(fd),isConnect_(isConn)
{

}

Chanel::~Chanel()
{
    close(fd_);
}

void Chanel::CallRevents()
{
    if(revents_ & EPOLLERR)
        CallErfunc();
    //一般是把EPOLLHUP文件挂断做处理;(EPOLLHUP和EPOLLRDHUP区别)
       
    // 3-24 bug EPOLLHUP包含在EPOLLIN中，原写法如果触发EPOLLIN直接在此处断了
    //else if(revents_ & EPOLLHUP|EPOLLRDHUP)

    // 对应的连接被挂起，通常是对方关闭了连接
    // 客户端网页刷新的情况是先EPOLLRDHUP，然后重新EPOLLIN
    else if(revents_ & EPOLLRDHUP) 
        CallDiscfunc();

    else if(revents_ & EPOLLOUT)
        CallWrfunc();
    //EPOLLIN伴随可能有 EPOLLHUP，还没处理
    //TDOD
    else if(revents_ & EPOLLIN)
        CallRdfunc();
}

void Chanel::CallRdfunc()
{
    if(read_handle) read_handle();
    else Getlogger()->error("Chanel fd {} don't has read_handle",fd_);
}

void Chanel::CallWrfunc()
{
    if(write_handle) 
        write_handle();
    else
       Getlogger()->error("Chanel fd {} don't has write_handle",fd_);
}

void Chanel::CallErfunc()
{
    if(error_handle) 
        error_handle();
    else
        Getlogger()->error("Chanel fd {} don't has error_handle",fd_);
}

void Chanel::CallDiscfunc()
{
    if(disconn_handle) 
        disconn_handle();
    else
        Getlogger()->error("Chanel fd {} don't has disconn_handle",fd_);
}
