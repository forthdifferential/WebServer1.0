#ifndef WEBSERVER_CHANEL_H
#define WEBSERVER_CHANEL_H

#include "Common.h"
#include "HttpData.h"
#include "EventLoop.h"
#include <stdint.h>

//管道端的数据类
//对fd事件相关方法的封装,有了Channel就有了fd及其对应的事件处理方法
class Chanel
{
private:
    int fd_;
    bool isConnect_;
    __uint32_t events_;     // 感兴趣的事件集
    __uint32_t revents_;   // 就绪的事件集(现在就要去处理了) 

    HttpData* holder_ = nullptr;
    using CALLBACK = std::function<void()>;
    CALLBACK read_handle;
    CALLBACK write_handle;
    CALLBACK error_handle;
    CALLBACK disconn_handle;
    
public:
    explicit Chanel(int fd,bool isConn);
    ~Chanel();   

    __uint32_t  Getevens()   const  {return events_;};
    int         Get_fd()     const  {return fd_;};
    HttpData*   Get_holder() const  {return holder_;};
    bool        Get_isConn() const  {return isConnect_;};

    void Set_events(__uint32_t event)           {events_ = event;};
    void Set_revents(__uint32_t event)          {revents_ = event;};
    void Set_holder(HttpData* holder)           {holder_= holder;};
    void Register_Rdhandle(CALLBACK func)       {read_handle = std::move(func);};
    void Register_Wrhandle(CALLBACK func)       {write_handle = std::move(func);};
    void Register_Erhandle(CALLBACK func)       {error_handle = std::move(func);};
    void Register_Dishandle(CALLBACK func)      {disconn_handle = std::move(func);};

    void CallRevents();
private:
    void CallRdfunc();
    void CallWrfunc();
    void CallErfunc();
    void CallDiscfunc();
    
};



#endif