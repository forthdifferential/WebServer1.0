#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t thread_number)
{
    //读取命令行的时候保证thread_number>0
    for(size_t i = 0; i < thread_number; ++i)//增加线程数量,但不超过 预定义数量
    {
        //括号里面的lambda表达式作为参数构造thread(thread的构造函数的参数是他的执行函数)，并将下面的 lambda 函数作为该线程的执行函数的操作。
        threads_.emplace_back(
            [this](){ //工作线程函数
            while(true){
                //空函数对象
                //std::function<void()> task;
                Task task; // 获取一个待执行的 task
                {
                    //获取任务队列的锁
                    //任务队列上锁，在lock对象析构的时候，自动解锁，也就是{}之外解锁
                    // unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()
                    std::unique_lock<std::mutex> lock(this->mtx_);
                    //条件变量，获取锁后进入wait，直到被唤醒并检查条件；如果条件满足就取出任务做，不满足就等待。
                    // 3.10 注意这里是线程被停止或者任务队列非空的条件，因为设置停止才要快速继续跑，不停止不需要由stop_管理
                    // 3.16 改成了线程需要停止或者任务列表非空，因为需要停止但是还有任务，应该继续跑完
                    // 3.26 wait一直到((需要停止||有task) && 拿到锁lock)，如果满足了就重新持有这个锁，继续后面的代码； 
                    // 可以拿到锁且条件满足才能继续运行，所以没有虚假唤醒
                    this->conditionv_.wait(lock,[this](){return this->stop_ ||!this->task_que_.empty();});
                    //如果线程池设置停止，并且线程池空，结束
                    if(this->stop_ &&this->task_que_.empty())   return;
                    
                    //取出任务
                    task = std::move(this->task_que_.front());
                    this->task_que_.pop();
                }
                //执行任务
                task();
            }
        });
    }
}

/*停止线程池，等待所有线程结束后回收资源*/
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mtx_);
        stop_ = true;
    }

    //通知等待在条件变量上的所有线程，使它们从等待状态中退出。
    conditionv_.notify_all();

    //主线程阻塞，等待所有线程执行完成后，主线程再退出
    for(auto &thread:threads_){
        thread.join();
    }

}
