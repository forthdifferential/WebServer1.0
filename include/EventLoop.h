#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#include <sys/epoll.h>
#include "Common.h"
#include "Timer.h"

class Chanel;
class HttpData;
class TimeWheel;

//EvenLoop类是事件循环（反应器 Reactor），每个线程只能有一个 EventLoop 实体，它负责 IO 和定时器事件的分派
class EventLoop
{
private:
    bool is_ManReactor_ = false;
    int conNum_;                    //注意：这里不能设置成静态，因为有很多个reactor,但是mainReactor使用的时候他是临界区资源，考虑上锁
    std::mutex numMtx_;            //处理conNum_的锁

    static const int PerEpollMaxEvent = 4096;   //Epoll单次最大处理事件数
    static const int Epoll_timeout = 10000;     //超时时间

    int epollfd_;
    epoll_event events_[PerEpollMaxEvent];
    
    //指针数组，通过这个数组 对注册到其内的众多fd的管理，毕竟有了Channel就有了fd及其对应的事件处理方法
    Chanel* chanelpool_[GlobalValue::MaxConnNumber];

    HttpData* httppool_[GlobalValue::MaxConnNumber];

    TimeWheel* wheelofloop_ = new TimeWheel();    //为什么每个事件池一个时间轮:为了避免竞争，时间轮作为一个线程一个的独有资源
    bool stop_;
public:
    explicit EventLoop(bool ismain);
    ~EventLoop();

    void StartLoop();
    void StopLoop();


    bool AddChanel(Chanel* chanel);
    bool DelChanel(Chanel* chanel);
    bool ModChanel(Chanel* chanel,__uint32_t ev);

    TimeWheel* Get_TimeWheel() const {return wheelofloop_;}
    int Get_conNum();
private:
    void Listen_and_Call();

};



#endif