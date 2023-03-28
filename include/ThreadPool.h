#ifndef WEBSERVER_THREAD_POOL_H
#define WEBSERVER_THREAD_POOL_H

#include <queue>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <vector>
#include <cerrno>
#include <future>

class ThreadPool
{

public:
    explicit ThreadPool(size_t thread_number);
    ~ThreadPool();

    template<class F,class ...Args>
    auto Add_task(F&& f,Args&& ...args) ->std::future<typename std::result_of<F(Args...)>::type>;

    size_t Get_pollSize() const {return threads_.size();}
private:
    typedef std::function<void()> Task;    //函数对象类型
    std::condition_variable conditionv_;    //条件变量
    std::mutex mtx_;                        //互斥锁
    std::queue<Task> task_que_;            //任务队列
    std::vector<std::thread> threads_;      //线程池
    bool stop_ = false;
};

//模板函数定义在头文件，否则连接出错

template<class F,class ...Args>
//将可调用对象和参数包装在一个任务中，将任务添加到线程池的队列中，并返回一个std::future对象，以便调用方可以等待异步任务的完成，并访问其结果。
//后缀指定返回类型，typename来告知编译器它是一个类型名，std::result_of<F(Args...)>将推导出函数F的返回类型。
auto ThreadPool::Add_task(F&& f,Args&& ...args) ->std::future<typename std::result_of<F(Args...)>::type> // 或者写成 -> std::future<decltype(f(args...))>
{
    using ReturnType = typename std::result_of<F(Args...)>::type;//注意加typename ，嵌套从属类型
    /*
    在这个语句中，std::forward 用于完美转发参数，确保参数按照原始类型（左值或右值）传递给 std::bind 函数。
    这样做的目的是在 std::bind 调用中保持完美转发，避免不必要的值拷贝，从而提高效率和性能。
    在 C++ 中，std::forward 是用于实现完美转发的关键字之一。它根据传递的参数类型（左值或右值）选择是将其转
    发为左值引用还是右值引用。当使用 std::forward 转发参数时，编译器会根据实际参数类型自动确定该使用左值引用还是右值引用。这种技术可用于将函数的参数按照原始类型转发给其他函数或模板，从而避免不必要的值拷贝和提高代码效率。
    
    std::bind 的作用是将一个可调用对象和它的参数绑定到一个函数对象中，这个函数对象可以被异步线程执行
    
    */
    //把函数都封装程void()类型（所以要bind参数），作为参数构造packaged_task（成为一个异步可调用对象）
    auto task = std::make_shared<std::packaged_task<ReturnType()>> 
                (std::bind(std::forward<F>(f),std::forward<Args>(args)...));
    //获取函数异步结果 任务的future
    std::future<ReturnType> res = task->get_future();
    
    {
        std::unique_lock<std::mutex> lock(mtx_);

        if(stop_){
            throw std::runtime_error("enqueue on stopped ThreadPool!");
        }
        //构造std::function<void()>对象并返回，function中执行可异步线程执行的task
        task_que_.emplace(
            [task](){
                (*task)();
            }
        );
        //bug——task_que.emplace((*tsk));//不能直接解引用，解引用了只是一个packaged_task对象，任务队列的类型是std::function<void()>，两者类型不一致
        //应该传一个std::function<void()>对象，这个function里面执行tsk.
    }
    //随机唤醒一个等待在条件变量上的线程
    conditionv_.notify_one();

    return res;//返回一个任务的future

}


#endif