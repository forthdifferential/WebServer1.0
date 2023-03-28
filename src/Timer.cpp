#include "Timer.h"
#include "Common.h"
#include "Chanel.h"

/********************定时器********************/
Timer::Timer(size_t pos, size_t turns):
posInWheel_(pos),turns_(turns){ }

void Timer::ExecuteFunc()
{
    if(funcofTimeUp_) funcofTimeUp_();
    else Getlogger()->debug("funcofTimeUp_ is not registered");
}

/********************时间轮*********************/
// 3-23 bug段错误，还是不太懂，在初始化列表中使用了 slot_，则可能会出现未定义行为，因为此时 slot_ 可能尚未完全初始化
// TimeWheel::TimeWheel(size_t maxsize):
//     sizeofTW_(maxsize),tick_size_(GlobalValue::TimerWheelResolution),
//     slot_(std::vector<std::list<Timer*>>(sizeofTW_,std::list<Timer*>()))
// {
TimeWheel::TimeWheel(size_t maxsize):
    sizeofTW_(maxsize),tick_size_(GlobalValue::TimerWheelResolution)
{
    slot_ = std::vector<std::list<Timer*>>(sizeofTW_,std::list<Timer*>());
    //创建管道，监听读事件，tick
    int res = pipe(tick_pfd_);
    if(res == -1){
        Getlogger()->error("TimeWheel create pipe error :{}", strerror(errno));
    }
    //设置非阻塞
    setnoblocking(tick_pfd_[0]);
    setnoblocking(tick_pfd_[1]);

    tick_chanel = new Chanel(tick_pfd_[0],false);
    tick_chanel->Set_events(EPOLLIN);
    //3-16 此处设置值传递？tick()应该会改变时间轮的参数
    //3-22 改为传this指针
    tick_chanel->Register_Rdhandle([this]{tick();});

}

TimeWheel::~TimeWheel()
{
    //清空定时器
    for(auto& sl:slot_){
        for(auto& timer:sl){
            delete timer;
        }
    }

    //关闭监听管道
    delete tick_chanel;
    tick_chanel = nullptr;

    //监听事件的读端关闭交给Chanel来close
    close(tick_pfd_[1]);
}

Timer *TimeWheel::TimeWheel_Insert_Timer(std::chrono::seconds timeout)
{
    if(timeout < std::chrono::seconds(0)) return nullptr;

    size_t pos, cycle;
    if(timeout < tick_size_){
        pos = 1;
        cycle = 0;
    }else{
        pos = (CurPos_+(timeout/tick_size_)%sizeofTW_)%sizeofTW_;
        cycle = (timeout/tick_size_)/sizeofTW_;
    }
    Timer *newTimer = new Timer(pos,cycle);
    slot_[pos].push_back(newTimer);
    
    return newTimer;
}

bool TimeWheel::TimeWheel_Remove_Timer(Timer *timer)
{
    if (!timer) return false;
    slot_[timer->Timer_GetPos()].remove(timer);    //remove参数元素，erase参数迭代器
    delete timer;
    timer = nullptr;
    return true;
}

bool TimeWheel::TimeWheel_Adjust_Timer(Timer *timer, std::chrono::seconds timeout)
{
    if(!timer || timeout < std::chrono::seconds(0)) return false;  
    
    size_t pos = 0, cycle = 0;
    if(timeout < tick_size_ ){      
        pos = 1;
        cycle = 0;
    }else{
        pos = (CurPos_+(timeout/tick_size_)%sizeofTW_)%sizeofTW_;
        cycle = (timeout/tick_size_)/sizeofTW_;
    }
    //移除 但不 delete
    slot_[timer->Timer_GetPos()].remove(timer);
    //修改
    timer->Timer_SetPos(pos);
    timer->Timer_SetTurns(cycle);
    //重新插入
    slot_[pos].push_back(timer);
    
    return true;
}

//有问题 
void TimeWheel::tick()
{
    char buf[128];
    int ret = Read_from_fd(tick_pfd_[0],buf,strlen("tick\0"));
    if(ret <= 0){
         Getlogger()->error("in tick ,read data to buffer error {} ，ret={}", strerror(errno),ret);
        return;
    }

    for(auto it = slot_[CurPos_].begin();it != slot_[CurPos_].end();){
       Timer* p = *it;
       if(!p ){
            slot_[CurPos_].erase(it);
            continue;
        } 
        //1.还没到turn 2.本轮turn
        if(p->Timer_GetTurns() > 0){
            p->Timer_TurnsDecline();
            ++it;
            continue;
        }else{
            
            auto next = ++it;
            p->ExecuteFunc();                  //关闭连接 删除时间 删除定时器
            //timer执行ExecuteFunc()后删除了，只需要链表下移一个位置就行
            //it =next;
            it = next;
        }

    }
    CurPos_ = (CurPos_ + 1)%sizeofTW_;
}