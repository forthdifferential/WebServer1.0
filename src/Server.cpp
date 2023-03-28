
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "Server.h"
#include "Chanel.h"
#include "ThreadPool.h"

// Server* Server::service_ = nullptr;
std::shared_ptr<Server> Server::service_ = nullptr;
std::mutex Server::init_lock_{};

Server::Server(int pot, EventLoop *mr, ThreadPool *tp):
                    port_(pot),listenfd_(BindAndListen(port_)),
                    Server_main_reactor_(mr),Server_threadpool_(tp),
                    listen_chanel_(new Chanel(listenfd_,false)) //监听chanel，不是连接
{
    std::cout<<"listenfd_ is: "<<listenfd_<<std::endl;

    if(listenfd_ == -1) exit(-1);

    setnoblocking(listenfd_);
}

Server::~Server()
{
    //考虑
    /**
     * 资源管理的原则是需要留意带指针的对象！资源管理整理：
     * static std::shared_ptr<Server> service_              主函数分配，智能指针
     * Chanel* listen_chanel_                               Chanel由Reactor管理，有两种途径，分别是：Reactor中DELChanel函数删除，以及 Reactor析构时（调用DeleteChanel
     * ThreadPool* Server_threadpool_                       主函数分配 ，智能指针
     * std::vector<std::shared_ptr<EventLoop>> subReactors_ 智能指针
     * 
    */
}

void Server::Server_Start()
{
    /*对 监听socket 监听可读以及异常事件*/
    listen_chanel_->Set_events(EPOLLIN | EPOLLERR);
    listen_chanel_->Register_Rdhandle([this](){CONNisComing();});
    listen_chanel_->Register_Erhandle([this](){ERRisComing();});

    //epoll设置
    Server_main_reactor_->AddChanel(listen_chanel_);

    /*创建subReactor，并且分发任务,每个线程代表一个subReatcor*/
    size_t subReactor_size = Server_threadpool_->Get_pollSize();
    for(size_t i = 0;i != subReactor_size;i++){
        //创建
        auto sub = std::make_shared<EventLoop>(false);
        // std::shared_ptr<EventLoop> sub(new EventLoop(false));
        if(!sub){
            Getlogger()->warn("fail to creat a subreactor", strerror(errno));
            continue;
        }
        //向事件环池添加事件环
        subReactors_.emplace_back(sub);
        //向线程池添加任务（每个子线程任务是运行一整个事件环的运行函数）
        Server_threadpool_->Add_task([=]{sub->StartLoop();});
        
        //向tick按钮池添加tick按钮
        timeWheel_PipeOfWrite_.emplace_back(sub->Get_TimeWheel()->Get_1tick());//保存每个时间轮的tick管道写端，往里面写数据意味着tick一下
    }
}

void Server::Server_Stop()
{
    //暂时关闭，不用还原，删除等

    //subReactor关闭
    for(auto sub:subReactors_){
        sub->StopLoop();
    }
    //mainReactor关闭
    Server_main_reactor_->StopLoop();

}

void Server::ERRisComing()
{
    Getlogger()->error("Server accept a error", strerror(errno));
}

void Server::CONNisComing()
{
    struct sockaddr_in caddr;
    socklen_t caddr_len = sizeof(caddr);
    
    //ET模式，单次触发必须接受完整的请求，所以用while -- 错的
    //server不停接受socket连接请求
    while(true){
        int connfd = accept(listenfd_,reinterpret_cast<struct sockaddr*>(&caddr),&caddr_len);
        if(connfd == -1){
            if(errno != EAGAIN && errno != EWOULDBLOCK){
                close(connfd);
                //日志
                Getlogger()->error("failed to accept:{}",strerror(errno));
            }
            return;
        } 

        //限制最大并发连接数
        if(GlobalValue::Get_CurConnNumber() >= GlobalValue::MaxConnNumber){
            close(connfd);
            Getlogger()->error("too many connect");
            return;
        }

        //设置非阻塞
        if(setnoblocking(connfd) == -1){
            close(connfd);
            Getlogger()->error("failed to set nonblocking on connfd");
            return;
        }

        //因为是数据量不大，实时性要求比较高，关闭Nagle算法
        int enable = 1;
        if((setsockopt(connfd,IPPROTO_TCP,TCP_NODELAY,&enable,sizeof(enable))) == -1){
            Getlogger()->error("failed to failed to set TCP_NODELAY on connfd(Nagel algrithem)");
            close(connfd);
            return;
        }

        //建立连接后，分发任务给subReactor(找连接数量最小的subReactor)
        //重点：仔细考虑如何分发才能负载均衡？？？
        //需要改进，这里的选法很不好，而且选的过程中负载实时变化
        int least_conn_num=INT32_MAX,idx=0;
        for(int i=0;i<subReactors_.size();++i)
        {
            if(subReactors_[i]->Get_conNum() < least_conn_num)
            {
                least_conn_num=subReactors_[i]->Get_conNum();
                idx=i;
            }
        }

        //建立事件的Chanel 初始设置（1，chanel新建，监听时间设置，holder设置，添加到事件环）
        Chanel* connchanel = new Chanel(connfd,true);
        if(!connchanel){
            Getlogger()->error("failed to new a chanel");
            continue;
        }
        connchanel->Set_events(EPOLLIN|EPOLLERR|EPOLLRDHUP);

        HttpData* newholder = new HttpData(connchanel,subReactors_[idx].get());
        connchanel->Set_holder(newholder);

        // 添加到 子线程事件环
        if(!subReactors_[idx]->AddChanel(connchanel)){
            Getlogger()->error("AddChanel failed subReactors_{} fd connfd{}",idx,connfd);
        }
        
        // 更新总连接数
        GlobalValue::Inc_CurConnNumber();

        // 目前为止，mainReactor把一个新连接分给一个subReactor

        Getlogger()->info("SubReactor {} add a connect fd:{}, CurrentUserNumber: {}",idx,connfd,GlobalValue::Get_CurConnNumber());

    }
}
