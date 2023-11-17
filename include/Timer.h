#ifndef WEBSERVER_TIMER_H
#define WEBSERVER_TIMER_H

#include <functional>
#include <chrono>
#include <vector>
#include <list>

class Chanel;
class HttpData;


  /**
   *  @brief 定时器类
   *   Timer实例的创建和销毁 全权转交给TimeWheel
   */
class Timer
{
private:
    typedef std::function<void()> CALLBACK;
    CALLBACK funcofTimeUp_;             // 超时回调函数
    size_t  posInWheel_;                // 位于时间轮的哪个槽
    size_t  turns_;                     // 剩余多少圈
public:
    Timer(size_t pos,size_t turns);

    friend class TimeWheel;
    
    void Timer_SetPos(size_t new_pos)           {posInWheel_ = new_pos;};
    void Timer_SetTurns(size_t new_turns)       {turns_ = new_turns;};
    void Timer_TurnsDecline()                   {turns_--;}
    void Register_Func(CALLBACK new_func)       {funcofTimeUp_ = new_func;};

    size_t Timer_GetPos() const {return posInWheel_;};
    size_t Timer_GetTurns() const {return turns_;};

    ~Timer(){};
private:
    void ExecuteFunc();//定时回调函数，

};

  /**
   *  @brief 时间轮
   * 
   */
class TimeWheel
{
private:
    std::vector<std::list<Timer*>> slot_;   // 时间轮的槽,每个槽一个链表
    size_t sizeofTW_;                       // 一共几个槽
    size_t CurPos_;                         // 初始在第0个槽
    std::chrono::seconds tick_size_;        // 时间粒度

    Chanel* tick_chanel;                    // 监听tick[0]事件的chanel
    int tick_pfd_[2]{};                     // 管道fd，timewheel拿到读端，监听写端发来的tick信号
private:
    void tick(); // 执行当前定时器，并推进时间轮到下一个槽
public:
    TimeWheel(size_t maxsize = 12);
    TimeWheel(const TimeWheel& wheel) = delete;
    TimeWheel& operator=(const TimeWheel wheel) = delete;
    ~TimeWheel();

    Timer* TimeWheel_Insert_Timer(std::chrono::seconds timeout);
    bool TimeWheel_Remove_Timer(Timer* timer);
    bool TimeWheel_Adjust_Timer(Timer* timer,std::chrono::seconds timeout);

    
    Chanel* Get_tickChanel() const  {return tick_chanel;};
    int     Get_1tick()      const  {return tick_pfd_[1];};   // 获取写端函数
};

#endif