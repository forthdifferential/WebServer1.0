#ifndef WEBSERVER_COMMON_H
#define WEBSERVER_COMMON_H

#include <tuple>
#include <optional>
#include <unistd.h>
#include <regex>
#include <string>
#include <iostream>
#include <signal.h>
#include <thread>
#include <assert.h>
#include <mutex>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

std::shared_ptr<spdlog::logger> Getlogger(std::string path="./log/log.txt");

class GlobalValue
{
    //静态变量初始化0
public:
    static int TimerWheelResolution;    //时间轮粒度 一个槽的时间
    static const int MaxConnNumber = 100000;           //最大连接树
    static int BufferMaxSIze;           // 读数据，最大缓冲区大小

    static std::chrono::seconds HttpHeadTime;       // TCP连接建立后，必须在该时间内接受完整的请求行和首部，否则超时
    static std::chrono::seconds HttpPostBodyTime;   // post报文，相邻报文到达时间的最大间隔
    static std::chrono::seconds KeepAliveTime;      // 长连接的超时时间
    static char Favicon[555];
private:
    static int CurConnNumber;
    static std::mutex CurConnNumber_mtx;

public:
    static void Inc_CurConnNumber(){
        std::unique_lock<std::mutex> locker(CurConnNumber_mtx);
        CurConnNumber++;
    }
    static void Dec_CurConnNumber(){
        std::unique_lock<std::mutex> locker(CurConnNumber_mtx);
        CurConnNumber--;
    }    
    static int Get_CurConnNumber(){
        std::unique_lock<std::mutex> locker(CurConnNumber_mtx);
        return CurConnNumber;
    }  

};

//网络数据的socket读取，recv和send区别于writr和read
int RecvData(int fd, std::string &read_buf, bool& is_over);

//网络数据的socket写入
int SendData(int fd, std::string &buf,bool& is_over);


/*
@breif ET模式下向文件描述符（非连接socket）写n个字节的数据
@param[in]  fd          文件描述符
@param[in]  content     待写入数据的首地址
@param[in]  length      待写入数据的字节数
@return                 成功写入的字节数，-1表示写数据出错
*/
int Write_to_fd(int fd,const char* content,int length);

/*
@breif ET模式下从文件描述符（非连接socket）读n个字节的数据
@param[in]  fd          文件描述符
@param[in]  content     待读取数据的首地址
@param[in]  length      期望取数据的字节数
@return                 成功读取的字节数，-1表示读取数据出错
*/
int  Read_from_fd(int fd,const char* buf,int length);

//解析命令行参数
std::optional<std::tuple<int,int,std::string>>  parseCommandLine(int argc, char* argv[]);

//设置fd非阻塞
int setnoblocking(int fd);

//socket 创建(设置端口复用)绑定监听
int BindAndListen(int port);

std::string Gettime();  //获取当前时间并按照一定的格式输出

#endif