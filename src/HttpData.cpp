#include "HttpData.h"
#include "sys/stat.h"
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
/*****************文件类型映射类************************/
std::unordered_map<std::string,std::string> SourceMap::source_map_{};
std::once_flag SourceMap::o_flag_{};

void SourceMap::Init()
{
    /*文件类型*/
    source_map_[".html"] = "text/html";
    source_map_[".avi"] = "video/x-msvideo";
    source_map_[".bmp"] = "image/bmp";
    source_map_[".c"] = "text/plain";
    source_map_[".doc"] = "application/msword";
    source_map_[".gif"] = "image/gif";
    source_map_[".gz"] = "application/x-gzip";
    source_map_[".htm"] = "text/html";
    source_map_[".ico"] = "image/x-icon";
    source_map_[".jpg"] = "image/jpeg";
    source_map_[".png"] = "image/png";
    source_map_[".txt"] = "text/plain";
    source_map_[".mp3"] = "audio/mp3";
    source_map_["default"] = "text/html";
}

std::string SourceMap::Get_file_type(std::string file_type)
{
    std::call_once(o_flag_,Init);

    if(source_map_.count(file_type))
        return source_map_[file_type];
    else
        return source_map_["default"];
}

/**************************************************/


HttpData::HttpData(Chanel *chanel, EventLoop *evlp):http_chanel_(chanel),belong_sub_(evlp)
{
   main_state_ = main_State_ParseHTTP::check_state_requestline;

   //设置chanel回调函数
   if(chanel){
        http_chanel_->Register_Rdhandle([=](){Callback_IN();});
        http_chanel_->Register_Wrhandle([=](){Callback_OUT();});
        http_chanel_->Register_Erhandle([=](){Callback_ERROR();});
        http_chanel_->Register_Dishandle([=](){Callback_Dis();});
   }  
}

HttpData::~HttpData()
{
    //没有控制的堆区对象 没有new
}

void HttpData::TimeUpCall()
{
    //连接超时了
    Getlogger()->info("fd{} http timeout",http_chanel_->Get_fd());
    //错误码回写，以及向write_buf传输

    std::cout<<http_chanel_->Get_isConn()<<std::endl;
    std::cout<<write_buf_<<std::endl;
    std::cout<<read_buf_<<std::endl;
    std::cout<<main_state_<<std::endl;
    std::cout<<http_chanel_->Get_holder()<<std::endl;
    std::cout<<http_chanel_ <<std::endl;
    std::cout<<http_chanel_->Get_fd()<<"http timeout"<<std::endl;

    Set_HttpErrorMsg(http_chanel_->Get_fd(),408,"Request Timeout");
    //错误，状态码需要响应回写！！
    //Callback_Dis();
    Callback_OUT(); // 但是回写之后直接关闭这个chanel以及定时器
}

//TODO 没有解析多个HTTP报文，之后考虑管线化怎么修改
//状态机操作函数
void HttpData::state_machine()
{
    bool finish = false, error = false;
    while(!finish && !error){
        switch (main_state_)
        {
            /*解析请求行*/
            case check_state_requestline:
            {
                sub_state_ = Parse_Requestline();
                switch (sub_state_)
                {
                case requestline_data_is_not_complete:
                    //继续读
                    return;
                case requestline_parse_error:
                    Set_HttpErrorMsg(http_chanel_->Get_fd(),400,error_400_respond);
                    error = true;
                    break;
                case requestline_is_ok:
                    main_state_ = check_state_header;
                    break;
                default:
                    //TOD日志
                    break;
                }
            } break;

            /*解析请求首部*/
            case check_state_header:
            {
                sub_state_ = Parse_Header();
                switch (sub_state_)
                {
                case header_data_is_not_complete:
                    //继续读
                    return;
                case header_parse_error:
                    Set_HttpErrorMsg(http_chanel_->Get_fd(),400,error_400_respond);
                    error = true;
                    break;
                case header_is_ok:
                    main_state_ = check_headerIsOk;
                    break;
                }
            }  break;

            /*判断post超时*/
            case check_headerIsOk:
            {
                //解析post get和head命令,其中post请求需要判断数据是否完整
                if(mp_["method"] == "POST")
                    main_state_ = check_body;
                else
                    main_state_ = check_state_analyse_content;
            }  break;

            /*只解析POST请求，判断POST请求体的完整性*/
            case check_body:
            {   //保证前后报文间隔时间不超过 HttpPostBodyTime
                belong_sub_->Get_TimeWheel()->TimeWheel_Adjust_Timer(http_timer_,GlobalValue::HttpPostBodyTime);

                //检查HTTP消息体的完整性
                if(mp_.count("Content-Length")){
                    std::string length = mp_["Content-Length"];
                    int body_length = std::stoi(length);
                    
                    // 3-16需要考虑，当length = 0的时候？？？
                    if(read_buf_.size() < body_length + 2) // +2还有\r\n
                        //数据不完整，继续读
                        return ;
                    else
                        main_state_ = check_state_analyse_content; // 接受完整数据后，转分析
                }
                else //没找到表示出错
                {
                    Set_HttpErrorMsg(http_chanel_->Get_fd(), 400, error_400_respond);
                    error=true;
                    break;
                } 
            }   break;

            /*分析请求，响应报文创建*/
            case check_state_analyse_content:
            {
                if(mp_.count("method")){
                    if(mp_["method"] == "POST")
                        sub_state_ = HttpData::Analyse_Post();
                    else if(mp_["method"]=="GET"|| mp_["method"]=="HEAD")
                        sub_state_=HttpData::Analyse_Get_or_Head();
                    else{
                        //TODO 其他请求方法暂不支持
                        Getlogger()->warn("this http requst not support");
                        return;
                    }

                    if(sub_state_ == analyse_error){
                        error = true;
                        break;
                    }
                    else if(sub_state_ == analyse_success){
                        //finish = true;
                        if(read_buf_.size() <= 2) //读完缓存了
                        { 
                            finish = true;
                        }else // 读下一条
                        {   
                            read_buf_ = read_buf_.substr(2);
                            main_state_ == check_state_requestline;
                        }
                        break;
                    }
                }
                else
                {
                    Getlogger()->error("http data not found method");
                    return;
                }
            }   break;
        }
    }
    //已完成请求读取和响应报文生成，之后需要发送http响应报文
    Callback_OUT();
}

HttpData::sub_state_ParseHTTP HttpData::Parse_Requestline()
{
    /*!
        Http请求报文的请求行的格式为：
        请求方法|空格|URI|空格|协议版本|回车符|换行符。其中URI以‘/’开始。例如：
        GET /index.html HTTP/1.1\r\n
    */
    std::string requestline;
    auto pos = read_buf_.find("\r\n");
    // 没找到，请求行没读完整
    if(pos == std::string::npos)
        return sub_state_ParseHTTP::requestline_data_is_not_complete;
    
    requestline = read_buf_.substr(0,pos);//注意substr是左闭右开
    // read_buf_ = read_buf_.substr(pos+2);
    read_buf_ = read_buf_.substr(pos);  //3-17保留请求行结束的\r\n

    /*正则请求行解析*/
    std::regex reg(R"(^(POST|GET|HEAD)\s(\S*)\s((HTTP)\/\d\.\d)$)");
    std::smatch res;
    std::regex_match(requestline,res,reg);

    if( std::regex_match(requestline,res,reg)){
        mp_["method"] = res[1];
        mp_["URL"] = res[2];
        mp_["version"] = res[3];
        
        return sub_state_ParseHTTP::requestline_is_ok;
    }

    return sub_state_ParseHTTP::requestline_parse_error;
}

HttpData::sub_state_ParseHTTP HttpData::Parse_Header()
{
    /*
        检查报文首部每一行的格式，对首部字段是否正确，字段值是否正确不做判断。
        首部行格式： “字段名+空格+字段值+\r\n”
        此时read_in_buf为\r\n首部+空格+实体
    */
    auto deal_with_perline = [this](std::string str) ->bool{
        std::regex reg(R"(^(\S*)\:\s(.*)$)");  //\s 是匹配所有空白符，包括换行，\S 非空白符，不包括换行
        std::smatch res;
        if(std::regex_match(str,res,reg)){
            mp_[res[1]] = res[2];
            return true;
        }
        return false;
    };

    //逐行解析消息首部
    size_t cur_pos = 0,old_pos = 0;
    while(true){
        //cur_pos下一行的结束，因为是从pos+2开始搜
        // 3-24 bug old_pos 没有更新
        old_pos = cur_pos;
        cur_pos = read_buf_.find("\r\n",old_pos+2);
        if(cur_pos == std::string::npos)     return header_data_is_not_complete;

        if(cur_pos == old_pos+2 )
        {
            //解析到下一行的内容为 \r\n,表示请求完结
            read_buf_ = read_buf_.substr(cur_pos);//注意这里请求体前部的\r\n没有删
            return header_is_ok;
        }
        std::string newline = read_buf_.substr(old_pos+2,cur_pos-old_pos-2);
        if(!deal_with_perline(newline))     return header_parse_error;
    }

    return sub_state_ParseHTTP::header_parse_error;
}

void HttpData::Set_HttpErrorMsg(int fd, int erro_num, std::string msg)
{
    //参考html，error响应报文
    Getlogger()->error("Http erro state {} from {},the msg is {}:",erro_num,fd,msg);
    //设置发送缓冲区的数据，不发送，由Http_send函数发送

    //编写html页面显示
    std::string  response_body{};
    response_body += "<html><title>error</title>";
    response_body += "<body bgcolor=\"ffffff\">";
    response_body += std::to_string(erro_num)+msg;
    response_body += "<hr><em> Hust---CKL Server</em>\n</body></html>";

    //编写header
    std::string response_header{};
    response_header += "HTTP/1.1 " + std::to_string(erro_num) + " " + msg + "\r\n";//---严重的bug，HTTP四个字母必须全部大写，否则浏览器解析不出
    response_header += "Date: " + Gettime() + "\r\n";
    response_header += "Server: Hust---CKL\r\n";
    response_header += "Content-Type: text/html\r\n";
    response_header += "Connection: close\r\n";
    response_header += "Content-length: " + std::to_string(response_body.size())+"\r\n";
    response_header += "\r\n";

    //3-27 bug 非法内存
    write_buf_ += response_header +response_body; 

    // 还是有bug，说是append调用了非法内存，太奇怪了，编译器好像把这两句优化掉了，可能是缓冲区不够会溢出
    // write_buf_.append(response_header);
    // write_buf_.append(response_body);
    //write_buf_ = response_header+response_body;
    return ;
}

HttpData::sub_state_ParseHTTP HttpData::Analyse_Get_or_Head()
{
    /*HEAD 是 GET 的轻量版，响应头完全相同，但是没有响应体，也就是不真正需要资源，用于比如确认文件是否存在*/
    // 先写通用格式
    Write_Response_GeneralData();

    /*按照请求的资源名成，分类返回相应报文*/
    std::string filename = mp_["URL"].substr(mp_["URL"].find_last_of('/')+1);
    if(filename == "hello"){
        write_buf_ += "Content-type: " + std::string("tetx/html\r\n") +
                                         std::string("Content-Length: 10\r\n\r\n") +
                                         std::string("Hello Word");
        return analyse_success;
    }
    // 3-26 bug 把以下注掉，已经修复，在网页中正常显示小图标，单独请求favicon.ico正常显示小图标的图
    // 单独get favicon.ico
    // 3-19这里可能有点问题，因为当前测试，收藏没有显示图标
    // if(filename == "favicon.ico")
    // {
    //     write_buf_ = "Content-Type: " + std::string("image/png\r\n") +
    //                                     std::string("Content-Length: ") + std::to_string(sizeof GlobalValue::Favicon) +
    //                                     std::string("\r\n\r\n") + std::string(GlobalValue::Favicon);
    //     return analyse_success;
    // }

    /*其他filename文件类型请求，报文统一格式 */

    // 支持index首页省略写法
    if(filename == "") filename = "index.html"; 

    //content_type  
    std::string content_type{};
    auto pos = filename.find('.');
    if(pos != std::string::npos)    content_type = SourceMap::Get_file_type(filename.substr(pos));
    else    content_type = SourceMap::Get_file_type("default");

    write_buf_ += "Content-Type: " + content_type + "\r\n";
    
    /*尝试读取资源*/
    std::string file_position = "../resource/" + filename;
    struct stat file_information;
    //先查文件权限
    if(stat(file_position.c_str(),&file_information) == -1){
        //获取失败
        Set_HttpErrorMsg(http_chanel_->Get_fd(),404,error_404_respond);
        //日志记录
        Getlogger()->error("fd {} not found {},404",http_chanel_->Get_fd(),filename);
        return analyse_error;
    }
    //判断文件读取权限
    if(!(file_information.st_mode & S_IROTH)){
        Set_HttpErrorMsg(http_chanel_->Get_fd(),403,error_403_respond);
        // 日志
        Getlogger()->error("fd {} file {} is forbidden ,403",http_chanel_->Get_fd(),filename);
        return analyse_error;
    }

    int lenth = file_information.st_size;
    write_buf_ += "Content-length: " + std::to_string(lenth) +  "\r\n";
    write_buf_ += "\r\n";

    /*
    * 响应请求体书写
    */
    // HEAD不需要报文体
    if(mp_["method"] == "HEAD") return analyse_success;

    //GET，打开并且读取文件，然后填充报文体
    int file_fd = open(file_position.c_str(),O_RDONLY);
    //内存映射 读取文件
    void* addr = mmap(0,lenth,PROT_READ,MAP_PRIVATE,file_fd,0);
    close(file_fd);
    if(addr == MAP_FAILED){
        munmap(addr,lenth);
        // Set_HttpErrorMsg(http_chanel_->Get_fd(),404,error_400_title);
        // 404前面已经处理了，这里应该是单纯内存映射创建读取权限出错, 而且这里一般情况是不会出错的
        // 先写成403forbidden
        Set_HttpErrorMsg(http_chanel_->Get_fd(),403,error_403_respond);
        Getlogger()->error("fd {} file {} mmap fail",http_chanel_->Get_fd(),filename);
        return analyse_error;
    }

    write_buf_ += std::string((char*)addr,lenth);
    munmap(addr,lenth);
    return analyse_success;
}

HttpData::sub_state_ParseHTTP HttpData::Analyse_Post()
{
    /**
     * POST方法用于提交表单，这里只简单把读取到数据转换为大写，然后发回客户端
    */
    Write_Response_GeneralData();

    std::string body = read_buf_.substr(2);
    for(auto& ch:body){
        ch = std::toupper(ch);
    }

    write_buf_ += std::string("Content-Type: text/plain\r\n") + "Content-Length: " + std::to_string(body.size()) + "\r\n";
    write_buf_ += "\r\n" + body;    

    return analyse_success;
}

void HttpData::Write_Response_GeneralData()
{
    std::string status_line = mp_["version"] + "200 OK\r\n";
    std::string headers{};
    headers += "Data: "+ Gettime()+"\r\n";
    headers += "Server: WebServer1.0-CKL\r\n";

    if(mp_["Connection"]=="Keep-Alive" || mp_["Connection"]=="keep-alive")
    {
        headers += "Connection: keep-alive\r\n"
                + std::string("Keep-Alive: ") + std::string(std::to_string(GlobalValue::KeepAliveTime.count())) +"s\r\n";
    }
    else
    {
        headers += "Connection: Connection is closed\r\n";
    }
    write_buf_ = status_line + headers;
    return ;
}

/**
 * 重新注册http事件到epoll中：
 * in为true表示注册EPOLLIN，false表示注册EPOLLOUT
*/
void HttpData::Reset_Http_Events(bool in)
{
    //重新设置监听的事件，但是不修改event.fd，这里采用直接改event.events的方法
    //不确定，先试试，只修改events
    epoll_event newev;
    decltype(newev.events) newevents;
    if(in)  newevents = EPOLLIN|EPOLLERR|EPOLLRDHUP|EPOLLHUP;
    else    newevents = EPOLLOUT|EPOLLERR|EPOLLRDHUP|EPOLLHUP;

    http_chanel_->Set_events(newevents);
    if(!belong_sub_->ModChanel(http_chanel_,newevents)){
        // 日志 
        Getlogger()->error("mod fd {} chanel failed",http_chanel_->Get_fd());
        return;
    }
    return ;
}

void HttpData::Reset()
{    //长连接，修改定时器
    if(mp_["Connection"] == "keep-alive"|| mp_["Connection"] == "Keep-Alive")
    {
        /*重置定时器*/ 
        auto timeout = GlobalValue::KeepAliveTime;

        if(!Get_timer()) {
            auto newtimer = belong_sub_->Get_TimeWheel()->TimeWheel_Insert_Timer(timeout);
            //Set_timer(newtimer);
        }
        else belong_sub_->Get_TimeWheel()->TimeWheel_Adjust_Timer(Get_timer(),timeout);
        //修改keep-alive状态的timeup函数
        Get_timer()->Register_Func([=](){Callback_Dis();});
    }
    //短连接，直接关闭
    else
    {
        Callback_Dis();
        return;
    }

    read_buf_.clear();
    write_buf_.clear();
    mp_.clear();
    main_state_ = main_State_ParseHTTP::check_state_requestline;
}

void HttpData::Callback_IN()
{
    int fd = http_chanel_->Get_fd();

    bool dis_conn = false;
    int res = RecvData(fd,read_buf_,dis_conn);
    if(res == -1 || dis_conn){

        Callback_Dis();
        return;
    }
    //状态机解析
    main_state_ = main_State_ParseHTTP::check_state_requestline;
    state_machine();
}

void HttpData::Callback_OUT()
{
    int fd = http_chanel_->Get_fd();

    int data_sum = write_buf_.size()+1;
    int write_sum = 0;
    bool is_over = false;

    //ET模式，需要完全写入。
    //两重循环:1.在被系统中断的情况，判断是否读取完整，能继续发送完整
    //         2.在send缓冲区满的情况，设置重新注册发送事件，下一次发送
    while(write_sum < data_sum)
    {
        int ret = SendData(fd,write_buf_,is_over);

        if(ret == -1){
            //写数据出错，断开连接
            Callback_Dis();
            return;
        }

            //情况分析：WriteData函数写入了一部分buf的数据，但是系统通过EAGAIN或EWOULDBLOCK信号提示发送send的缓冲区已经满了
            //解决方案：截断缓冲区，把剩下的buf内的数据保存，等待下一次缓冲区可用的时候，再次尝试发送
        if(is_over && write_sum < data_sum)
        { 
            //缓冲区满
            Getlogger()->error(" buffer full while sending to fd {} ",http_chanel_->Get_fd());
            std::cout<<"send buffer full"<<std::endl;

            /*重新注册http事件*/
            //注册
            Reset_Http_Events(false);
            //删除定时器
            belong_sub_->Get_TimeWheel()->TimeWheel_Remove_Timer(http_timer_);
            http_timer_ = nullptr;

            return ;
        }
        write_sum += ret;
    }

    //发送完，重新监听这个fd，重新注册epollin
    Reset_Http_Events(true);

    //重置
    Reset();

}

void HttpData::Callback_ERROR()
{
    //日志记录
    Getlogger()->error("fd {} HTTP call back error",http_chanel_->Get_fd());
    std::cout<<"HTTP call back error"<<std::endl;
    Callback_Dis();
}

/*断开连接*/
void HttpData::Callback_Dis()
{
   
   int fd = http_chanel_->Get_fd();
   //删除Chanel 删除定时器，删除httpdata
   //这里把定时器和httpdata和chanel 都委托给Eventloop的接口 一起删除
//    if(belong_sub_->DelChanel(http_chanel_)){
//         GlobalValue::Dec_CurConnNumber();
//         // 日志记录 输出关闭fd,剩余多少个连接数
//         Getlogger()->info("Clientfd {} disconnect, current user number: {}", fd, GlobalValue::Get_CurConnNumber());
//    }
    // 3-25 考虑在subReactor中吧httpdata、 chanel、timer删个干净。
    // 这里期待的情况是delchenel函数删完之后不回来了，但是怕再回来，这时候httpdata对象已经被删了。TODO
    try
    {
        belong_sub_->DelChanel(http_chanel_);
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "delete disconnect Chanel failed" << ex.what() << std::endl;
    }
    
}

