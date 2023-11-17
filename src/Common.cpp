#include "Common.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

int GlobalValue::CurConnNumber = 0;
std::mutex GlobalValue::CurConnNumber_mtx{};

std::chrono::seconds GlobalValue::HttpPostBodyTime = std::chrono::seconds(888);
std::chrono::seconds GlobalValue::KeepAliveTime = std::chrono::seconds(30);
std::chrono::seconds GlobalValue::HttpHeadTime = std::chrono::seconds(15);

int GlobalValue::BufferMaxSIze = 2048;
int GlobalValue::TimerWheelResolution = 1;

//favicon的创建，确实看不懂
//网站开发者会同时提供 16x16 像素和 32x32 像素两个版本的 favicon.ico 文件
char GlobalValue::Favicon[555] = {
        '\x89', 'P',    'N',    'G',    '\xD',  '\xA',  '\x1A', '\xA',  '\x0',
        '\x0',  '\x0',  '\xD',  'I',    'H',    'D',    'R',    '\x0',  '\x0',
        '\x0',  '\x10', '\x0',  '\x0',  '\x0',  '\x10', '\x8',  '\x6',  '\x0',
        '\x0',  '\x0',  '\x1F', '\xF3', '\xFF', 'a',    '\x0',  '\x0',  '\x0',
        '\x19', 't',    'E',    'X',    't',    'S',    'o',    'f',    't',
        'w',    'a',    'r',    'e',    '\x0',  'A',    'd',    'o',    'b',
        'e',    '\x20', 'I',    'm',    'a',    'g',    'e',    'R',    'e',
        'a',    'd',    'y',    'q',    '\xC9', 'e',    '\x3C', '\x0',  '\x0',
        '\x1',  '\xCD', 'I',    'D',    'A',    'T',    'x',    '\xDA', '\x94',
        '\x93', '9',    'H',    '\x3',  'A',    '\x14', '\x86', '\xFF', '\x5D',
        'b',    '\xA7', '\x4',  'R',    '\xC4', 'm',    '\x22', '\x1E', '\xA0',
        'F',    '\x24', '\x8',  '\x16', '\x16', 'v',    '\xA',  '6',    '\xBA',
        'J',    '\x9A', '\x80', '\x8',  'A',    '\xB4', 'q',    '\x85', 'X',
        '\x89', 'G',    '\xB0', 'I',    '\xA9', 'Q',    '\x24', '\xCD', '\xA6',
        '\x8',  '\xA4', 'H',    'c',    '\x91', 'B',    '\xB',  '\xAF', 'V',
        '\xC1', 'F',    '\xB4', '\x15', '\xCF', '\x22', 'X',    '\x98', '\xB',
        'T',    'H',    '\x8A', 'd',    '\x93', '\x8D', '\xFB', 'F',    'g',
        '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f',    'v',    'f',    '\xDF',
        '\x7C', '\xEF', '\xE7', 'g',    'F',    '\xA8', '\xD5', 'j',    'H',
        '\x24', '\x12', '\x2A', '\x0',  '\x5',  '\xBF', 'G',    '\xD4', '\xEF',
        '\xF7', '\x2F', '6',    '\xEC', '\x12', '\x20', '\x1E', '\x8F', '\xD7',
        '\xAA', '\xD5', '\xEA', '\xAF', 'I',    '5',    'F',    '\xAA', 'T',
        '\x5F', '\x9F', '\x22', 'A',    '\x2A', '\x95', '\xA',  '\x83', '\xE5',
        'r',    '9',    'd',    '\xB3', 'Y',    '\x96', '\x99', 'L',    '\x6',
        '\xE9', 't',    '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',    '\xA7',
        '\xC4', 'b',    '1',    '\xB5', '\x5E', '\x0',  '\x3',  'h',    '\x9A',
        '\xC6', '\x16', '\x82', '\x20', 'X',    'R',    '\x14', 'E',    '6',
        'S',    '\x94', '\xCB', 'e',    'x',    '\xBD', '\x5E', '\xAA', 'U',
        'T',    '\x23', 'L',    '\xC0', '\xE0', '\xE2', '\xC1', '\x8F', '\x0',
        '\x9E', '\xBC', '\x9',  'A',    '\x7C', '\x3E', '\x1F', '\x83', 'D',
        '\x22', '\x11', '\xD5', 'T',    '\x40', '\x3F', '8',    '\x80', 'w',
        '\xE5', '3',    '\x7',  '\xB8', '\x5C', '\x2E', 'H',    '\x92', '\x4',
        '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g',    '\x98', '\xE9',
        '6',    '\x1A', '\xA6', 'g',    '\x15', '\x4',  '\xE3', '\xD7', '\xC8',
        '\xBD', '\x15', '\xE1', 'i',    '\xB7', 'C',    '\xAB', '\xEA', 'x',
        '\x2F', 'j',    'X',    '\x92', '\xBB', '\x18', '\x20', '\x9F', '\xCF',
        '3',    '\xC3', '\xB8', '\xE9', 'N',    '\xA7', '\xD3', 'l',    'J',
        '\x0',  'i',    '6',    '\x7C', '\x8E', '\xE1', '\xFE', 'V',    '\x84',
        '\xE7', '\x3C', '\x9F', 'r',    '\x2B', '\x3A', 'B',    '\x7B', '7',
        'f',    'w',    '\xAE', '\x8E', '\xE',  '\xF3', '\xBD', 'R',    '\xA9',
        'd',    '\x2',  'B',    '\xAF', '\x85', '2',    'f',    'F',    '\xBA',
        '\xC',  '\xD9', '\x9F', '\x1D', '\x9A', 'l',    '\x22', '\xE6', '\xC7',
        '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15', '\x90', '\x7',  '\x93',
        '\xA2', '\x28', '\xA0', 'S',    'j',    '\xB1', '\xB8', '\xDF', '\x29',
        '5',    'C',    '\xE',  '\x3F', 'X',    '\xFC', '\x98', '\xDA', 'y',
        'j',    'P',    '\x40', '\x0',  '\x87', '\xAE', '\x1B', '\x17', 'B',
        '\xB4', '\x3A', '\x3F', '\xBE', 'y',    '\xC7', '\xA',  '\x26', '\xB6',
        '\xEE', '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
        '\xA',  '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X',    '\x0',  '\x27',
        '\xEB', 'n',    'V',    'p',    '\xBC', '\xD6', '\xCB', '\xD6', 'G',
        '\xAB', '\x3D', 'l',    '\x7D', '\xB8', '\xD2', '\xDD', '\xA0', '\x60',
        '\x83', '\xBA', '\xEF', '\x5F', '\xA4', '\xEA', '\xCC', '\x2',  'N',
        '\xAE', '\x5E', 'p',    '\x1A', '\xEC', '\xB3', '\x40', '9',    '\xAC',
        '\xFE', '\xF2', '\x91', '\x89', 'g',    '\x91', '\x85', '\x21', '\xA8',
        '\x87', '\xB7', 'X',    '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N',
        'N',    'b',    't',    '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
        '\xEC', '\x86', '\x2',  'H',    '\x26', '\x93', '\xD0', 'u',    '\x1D',
        '\x7F', '\x9',  '2',    '\x95', '\xBF', '\x1F', '\xDB', '\xD7', 'c',
        '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF', '\x22', 'J',    '\xC3',
        '\x87', '\x0',  '\x3',  '\x0',  'K',    '\xBB', '\xF8', '\xD6', '\x2A',
        'v',    '\x98', 'I',    '\x0',  '\x0',  '\x0',  '\x0',  'I',    'E',
        'N',    'D',    '\xAE', 'B',    '\x60', '\x82',
};

std::shared_ptr<spdlog::logger> Getlogger(std::string path)
{
    try
    {
        //创建静态对象，之后都返回这个对象，也就是路径固定
        static auto my_logger = spdlog::basic_logger_mt("basic_logger", path);
        return my_logger;
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }

    return {};
}

int RecvData(int fd, std::string &read_buf, bool &is_over)
{

    int read_sum = 0;

    //ET模式，一次性读取
    /**
    对非阻塞I/O：
         1.若当前没有数据可读，函数会立即返回-1，同时errno被设置为EAGAIN或EWOULDBLOCK。
           若被系统中断打断，返回值同样为-1,但errno被设置为EINTR。对于被系统中断的情况，
           采取的策略为重新再读一次，因为我们无法判断缓冲区中是否有数据可读。然而，对于
           EAGAIN或EWOULDBLOCK的情况，就直接返回，因为操作系统明确告知了我们当前无数据
           可读。
         2.若当前有数据可读，那么recv函数并不会立即返回，而是开始从内核中将数据拷贝到用
           户区，这是一个同步操作，返回值为这一次函数调用成功拷贝的字节数。所以说，非阻
           塞I/O本质上还是同步的，并不是异步的。
    */
    while(true){

        char buf[GlobalValue::BufferMaxSIze];
        memset(buf,'\0',sizeof(buf));

        int ret = recv(fd,buf,sizeof buf,0);
        if(ret == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK) return read_sum;

            else if(errno == EINTR) continue;

            else{
                /**
                 * 这里如果返回read_num,表示读取错误了还要回写数据，这是错误的，应该直接返回 -1 显示出错 
                 * 这边是触发EPOLLIN事件，后处理RDHUP 
                */
                Getlogger()->error("clientfd {} RecvData failed: {}",fd,strerror(errno));
                return -1;
            }
        }
        else if(ret == 0){
            Getlogger()->debug("clientfd {} has close the connection", fd); 
            is_over = true;
            break;
        }
        read_buf += buf;
        read_sum += ret;
    }
    return read_sum;
}

int SendData(int fd, std::string &buf, bool &is_over)
{
    int write_sum = 0;
    const char* data_to_send = buf.c_str();
    size_t data_length = buf.size() + 1;

    while(data_length){
        int ret = send(fd,data_to_send,data_length,0);
        if(ret == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                is_over = true;
                return write_sum;
            }

            else if(errno == EINTR) continue;

            else{
                Getlogger()->error("clientfd {} SendData failed: {}",fd,strerror(errno));
                return -1;
            }
        }
        //更新写入缓存
        write_sum += ret;
        data_length -= ret;
        data_to_send += ret;
    }

    //写完清理写入缓存区,否则截断，返回剩余字符
    if(buf.size()+1 == write_sum )  buf.clear();
    else buf = buf.substr(write_sum);

    return write_sum;
}

int Write_to_fd(int fd, const char *content, int length)
{
    const char* position = content;
    int ret_sum = 0, ret;
    while(ret_sum < length){
        ret = write(fd,position,length-ret_sum);
        if(ret == -1){
            if(errno == EINTR) continue;    //可能是系统中断了，继续写
            if(errno == EAGAIN) return ret; //缓冲区满了
            Getlogger()->error("write data failed to fd {} error: {}", fd, strerror(errno)); 
            return -1;  
        }

        ret_sum += ret;
        position += ret;
    }
    return ret_sum;
}


int Read_from_fd(int fd, const char *buf, int length)
{
    int ret_sum = 0 ,ret;
    const char* pos = buf;
    while (true)
    {
        ret = read(fd,(void *)pos,length-ret_sum);
        if(ret == -1){
            if(errno == EINTR)   continue; //忽略系统中断
            if(errno == EAGAIN) return ret_sum; //已无数据可读       

            Getlogger()->error("read data failed from fd {} error: {}", fd, strerror(errno)); 
            return -1;
            
        }else if(ret == 0){
            return ret_sum;
        }

        ret_sum += ret;
        pos += ret;      
    }
    return ret_sum;
}


std::optional<std::tuple<int, int, std::string>> parseCommandLine(int argc, char *argv[])
{
    const char* str = "p:s:l:";
    int res;
    unsigned int port, subReactorSize;
    std::string log_path;
    // 解析命令行./server -p Port -s SubReactorSize -l LogPath(start with ./)
    while((res = getopt(argc,argv,str)) != -1){
        switch (res)
        {
        case 'p':
            port = atoi(optarg);
            assert(port > 0 && port <65535);
            break;
        case 's':
            subReactorSize = atoi(optarg);
            assert(subReactorSize > 0);
            break;;
        case 'l':{
            std::regex PathExpression(R"(^\.\/\S*)");   // 匹配./开头字符串，提取log文件路径
            std::smatch matchRes{};     // 正则匹配结果 内部是vector，存储了所有匹配的对象
            std::string path(optarg);
            if(!std::regex_match(path,matchRes,PathExpression)){
                std::cout <<"ilegal log path\n"<<std::endl;
                return std::nullopt;
            }
            log_path = matchRes[0];
        }
            break;
        default:
            break;
        }

    }
    std::optional<std::tuple<int, int, std::string>> opt {std::make_tuple(port,subReactorSize,log_path)};
    return opt;
}

int setnoblocking(int fd)
{
    int old_flag;
    if((old_flag = fcntl(fd,F_GETFL)) == -1){
        std::cout<<strerror(errno)<<std::endl;
        return -1;
    }

    int new_flag = old_flag|O_NONBLOCK;
    if(fcntl(fd,F_SETFL,new_flag) == -1)
    {
        std::cout<<strerror(errno)<<std::endl;
        return -1;
    }

    return 0;
}

int BindAndListen(int port)
{
    // if(port < 0 ||port > 65535) return -1;
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    if(listenfd < 0){
        close(listenfd);
        Getlogger()->error("create listenfd failed: {}", strerror(errno));
        return -1;
    }

    //设置端口复用
    int reuse = 1;
    int ret = setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
    if(ret == -1){
        Getlogger()->error("faied to set listenfd SO_REUSEADDR");
        close(listenfd);
        return -1;
    }

    struct sockaddr_in saddr{};
    bzero(&saddr,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(port);

    ret = bind(listenfd,reinterpret_cast<const sockaddr*>(&saddr),sizeof saddr);

    if(ret == -1){
        Getlogger()->error("faied to bind listenfd");
        std::cout<<errno<<std::endl;
        close(listenfd);
        return -1;
    }

    ret =listen(listenfd,4096);// 第二个参数 backlog参数定义挂起队列的最大长度
    if(ret == -1){
        Getlogger()->error("faied to listen listenfd ");
        close(listenfd);
        return -1;
    }

    return listenfd;
}

std::string Gettime()
{
    time_t lt = time(NULL); // 获取时间戳（秒数）
    struct tm* ptime = localtime(&lt); // 转换为当地时间

    std::string timebuf;
    strftime(timebuf.data(),100,"%a,%d %b %Y %H:%M:%S",ptime);//格式化时间字符串
    return timebuf;
}
