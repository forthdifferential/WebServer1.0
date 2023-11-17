#include "EventLoop.h"
#include "Chanel.h"
#include "Timer.h"

EventLoop::EventLoop(bool ismain):
is_ManReactor_(ismain),conNum_(0),epollfd_(epoll_create1(EPOLL_CLOEXEC)),stop_(false)
{
    //epoll_create()产生的epoll事件表会动态增长，
    //epoll_create1(EPOLL_CLOEXEC)指定exec后，关闭这个epoll
    
    //先把时间轮tick的信号加入到epoll中
    if(epollfd_ == -1 || !AddChanel(wheelofloop_->Get_tickChanel())){
       Getlogger()->error("faile to create a reactor on constructor: {}",strerror(errno));
       exit(-1); 
    }
}

EventLoop::~EventLoop()
{
    stop_ = true;
    delete wheelofloop_; //TimeWheel每个reactor一个，也有reactor管理
    close(epollfd_);
}

void EventLoop::StartLoop()
{
    while (!stop_)
    {
        Listen_and_Call();
    }
    stop_ = true;
}

void EventLoop::StopLoop()
{   //删除所有事件，也就是还原
    stop_ = true;
    for(auto& it:chanelpool_){
        if(it) DelChanel(it);
    }
}

//监听事件fd加入到epoll中
bool EventLoop::AddChanel(Chanel *chanel)
{
    // //chanel没数据：空 连上了但是没有HTTP数据
    // if(!chanel || (chanel->Get_isConn() && chanel->Get_holder() == nullptr))   return false;
    
    // 3-23 重新理解addchanel：一种是httpdata相关的chanel，一种是普通事件的chanel
    if(chanel == nullptr)   return false;

    // 如果是httpdata事件的chanel，需要检查httpdata对象是否拿到   
    if(chanel->Get_isConn() && chanel->Get_holder() == nullptr) return false;
    
    // bug 3-24 ,创建subreactor的时候addchanel也需要放到事件池
    int cfd = chanel->Get_fd();
    chanelpool_[cfd] = chanel;
    // // 成功设置chanel和httpdata后吗，存入 chanel池和http_data池 这两个池有什么作用？当前reactor监听的所有事件和所有http_data请求
    // chanelpool_[cfd] = chanel;
    // httppool_[cfd] = chanel->Get_holder();

    // //存入时间轮
    // Timer* res = wheelofloop_->TimeWheel_Insert_Timer(GlobalValue::HttpHeadTime);

    // if(!res){
    //     return false;
    // }

    // //把Http_data和timer关联 设置timer的超时函数为http的超时响应报文
    // chanel->Get_holder()->Set_timer(res);
    // res->Register_Func([chanel](){chanel->Get_holder()->TimeUpCall();});
    
    // // 更新连接数量
    // // 为什么要上锁，但是前面添加部分不用上锁？ 因为是conNum_临界资源
    // {
    //     std::unique_lock<std::mutex> locker(numMtx_);
    //     conNum_++;
    // }

    // 3-23 严重bug 创建EventLoop的时候，只是加入tick监听口事件chanel，没有httpdata，更不需要httpdata关联

    // 以下代码只针对httpdata关联的chanel事件
    if(chanel->Get_holder()){
        
        httppool_[cfd] = chanel->Get_holder(); 
            //存入时间轮
        Timer* res = wheelofloop_->TimeWheel_Insert_Timer(GlobalValue::HttpHeadTime);
        if(!res){
            Getlogger()->error("insert timer error");
            return false;
        }

        //把Http_data和timer关联 设置timer的超时函数为http的超时响应报文
        chanel->Get_holder()->Set_timer(res);
        res->Register_Func([chanel](){chanel->Get_holder()->TimeUpCall();});
        
        {
            std::unique_lock<std::mutex> locker(numMtx_);
            conNum_++;
        }
    }
    // 3-27 bug 把注册添加events放到注册chanel池之后，因为一添加events子线程就能接受，
    // epoll拿到时如果chanel池还没有添加该chanel,会出错
    /*注册channel事件*/

    epoll_event ep_event{};
    memset(&ep_event,0,sizeof ep_event);
    ep_event.data.fd = cfd;
    ep_event.events = chanel->Getevens()|EPOLLET;//要不要EPOLLRDHUP？3-22不需要，这是reactor监听口，负责监听新的客户端进来

    if((epoll_ctl(epollfd_,EPOLL_CTL_ADD,cfd,&ep_event)) == -1){
       Getlogger()->error("add chanel to epoll fail:{}", strerror(errno));
        return false;
    }

    return true;
}

/*
  DelChnanel函数调用场景:
    1. 对于subReactor，客户端断开之后删除对应的chanel
    2. 对于Reactor,在stoploop重置的时候，需要删除其上的所有chanel，
    如果是subReactor删除全部的客户端chanel，如果是mainReactor删除监听的chanel
*/
bool EventLoop::DelChanel(Chanel *chanel)
{
    if(!chanel){
        Getlogger()->info("deleting chanel is null");
        return false;
    }
    int cfd = chanel->Get_fd();
    epoll_event ep_event;
    memset(&ep_event,0,sizeof(ep_event));
    ep_event.data.fd = cfd;
    ep_event.events = chanel->Getevens(); //bug 需要删除Eventloop对应chanel的events，否则epoll能读取到

    if((epoll_ctl(epollfd_,EPOLL_CTL_DEL,cfd,&ep_event)) == -1){
        //日志
        if(is_ManReactor_) Getlogger()->error("subReactor failed to delete fd {} in epoll:  {} ", epollfd_,strerror((errno)));
        else Getlogger()->error("mainReactor failed to delete fd {} in epoll:  {} ", epollfd_,strerror((errno)));

        return false;
    }

    //针对subReactor使用时，需要删除对应的httpdata
    // 这里删除HttpData和定时器 两个对象
    if(chanel->Get_holder()){
        //删除定时器,并从时间轮中删除
        Get_TimeWheel()->TimeWheel_Remove_Timer(chanel->Get_holder()->Get_timer());
        int fd = chanel->Get_fd();
        //删除Chanel并从chanelpool去掉
        delete chanelpool_[cfd];
        chanelpool_[cfd] = nullptr;
        //删除HttpData 并从httppool去掉
        delete httppool_[cfd];
        httppool_[cfd] = nullptr;   

        {
            std::unique_lock<std::mutex> locker(numMtx_);
            conNum_--;
        }

        GlobalValue::Dec_CurConnNumber();
        // 日志记录 输出关闭fd,剩余多少个连接数
        Getlogger()->info("Clientfd {} disconnect, current user number: {}", fd, GlobalValue::Get_CurConnNumber());
    }
    return true;
}

//调整fd对应的event事件
bool EventLoop::ModChanel(Chanel *chanel,__uint32_t ev)
{
    if(!chanel) return false;

    int cfd = chanel->Get_fd();
    epoll_event ep_event;
    memset(&ep_event,0,sizeof(ep_event));
    ep_event.data.fd = cfd;
    ep_event.events = ev|EPOLLET;    //要不要EPOLLRDHUP？不需要，之前设置好了
    
    if((epoll_ctl(epollfd_,EPOLL_CTL_MOD,cfd,&ep_event)) == -1){
        Getlogger()->error("modify chanel fail: {} fd {}", strerror(errno),cfd);
        return false;
    }
    chanel->Set_events(ev); //这里应该要加
    
    return true;
}

int EventLoop::Get_conNum()
{
    int num = 0;
    {
        std::unique_lock<std::mutex> lock(numMtx_);
        num = conNum_;
    }
    return num;
}

// 开始跑Reactor
void EventLoop::Listen_and_Call()
{
    while(!stop_){

        int number = epoll_wait(epollfd_,events_,PerEpollMaxEvent,Epoll_timeout);

        if(number == -1){
            if(errno == EINTR) {
                Getlogger()->debug("System call interrupts epoll: {}",strerror(errno));
                return;
            }
        
            continue;
        }
        // //设置超时时间和判断
        // else if(number == 0){
        //     Getlogger()->error("epoll timeout");
        //     continue;
        // }

        for(int i = 0; i < number; ++i){
            int sockfd = events_[i].data.fd;
            //Chanel*&，指向指针的引用，直接修改对象
            auto& cur_chanel = chanelpool_[sockfd];

            //如果是在chanelpool中的已注册事件；
            //subReactor的chanelpool里的chanel是mainReactor分发的时候注册的
            //mianReactor里的chanel只有监听新连接的listen_chanel_
            if(cur_chanel)
            {
                //拿到就设置已经处理，然后去处理它
                cur_chanel->Set_revents(events_[i].events);
                cur_chanel->CallRevents();
            }else{
                //日志输出；此事件的chanel不在chanelpoll中，说明之前有chanel没有add到事件池中
                Getlogger()->error("get an unregistered fd {}",sockfd);
            }
        }

        Getlogger()->flush();
    }

}
