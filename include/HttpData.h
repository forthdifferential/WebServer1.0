#ifndef WEBSERVER_HTTPDATA_H
#define WEBSERVER_HTTPDATA_H

#include "Common.h"
#include "Chanel.h"
#include "Timer.h"

//class Chanel;
class EventLoop;

//文件类型内存映射
class SourceMap
{
private:
    static void Init();
    SourceMap() = default;  //为什么设置私有： 因为这个类不用实例化

public:
    SourceMap(SourceMap& other) = delete;
    SourceMap operator=(SourceMap& other) = delete;
    static std::string Get_file_type(std::string file_type);

private:
    static std::unordered_map<std::string,std::string> source_map_;
    static std::once_flag o_flag_;
};

class HttpData
{
public:
    //主状态机
    enum main_State_ParseHTTP{check_state_requestline, check_state_header, check_headerIsOk, check_body, check_state_analyse_content};

    //从状态机
    enum sub_state_ParseHTTP{
        requestline_data_is_not_complete, requestline_parse_error, requestline_is_ok,       //这一行表示的是解析请求行的状态
        header_data_is_not_complete, header_parse_error, header_is_ok,                      //这一行表示的是解析首部行的状态
        analyse_error,analyse_success                                                       //分析报文并填充发送报文状态
    };
    //HTTP状态码
    const char* ok_200_title = "OK";
    const char* error_400_respond = "Bad Request: Your request has bad syntax or is inherently impossible to satisfy.";
    //const char* error_400_form = "Your request has bad syntax or is inherently impossible to satisfy.\n";
    const char* error_403_respond = "Forbidden: You do not have permission to get file from this server.";
    //const char* error_403_form = "";
    const char* error_404_respond = "Not Found: The requested file was not found on this server.";
    //const char* error_404_form = "The requested file was not found on this server.\n";
    const char* error_408_respond = "Request Timeout.";
    const char* error_500_respond = "Internal Error: There was an unusual problem serving the requested file.";
    //const char* error_500_form = "There was an unusual problem serving the requested file.\n";
private:
    Chanel* http_chanel_;
    EventLoop* belong_sub_;                       //TODO epoll相关   EventLoop* 
    Timer* http_timer_;

    std::string write_buf_;
    std::string read_buf_;

    main_State_ParseHTTP main_state_;
    sub_state_ParseHTTP sub_state_;

    std::map<std::string,std::string> mp_;

public:
    HttpData(Chanel* chanel,EventLoop* evlp);
    ~HttpData();

    void TimeUpCall();

    void Set_timer(Timer* timer)    {http_timer_ = timer;}
    Timer* Get_timer()  const       {return http_timer_;}  
private:
    void state_machine();
    sub_state_ParseHTTP Parse_Requestline();    //解析请求行
    sub_state_ParseHTTP Parse_Header();         //解析请求头

    void Set_HttpErrorMsg(int fd,int erro_num,std::string msg);

    sub_state_ParseHTTP Analyse_Get_or_Head();    //get head
    sub_state_ParseHTTP Analyse_Post();         //post

    void Write_Response_GeneralData();

    void Reset_Http_Events(bool in);                //重新注册http事件
    void Reset();

private:
    void Callback_IN();
    void Callback_OUT();
    void Callback_ERROR();
    void Callback_Dis();

};

#endif