#ifndef WEBSERVER_SERVER_H
#define WEBSERVER_SERVER_H

#include "Common.h"


class Chanel;
class EventLoop;
class ThreadPool;

//Server是基于mainReactor和threadpool创建的
class Server
{
private:
    static std::shared_ptr<Server> service_;                //指向单例的智能指针
    static std::mutex init_lock_;                           //唯一的锁,用于单例实例化

    int port_;                                               //端口号
    int listenfd_;                                           //监听fd
    Chanel* listen_chanel_;                                  //监听Chanel
    std::vector<std::shared_ptr<EventLoop>> subReactors_;    //考虑用智能指针自动析构
    ThreadPool* Server_threadpool_;
    EventLoop* Server_main_reactor_;

    Server(int pot,EventLoop* mr,ThreadPool* tp);             //私有构造函数 懒汉模式
    /*私有析构类技术有点丑，考虑还是用智能指针*/
    // //用于析构的私有类，作用：因为单例是new出来的，程序结束会不自动释放，也不能主动调用析构函数释放，我在全局放一个私有类成员对象，因为对象是在栈区的，
    // //所以成员会自动析构，在他的析构函数中操作delet单例；这样一来就达成了自动释放单例的作用
    // class Release
    // {
    //     public:
    //     ~Release(){
    //         if(Server::service_ != nullptr){
    //             delete Server::service_;
    //         }
    //     }
    // };
    // //static Release memRelease;  // 注意：要全局实例化之后才会，程序结束才会调用Release析构，从而析构Server
public:
    std::vector<int> timeWheel_PipeOfWrite_;
public:
    ~Server();//还是考虑用智能指针
    // ~Server(){
    //     std::unique_lock<std::mutex> locker(init_lock_);
    //     if(service_ != nullptr){
    //         delete service_;    //可是这个delete也是调用本身析构函数？？？？
    //         service_ = nullptr;
    //     }
    // }
    void Server_Start();
    void Server_Stop();
    /**
     * 这里考虑使用线程安全的单例模式中懒汉模式，以做学习；
     * 但是Server是主线程创建的，不存在race condition（可以不锁？），也就是Get_the_service()只调用一次
    */
    static auto Get_the_service(int pot,EventLoop* mr,ThreadPool* tp){
        //这里上锁是因为存在 if(service_ == nullptr)和 service_ = new Server(pot,mr,tp);不同步的情况
        if(service_ == nullptr){  
            {
                //这里用std::lock_guard 也可以，但是Cpp17好像被建议删除
                std::unique_lock<std::mutex> locker(init_lock_);
                //
                //
                /**
                 * 当编译器优化的时候以下写法有问题，参考论文《C++ and the Perils of Double-Checked Locking》
                 * 问题描述：有问题，因为构造函数分为分配内存，构造器初始化，返回首地址指针（优化后顺序是132），有些内存模型，会出现分配内存后直接挂起，也就是没有构造完，另一个线程又构造了一次
                 * 怎么理解new没new完就返回了：实际很多编译器中都是使用了优化，可能会的执行顺序为 1、3、2（编译指令集的层次）。故 C++ 11 之后可以使用 volatile 指定不优化即可
                 * 解决方案： C++11中的atomic类，但是其实C++11规定了local static在多线程条件下的初始化行为，要求编译器保证了内部静态变量的线程安全性。
                */
                if(service_ == nullptr){

                    //std::shared_ptr<EventLoop> sub(new EventLoop(false)); 
                    //3-22 - bug,这里make_shared实际上创建shared_ptr对象的时候需要调用EventLoop构造函数，没有权限 ，修改为直接new
                    //service_ = std::make_shared<Server>(pot,mr,tp);

                    //3-23 - bug原版返回局部对象，错误  
                    //std::shared_ptr<Server> service_(new Server(pot,mr,tp));

                    std::shared_ptr<Server> ptr(new Server(pot,mr,tp));
                    service_ = ptr;
                }
            }
        }
        // 3-23 奇怪的bug 在函数体中new出来的service_是有内容的，解锁之后，返回的service_是空的
        return service_;
    }
    // Server* Get_the_service(int pot,EventLoop* mr,ThreadPool* tp){
    //     static std::once_flag oc;
    //     std::call_once(oc,[&]{service_ = unique_ptr<Server*> Server(pot,mr,tp);});
    //     return service_;
    // }
private:
    //监听socket负责这两个事件
    void ERRisComing();
    void CONNisComing();  
};


#endif 
