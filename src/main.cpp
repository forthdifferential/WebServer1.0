#include <iostream>
#include <string>
#include <signal.h>

#include "Server.h"
#include "Common.h"
#include "ThreadPool.h"
#include "EventLoop.h"


// 写成全局的，唯一，方便调用
// static std::shared_ptr<Server> main_server;
static Server* main_server;

/*信号处理线程工作函数*/
static void sig_thread_call(void* set){
    
    sigset_t* sigset = reinterpret_cast<sigset_t*>(set);// 指针类型转换
    bool isStop = false;
    int ret, sig;
    while(!isStop){
        // 阻塞的等待之前加入阻塞信号集那三个信号
        ret = sigwait(sigset,&sig);
        if(ret != 0){
            Getlogger()->error("sigwait error: {}", strerror(ret));
            exit(1);
        }

        switch (sig)
        {
        case SIGTERM:{ // 进程向一个已经接收到 RST（复位）或者 FIN（结束）段的 socket 发送数据
            
            // 释放资源，退出程序
            //main_server->Server_Stop();
            //利用智能指针的对象自动释放 
            //isStop = true;
            exit(0);
        }
           break;   
        case SIGALRM:{ 
           // 时间走一个槽, 通知所有subReactor定时器检验
            auto tick1s = main_server->timeWheel_PipeOfWrite_;
            for(auto& it: tick1s){
                // 随便写点消息在tick的写端，触发EPOLLIN，通知所有Reactor
                const char* msg = "tick\0";
                int ret = Write_to_fd(it,msg,strlen(msg));               
                if(ret == -1){
                    Getlogger()->error("tick error in sig_thread");
                    exit(-1);
                }
                //一次性信号，重新设置
                alarm(GlobalValue::TimerWheelResolution);
            }

        }
           break;  
        case SIGPIPE:{ // 请求进程终止 kill
            main_server->Server_Stop();
            isStop = true;
            exit(1);
        }
           break;  
        default:
            break;
        }
    }
}

int main(int argc, char* argv[]){

    /*解析命令行参数*/
    // 3-23 bug 端口号0-1014是root用户用的
    auto res = parseCommandLine(argc, argv);
    if(res == std::nullopt){
        std::cout<<"The command line is wrong! "<<std::endl;
        std::cout<<"Please use the form :: ./server -p Port -s SubReactorSize -l LogPath(start with ./)"<<std::endl;
        return 0;
    }
    /*TODO 日志开启：创建日志单例，设置*/
    if(!Getlogger(std::get<2>(*res))){
        std::cout<<"Failed to create a Logger!\n"<<std::endl;
        return 0;
    }
    Getlogger()->set_pattern("[%Y-%m-%d %H:%M:%S] [thread %t] [%l] %v");
    /**设置信号掩码：
        如果现在主线程启动的时候设置了pthread_sigmask信号屏蔽，那
        之后创建的子线程会继承信号掩码，这样所有的线程都不会响应被
        屏蔽的信号，以确保线程的信号处理行为一致。然后单独拉出一个
        线程，解除阻塞信号，处理事件。
     */
        
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,SIGPIPE);
    sigaddset(&set,SIGALRM);
    sigaddset(&set,SIGTERM);

    //SIGPIPE、SIGALRM、SIGTERM信号将被阻塞，不会中断线程处理的过程。
    if(pthread_sigmask(SIG_BLOCK,&set,NULL) != 0 ){
        std::cout<<"Failed to mask the signal in the main function ! "<<std::endl;
        std::cout<<"errno: "<< errno<< std::endl; // c++中errno无需在声明头文件
        return 0;
    }

    /*分一个线程用作信号处理*/
    std::thread Sig_thread(sig_thread_call, &set);
    Sig_thread.detach();

    /*线程池初始化,注意信号线程和主线程不在线程池*/
    auto main_thread_pool = std::make_shared<ThreadPool>(static_cast<size_t>(std::get<1>(*res)));
    
    // 单例模式
    auto main_reactor = std::make_shared<EventLoop>(true);
    main_server = Server::Get_the_service(std::get<0>(*res), main_reactor.get(), main_thread_pool.get());

    /*服务器开始运行*/
    main_server->Server_Start();

    // 时间轮推动信号 按照时间轮的粒度推动
    alarm(GlobalValue::TimerWheelResolution);

    // 主reactor开始接收和分发
    main_reactor->StartLoop();
    return 0;
}