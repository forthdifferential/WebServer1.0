# Linux服务器搭建入门课

## 1. Linux系统编程入门

### GCC

gcc和g++都是GCC组织的编译器 ，套件和库；一般用gcc和g++两个编译器

* gcc不能自动和cpp程序库链接，一般编译和链接都用g++，因为g++会调用gcc

* gcc编译c文件 gcc test.c -o app 然后./app 
  其中-o 指明输出文件名

* 预处理 编译 汇编 链接 

* -c 只是编译不链接,生成目标文件“.o”  test.o

  -S 只是编译不汇编,生成汇编代码  test.s

  -E 只进行预编译,不做其他处理    test.i

  

  **后置** ：

* -D 指定宏 配合#ifdef DEBUG #endif
  g++ test.cpp -o test -DDEBUG *指定宏DEGUB*
* -I 指定include包含文件搜索目录
* -L 指定编译时 库 的路径
* -l 指定编译时 使用的 库

![image-20230214150013734](D:\MyTxt\typoraPhoto\image-20230214150013734.png)

![image-20230214151216633](D:\MyTxt\typoraPhoto\image-20230214151216633.png)

### 库

* 代码仓库，保存一些变量、函数、类等；编写差不多，但不能单独运行；

* 静态库在程序的链接阶段被复制到程序中；动态库在程序运行时，加载到内存中供程序调用

* 好处：1. 代码保密（cpp反汇编还原度低）；2. 方便部署和分发

* **工作原理**：静态库GCC链接时，把静态库代码打包到可执行程序中；
                    动态度GCC链接时，动态库代码**不会**被打包到可执行程序中，运行时加载到内存中，通过ldd(list dynamic dependencies)命令检查动态库依赖关系；系统加载可执行代码时，需要知道库的名字和绝对路径，一般用**动态载入器获取绝对路径，然后找到库文件载入内存中**

   **优缺点比较**：

  * 静态库被打包到应用程序 ；加载速度快；
        但消耗资源，浪费内存； 更新速度慢；
  *  动态库 可以进程资源共享（共享库）；更新部署简单；控制加载时间；
       但加载速度慢；发布程序需要提供依赖的动态库；
  * 库比较小用静态 大用动态

#### 静态库

* 命名规则 Linux:    libxxx.a
  				Win :     libxxx.lib

* 制作静态库

  1. gcc获得.o文件 - c

  2. 将 .o文件打包，使用ar工具 
     ~~~shell
     ar rcs libxxx.a  xxx.o xxx.o
     ~~~

     静态库移动到lib目录下

  3. 编译main程序
     ~~~shell 
     gcc main.c -o app -I ./include/ -L ./lib/ -l xxx
     ~~~

     ![image-20230214160115050](D:\MyTxt\typoraPhoto\image-20230214160115050.png)

#### 动态库

* 命名规则 Linux:   libxxx.so 在Linux下是一个可执行文件
  				Win :    libxxx.dll

* 制作动态库

  1. gcc得到.o文件，得到与位置无关的代码
     ~~~shell	
     gcc -c -fpic/-fPIC a.c b.c
     ~~~

     -fpic 用于编译阶段，产生的代码没有绝对地址，全部用相对地址，这正好满足了共享库的要求，共享库被加载时地址不是固定的。如果不加-fpic ，那么生成的代码就会与位置有关，当进程使用该.so文件时都需要重定位，且会产生成该文件的副本，每个副本都不同，不同点取决于该文件代码段与数据段所映射内存的位置

  2. gcc得到动态库
     ~~~shell
     gcc -shared a.o b.o -o libcalc.so
     ~~~

  3. 编译mian程序
     ~~~shell
     gcc main.c -o app2 -I ./include/ -L ./lib/ -l calc
     ~~~

     

     运行时加载到内存中，通过ldd(list dynamic dependencies)命令检查动态库依赖关系；系统加载可执行代码时，需要知道库的名字和绝对路径，一般用**动态载入器获取绝对路径，然后找到库文件载入内存中**

     1. 窗口级别 直接export配置环境变量：

        ~~~shell
        klchen@vmware:~/WinToUbuntu/lession06/library$ export LD_LIBRARY_PATH=$LD_LIBRAY_PATH:/home/klchen/WinToUbuntu/lession06/library/lib
        ~~~

     2. 用户级别 在用户中配置.bashrc环境变量
        ~~~shell
        vim .bashrc
        
        export LD_LIBRARY_PATH=$LD_LIBRAY_PATH:/home/klchen/WinToUbuntu/lession06/library/lib
        
        . .bashrc或者source .bashrc
        ~~~

     3. 系统级别 在etc/profile中export

     * 还有/etc/ld.so.cache文件列表添加的方式

### MakeFile

一个工程中的源文件不计其数，其按类型、功能、模块分别放在若干个目录中，Makefile文件定义了一系列的规则来指定哪些文件需要先编译，哪些文件需要后编译，哪些文件需要重新编译，甚至于进行更复杂的功能操作，因为Makefile文件就像一个Shell脚本一样，也可以执行操作系统的命令。

◼Makefile带来的好处就是“自动化编译”，一旦写好，只需要一个make命令，整个工程完全自动编译，极大的提高了软件开发的效率。

* make是一个命令工具，是一个解释Makefile文件中指令的命令工具，一般来说，大多数的IDE都有这个命令，比如Delphi的make，VisualC++的nmake，Linux下GNU的make。

#### makefile文件书写规则

~~~makefile
#定义变量
src=$(wildcard ./*c)
objs=$(patsubst %.c, %.o, $(src))
target=app
$(target):$(objs)
        $(CC) $(objs) -o $(target) 
%.o:%.c
        $(CC) -c $< -o $@
 #clean操作在make不会被执行，需要手动make clean
 #表示clean是一个为目标，不会生成文件
 .PHONY:clean
 clean:
 		rm $(objs) -f
~~~

![image-20230214231348841](D:\MyTxt\typoraPhoto\image-20230214231348841.png)

![image-20230214231358292](D:\MyTxt\typoraPhoto\image-20230214231358292.png)

![image-20230215135331986](D:\MyTxt\typoraPhoto\image-20230215135331986.png)

![image-20230215135347416](D:\MyTxt\typoraPhoto\image-20230215135347416.png)

![image-20230215135357950](D:\MyTxt\typoraPhoto\image-20230215135357950.png)

![image-20230215135426200](D:\MyTxt\typoraPhoto\image-20230215135426200.png)

### GDB

​	调试工具，是许多类Unix系统中的标准开发环境

* 主要功能：断点调试，监控，改BUG

* 准备工作 在生成可执行文件时添加

  1. -O关掉优化选项
  2. -g打开调试选项：可执行文件中加入源代码信息，第几条机器指令对应源代码几行
  3. -Wall打开所有warning

* gdb基本操作
  ~~~shell
  #应该要保证可执行文件和源文件都在
  gdb 目标程序
  ~~~

  ![image-20230215165845215](D:\MyTxt\typoraPhoto\image-20230215165845215.png)

* 断点操作

  **退出gdb后断点全部失效**

  ![image-20230215185707288](D:\MyTxt\typoraPhoto\image-20230215185707288.png)

* 调试命令 

  ![image-20230215190042661](D:\MyTxt\typoraPhoto\image-20230215190042661.png)

### Linux系统的IO函数

 标准C库IO函数带有缓冲区，有FILE*fp文件指针，减少写磁盘次数；但是Linux系统自带的IO没有缓冲区

* 在网络通信时候用linux系统自带的IO；

* 在磁盘读写用标准C库IO；

* FILE类型 文件描述符

* 库函数说明查找
  
  ~~~shell
  #Linux库函数
  man 2 xxx
  #标准C库
  man 3 xxx
  ~~~

##### open函数

* 用man操作打开函数的说明文档，可以查找头文件，看IO

  man 1是普通的shell命令比如ls，
       man 2是系统调用比如open，write说明，
          man 3是函数说明，一些库函数

##### 打开文件

​       int open(const char *pathname, int flags);的说明

~~~c
头文件：
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>

打开文件操作

1. 打开一个已经存在的文件
   int open(const char *pathname, int flags);
       参数：

      - pathname：要打开的文件路径
        - flags：对文件的操作权限设置还有其他的设置
              - flO_RDONLY,  O_WRONLY,  O_RDWR  这三个设置是互斥的
      - 返回值：返回一个新的文件描述符，如果调用失败，返回-1

2. errno：属于Linux系统函数库，库里面的一个全局变量，记录的是最近的错误号。

   #include <stdio.h>
   void perror(const char *s);作用：打印errno对应的错误描述
       s参数：用户描述，比如hello,最终输出的内容是  hello:xxx(实际的错误描述)
       创建一个新的文件
   int open(const char *pathname, int flags, mode_t mode);
~~~

例子

~~~c
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <stdio.h>
    #include <unistd.h>

    int main(){
        
        int fd = open("a.txt",O_RDONLY);
        /*man 1是普通的shell命令比如ls，
        man 2是系统调用比如open，write说明，
        man 3是函数说明，一些库函数。        
        */
        if(fd==-1){
            perror("open");
        }
        //关闭文件描述符
        close(fd);

        return 0;
    }
~~~

##### 创建文件

```c

 int open(const char *pathname, int flags, mode_t mode);的说明参数：
    - pathname：要创建的文件的路径
    - flags：对文件的操作权限和其他的设置
        - 必选项：O_RDONLY,  O_WRONLY, O_RDWR  这三个之间是互斥的，就是权限的设置只读、只写、读写
        - 可选项：O_CREAT 文件不存在，创建新文件
    - mode：八进制的数，表示创建出的新的文件的操作权限，比如：0775
    最终的权限是：mode & ~umask
    0777   ->   111111111
&   0775   ->   111111101
----------------------------
                111111101
按位与：0和任何数都为0
umask的作用就是抹去某些权限，也可以自己

flags参数是一个int类型的数据，占4个字节，32位。
flags 32个位，每一位就是一个标志位。
```

例子

~~~c
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <stdio.h>
    #include <unistd.h>

    int main(){
        
        int fd = open("create.txt",O_RDONLY|O_CREAT,0777);

        if(fd==-1){
            perror("open");
        }
        //关闭文件描述符
        close(fd);

        return 0;
    }
~~~

#### read函数

```c
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
    参数：
        - fd：文件描述符，open得到的，通过这个文件描述符操作某个文件
        - buf：需要读取数据存放的地方，数组的地址（传出参数）
        - count：指定的数组的大小
    返回值：
        - 成功：
            >0: 返回实际的读取到的字节数
            =0：文件已经读取完了
        - 失败：-1 ，并且设置errno
```
#### write函数

```c
ssize_t write(int fd, const void *buf, size_t count);
    参数：
        - fd：文件描述符，open得到的，通过这个文件描述符操作某个文件
        - buf：要往磁盘写入的数据，数据
        - count：要写的数据的实际的大小
          write(fd, " ", 1);
    返回值：
        成功：实际写入的字节数
        失败：返回-1，并设置errno
```
例子：读写复制文件

~~~c
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <stdio.h>
    #include <unistd.h>

    int main(){
        //open打开资源文件
        int srcfd = open("english.txt",O_RDONLY);

        //创建拷贝文件
        int destfd = open("copy.txt",O_WRONLY|O_CREAT,0664);
        if(destfd==-1){
            perror("open");
        }
        //频繁的读写操作
        char buf[1024];
        
        int len=0;
        while((len = read(srcfd,buf,sizeof(buf)))>0){
            write(destfd,buf,len);
        }

        //关闭文件描述符
        close(srcfd);
        close(destfd);

        return 0;
    }
~~~

#### lseek移动文件指针偏移

```c
    标准C库的函数
  #include <stdio.h>
  int fseek(FILE *stream, long offset, int whence);
    Linux系统函数
#include <sys/types.h>
#include <unistd.h>
off_t lseek(int fd, off_t offset, int whence);
    参数：
        - fd：文件描述符，通过open得到的，通过这个fd操作某个文件
        - offset：偏移量
        - whence:
            SEEK_SET
                设置文件指针的偏移量
            SEEK_CUR
                设置偏移量：当前位置 + 第二个参数offset的值
            SEEK_END
                设置偏移量：文件大小 + 第二个参数offset的值
    返回值：返回文件指针的位置
    作用：
    1.移动文件指针到文件头
    lseek(fd, 0, SEEK_SET);

    2.获取当前文件指针的位置
    lseek(fd, 0, SEEK_CUR);

    3.获取文件长度
    lseek(fd, 0, SEEK_END);

    4.拓展文件的长度，当前文件10b, 110b, 增加了100个字节
    lseek(fd, 100, SEEK_END)
    注意：需要写一次数据
```

~~~c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {

    int fd = open("hello.txt", O_RDWR);

    if(fd == -1) {
        perror("open");
        return -1;
    }

    // 扩展文件的长度
    int ret = lseek(fd, 100, SEEK_END);
    if(ret == -1) {
        perror("lseek");
        return -1;
    }

    // 写入一个空数据
    write(fd, " ", 1);

    // 关闭文件
    close(fd);

    return 0;
}
~~~

#### stat函数

* 终端操作

~~~shell
stat xxx
~~~

##### stat函数 lstat函数获取文件信息

```c
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
int stat(const char *pathname, struct stat *statbuf);
    作用：获取一个文件相关的一些信息
    参数:
        - pathname：操作的文件的路径
        - statbuf：结构体变量，传出参数，用于保存获取到的文件的信息
    返回值：
        成功：返回0
        失败：返回-1 设置errno

int lstat(const char *pathname, struct stat *statbuf);获取软链接的文件信息
    参数:
        - pathname：操作的文件的路径
        - statbuf：结构体变量，传出参数，用于保存获取到的文件的信息
    返回值：
        成功：返回0
        失败：返回-1 设置errno
```

![image-20230218161815168](D:\MyTxt\typoraPhoto\image-20230218161815168.png)

例子：用stat实现ls -l

~~~c

#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

//模拟ls-l指令
//-rw-rw-r-- 1 klchen klchen 0 2月  18 14:01 a.txt

int main(int argc,char* argv[]){

    if(argc<2){
        printf("%s filename\n",argv[0]);
        return -1;
    }
    //通过stat函数获取用户传入文件的信息
    struct stat st;
    int ret=stat(argv[1],&st);
    if(ret==-1){
        perror("stat");
        return -1;
    }

    //获取文件类型和文件权限
    char perms[11] ={0};
    switch (st.st_mode & __S_IFMT)
    {
        case __S_IFLNK:
            perms[0]='l';
            break;
        case __S_IFDIR:
            perms[0]='d';
            break;
        case __S_IFREG:
            perms[0]='-';
            break;
        case __S_IFBLK:
            perms[0]='b';
            break;    
        case __S_IFCHR:
            perms[0]='c';
            break;
        case __S_IFSOCK:
            perms[0]='s';
            break;    
        case __S_IFIFO:
            perms[0]='p';
            break;   
        default:
            perms[0]='?';
            break;
    }
    
    //判断文件访问权限
    perms[1]= st.st_mode & S_IRUSR ? 'r':'-';
    perms[2]= st.st_mode & S_IWUSR ? 'w':'-';
    perms[3]= st.st_mode & S_IXUSR ? 'x':'-';

    perms[4]= st.st_mode & S_IRUSR ? 'r':'-';
    perms[5]= st.st_mode & S_IWGRP ? 'w':'-';
    perms[6]= st.st_mode & S_IXGRP ? 'x':'-';

    perms[7]= st.st_mode & S_IROTH ? 'r':'-';
    perms[8]= st.st_mode & S_IWOTH ? 'w':'-';
    perms[9]= st.st_mode & S_IXOTH ? 'x':'-';

    //硬链接数
    int linkNum =st.st_nlink;


    //文件所有者
    char *fileUser = getpwuid(st.st_uid)->pw_name;

    //文件所在组
    char *fileGrp=getgrgid(st.st_uid)->gr_name;

    //获取文件大小
    long int fileSize =st.st_size;

    //获取修改时间
    char *time = ctime(&st.st_mtime);
    char mtime[512]={0};
    strncpy(mtime,time,strlen(time) -1);

    
    char buf[1024];
    sprintf(buf,"%s %d %s %s %ld %s %s",perms,linkNum,fileUser,fileGrp,fileSize,mtime,argv[1]);
    
    printf("%s\n",buf);

    return 0;
}
~~~

#### 文件属性操作函数

##### access判断文件权限或文件是否存在

    #include <unistd.h>
    int access(const char *pathname, int mode);
        作用：判断某个文件是否有某个权限，或者判断文件是否存在
        参数：
            - pathname: 判断的文件路径
            - mode:
                R_OK: 判断是否有读权限
                W_OK: 判断是否有写权限
                X_OK: 判断是否有执行权限
                F_OK: 判断文件是否存在
        返回值：成功返回0， 失败返回-1

例子：

~~~c
#include <unistd.h>
#include <stdio.h>

int main() {

    int ret = access("a.txt", F_OK);
    if(ret == -1) {
        perror("access");
    }

    printf("文件存在！！!\n");

    return 0;
}
~~~

##### chmod函数修改文件权限

```c
#include <sys/stat.h>
int chmod(const char *pathname, mode_t mode);
    修改文件的权限
    参数：
        - pathname: 需要修改的文件的路径
        - mode:需要修改的权限值，八进制的数
    返回值：成功返回0，失败返回-1
```

例子：

```c
#include <sys/stat.h>
#include <stdio.h>
int main() {
    int ret = chmod("a.txt", 0777);

if(ret == -1) {
    perror("chmod");
    return -1;
}

return 0;
}
```
##### truncate修改文件尺寸

```c
#include <unistd.h>
#include <sys/types.h>
int truncate(const char *path, off_t length);
    作用：缩减或者扩展文件的尺寸至指定的大小
    参数：
        - path: 需要修改的文件的路径
        - length: 需要最终文件变成的大小
    返回值：
        成功返回0， 失败返回-1
```

例子

```c
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
int ret = truncate("b.txt", 5);

if(ret == -1) {
    perror("truncate");
    return -1;
}

return 0;
}
```
#### 目录操作函数

##### mkdir函数创建目录

```c
#include <sys/stat.h>
#include <sys/types.h>
int mkdir(const char *pathname, mode_t mode);
    作用：创建一个目录
    参数：
        pathname: 创建的目录的路径
        mode: 权限，八进制的数
    返回值：
        成功返回0， 失败返回-1
```

##### chdir修改进程工作目录

```c
#include <unistd.h>
int chdir(const char *path);
    作用：修改进程的工作目录
        比如在/home/nowcoder 启动了一个可执行程序a.out, 进程的工作目录 /home/nowcoder
    参数：
        path : 需要修改的工作目录
```

##### getcwd获取当前目录

```c
#include <unistd.h>
char *getcwd(char *buf, size_t size);
    作用：获取当前工作目录
    参数：
        - buf : 存储的路径，指向的是一个数组（传出参数）
        - size: 数组的大小
    返回值：
        返回的指向的一块内存，这个数据就是第一个参数
```

例子

~~~c
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main() {

    // 获取当前的工作目录
    char buf[128];
    getcwd(buf, sizeof(buf));
    printf("当前的工作目录是：%s\n", buf);

    // 修改工作目录
    int ret = chdir("/home/nowcoder/Linux/lesson13");
    if(ret == -1) {
        perror("chdir");
        return -1;
    } 

    // 创建一个新的文件
    int fd = open("chdir.txt", O_CREAT | O_RDWR, 0664);
    if(fd == -1) {
        perror("open");
        return -1;
    }

    close(fd);

    // 获取当前的工作目录
    char buf1[128];
    getcwd(buf1, sizeof(buf1));
    printf("当前的工作目录是：%s\n", buf1);
    
    return 0;
}
~~~

#### 目录遍历函数

##### opendir()

```c
// 打开一个目录
#include <sys/types.h>
#include <dirent.h>
DIR *opendir(const char *name);
    参数：
        - name: 需要打开的目录的名称
    返回值：
        DIR * 类型，理解为目录流
        错误返回NULL
```

##### readdir()

```c
#include <dirent.h>
struct dirent *readdir(DIR *dirp);
    - 参数：dirp是opendir返回的结果
    - 返回值：
        struct dirent，代表读取到的文件的信息
        读取到了末尾或者失败了，返回NULL
//返回值 dirent结构体的具体 元素 如图
```

![image-20230219211206835](D:\MyTxt\typoraPhoto\image-20230219211206835.png)

##### closedir()

```c
// 关闭目录
#include <sys/types.h>
#include <dirent.h>
int closedir(DIR *dirp);//此处 参数 关闭的是文件描述符
```

###### 应用-统计某个目录下文件数量

~~~c
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

int getFilenum(const char* path);

//读取某目录下所有文件个数
int main(int argc, char* argv[]){

    //判断是否输入了文件名
    if(argc<2){
        printf("%s path\n",argv[0]);
        return -1;
    }
    int num=getFilenum(argv[1]);
    printf("平台文件的个数是 %d\n",num);

    return 0;
}

int getFilenum(const char* path){
    //打开目录
    DIR *dir=opendir(path);
    if(dir==NULL){
        perror("opendir");
        return -1;
    }

    struct dirent *ptr;
    //记录普通文件数
    int total=0;

    
    while((ptr=readdir(dir))!=NULL){

        //忽略掉. 和..
        char* dname=ptr->d_name;
        if(strcmp(dname,".")==0 || strcmp(dname,"..")==0 ){
            continue;
        }

        //判断是否是目录递归
        if(ptr->d_type == DT_DIR){
            //目录，需要继续读取目录
            char newpath[256];
            //生成地址名称，字符串合并函数
            sprintf(newpath,"%s/%s",path,dname);
            total+=getFilenum(newpath);
        }
        if(ptr->d_type == DT_REG){
            //普通文件
            total++;
        }

    }

    //关闭目录
    closedir(dir);

    return total;
}
~~~

#### 文件描述符操作函数

一个进程中有一个文件描述符表



##### dup()文件描述符 复制 - 浅拷贝

    #include <unistd.h>
    int dup(int oldfd);
        作用：复制一个新的文件描述符
        fd=3, int fd1 = dup(fd),
        fd指向的是a.txt, fd1也是指向a.txt
        从空闲的文件描述符表中找一个最小的，作为新的拷贝的文件描述符

例子：

~~~c
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main() {

    int fd = open("a.txt", O_RDWR | O_CREAT, 0664);

    int fd1 = dup(fd);

    if(fd1 == -1) {
        perror("dup");
        return -1;
    }

    printf("fd : %d , fd1 : %d\n", fd, fd1);

    close(fd);

    char * str = "hello,world";
    int ret = write(fd1, str, strlen(str));
    if(ret == -1) {
        perror("write");
        return -1;
    }

    close(fd1);

    return 0;
}
~~~

##### dup2()文件描述符 重定向

```c
#include <unistd.h>
int dup2(int oldfd, int newfd);
    作用：重定向文件描述符,让newfd也来指向oldfd
    oldfd 指向 a.txt, newfd 指向 b.txt
    调用函数成功后：newfd 和 b.txt 做close, newfd 指向了 a.txt
    oldfd 必须是一个有效的文件描述符
    oldfd和newfd值相同，相当于什么都没有做
```

例子：

~~~c
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main() {

    int fd = open("1.txt", O_RDWR | O_CREAT, 0664);
    if(fd == -1) {
        perror("open");
        return -1;
    }

    int fd1 = open("2.txt", O_RDWR | O_CREAT, 0664);
    if(fd1 == -1) {
        perror("open");
        return -1;
    }

    printf("fd : %d, fd1 : %d\n", fd, fd1);

    int fd2 = dup2(fd, fd1);
    if(fd2 == -1) {
        perror("dup2");
        return -1;
    }

    // 通过fd1去写数据，实际操作的是1.txt，而不是2.txt
    char * str = "hello, dup2";
    int len = write(fd1, str, strlen(str));

    if(len == -1) {
        perror("write");
        return -1;
    }

    printf("fd : %d, fd1 : %d, fd2 : %d\n", fd, fd1, fd2);

    close(fd);
    close(fd1);

    return 0;
}
~~~

##### fcntl()函数 控制文件描述符

```c
#include <unistd.h>
#include <fcntl.h>

int fcntl(int fd, int cmd, ...);
参数：
    fd : 表示需要操作的文件描述符
    cmd: 表示对文件描述符进行如何操作
        - F_DUPFD : 复制文件描述符,复制的是第一个参数fd，得到一个新的文件描述符（返回值）
            int ret = fcntl(fd, F_DUPFD);

        - F_GETFL : 获取指定的文件描述符文件状态flag
          获取的flag和我们通过open函数传递的flag是一个东西。//这个flag都可以|上O_NONBLOCK
            //设置fd非阻塞的方法，一般是fcntl+F_GETFL得到flag，|上O_NONBLOCK，然后fcntl+F_SETFL设置falg

        - F_SETFL : 设置文件描述符文件状态flag
          必选项：O_RDONLY, O_WRONLY, O_RDWR 不可以被修改！！！！！
          可选性：O_APPEND, O_NONBLOCK
            O_APPEND 表示追加数据
            NONBLOK 设置成非阻塞
    
    阻塞和非阻塞：描述的是函数调用的行为。 导致当前进程线程被挂起-阻塞
```

例子

~~~c
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main() {

    // 1.复制文件描述符
    // int fd = open("1.txt", O_RDONLY);
    // int ret = fcntl(fd, F_DUPFD);

    // 2.修改或者获取文件状态flag
    int fd = open("1.txt", O_RDWR);
    if(fd == -1) {
        perror("open");
        return -1;
    }
     // 修改文件描述符状态的flag，给flag加入O_APPEND这个标记
    // 首先要获取文件描述符状态flag
    int flag = fcntl(fd, F_GETFL);
    if(flag == -1) {
        perror("fcntl");
        return -1;
    }
    
    //修改完flag后替换进去
    flag |= O_APPEND;   // flag = flag | O_APPEND 此处用按位或来追加flag状态
    int ret = fcntl(fd, F_SETFL, flag);
    if(ret == -1) {
        perror("fcntl");
        return -1;
    }

    char * str = "nihao";
    write(fd, str, strlen(str));

    close(fd);

    return 0;
}
~~~

## 2. Linux多进程编程

### 2.1进程状态转换  -终端指令

* Linux中PBC的结构体task_struct
* 进程参数查看 ulimit -a 
* 查看进程
  ps aux / ajx
  a：显示终端上的所有进程，包括其他用户的进程
  u：显示进程的详细信息
  x：显示没有控制终端的进程
  j：列出与作业控制相关的信息

* STAT参数意义：
  D 不可中断 Uninterruptible（usually IO）
  R 正在运行，或在队列中的进程
  S(大写) 处于休眠状态
  T 停止或被追踪
  Z 僵尸进程
  W 进入内存交换（从内核2.6开始无效）
  X 死掉的进程
  < 高优先级
  N 低优先级
  s 包含子进程

  \+ 位于前台的进程组

* 然后，实时显示进程动态
  top
  可以在使用 top 命令时加上 -d 来指定显示信息更新的时间间隔，在 top 命令
  执行后，可以按以下按键对显示的结果进行排序：
  ⚫ M 根据内存使用量排序
  ⚫ P 根据 CPU 占有率排序
  ⚫ T 根据进程运行时间长短排序
  ⚫ U 根据用户名来筛选进程
  ⚫ K 输入指定的 PID 杀死进程
* 杀死进程
  kill [-signal] pid
  kill –l 列出所有信号
  kill –SIGKILL 进程ID
  kill -9 进程ID
  killall name 根据进程名杀死进程
* 进程号和进程组相关函数：
  ⚫ pid_t getpid(void);
  ⚫ pid_t getppid(void);  //父进程的pid
  ⚫ pid_t getpgid(pid_t pid);

### 2.2进程创建

##### fork()函数

* 创建子进程

    #include <sys/types.h>
    #include <unistd.h>
    
    pid_t fork(void);
        函数的作用：用于创建子进程。
        返回值：
            fork()的返回值会返回两次。一次是在父进程中，一次是在子进程中。
            在父进程中返回创建的子进程的ID,
            在子进程中返回0
            如何区分父进程和子进程：通过fork的返回值。
            在父进程中返回-1，表示创建子进程失败，并且设置errno
    
        父子进程之间的关系：
        区别：
            1.fork()函数的返回值不同
                父进程中: >0 返回的子进程的ID
                子进程中: =0
            2.pcb中的一些数据
                当前的进程的id pid
                当前的进程的父进程的id ppid
                信号集
        
        共同点：
            某些状态下：子进程刚被创建出来，还没有执行任何的写数据的操作
                - 用户区的数据
                - 文件描述符表
        
        父子进程对变量是不是共享的？
            - 刚开始的时候，是一样的，共享的。如果修改了数据，不共享了。
            - 读时共享（子进程被创建，两个进程没有做任何的写的操作），写时拷贝。
    
     实际上，更准确来说，Linux 的 fork() 使用是通过**写时拷贝 (copy- on-write)** 实现。
    写时拷贝是一种可以推迟甚至避免拷贝数据的技术。
    内核此时并不复制整个进程的地址空间，而是让父子进程共享同一个地址空间。
    只用在需要写入的时候才会复制地址空间，从而使各个进行拥有各自的地址空间。
    也就是说，资源的复制是在需要写入的时候才会进行，在此之前，只有以只读方式共享。
    注意：fork之后父子进程共享文件，
    fork产生的子进程与父进程相同的文件文件描述符指向相同的文件表，引用计数增加，共享文件偏移指针。

* 查看例子

~~~c
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(){
    int num=10;
    //创建子进程
    pid_t pid=fork();
    //判断是父进程还是子进程
    if(pid >0){
        printf("pid: %d", pid);
        //如果大于0，返回的创建的是子进程的进程号，当前是父进程
        printf("parent process,pid:%d,ppid:%d\n",getpid(),getppid());
        printf("parent num:%d\n",num);
        num+=10;
        printf("parent num +=10:%d\n",num);
    }else if(pid==0){
        printf("child process,pid:%d,ppid:%d",getpid(),getppid());
        printf("child num:%d\n",num);
        num+=100;
        printf("child num +=10:%d\n",num);
    }
    //for循环
    for(int i=0;i<3;i++){
        printf("i:%d,pid:%d\n",i,getpid());
        sleep(1);
    }
    return 0;
}
~~~

* 父进程和子进程执行的代码块分别是以下图：
* 1. 子进程只执行以下的自己进程部分
* 2. 父子进程代码段一样，但是按照pid来运行；
* 3. 写操作之后，进程资源之间互不影响，共同部分运行的是各自的资源

##### gdb调试父子进程

使用 GDB 调试的时候，GDB 默认只能跟踪一个进程，可以在 fork 函数调用之前，通过指令设置 GDB 调试工具跟踪父进程或者是跟踪子进程，默认跟踪父进程。

* 设置调试父进程或者子进程：set follow-fork-mode [parent（默认）| child]
* 设置调试模式：set detach-on-fork [on | off]
  默认为 on，表示调试当前进程的时候，其它的进程继续运行，如果为 off，调试当前进程的时候，其它进程被 GDB 挂起。
* 查看调试的进程：info inferiors
  切换当前调试的进程为id进程：inferior id
  使进程脱离 GDB 调试：detach inferiors id

##### exec函数族

◼ exec 函数族的作用是根据指定的文件名找到可执行文件，并用它来取代调用进程的内容，换句话说，就是在调用进程内部执行一个可执行文件。

◼ exec 函数族的函数执行成功后不会返回，因为调用进程的实体，包括代码段，数据段和堆栈等都已经被新的内容取代，只留下进程 ID 等一些表面上的信息仍保持原样，颇有些神似“三十六计”中的“金蝉脱壳”。看上去还是旧的躯壳，却已经注入了新的灵魂。只有调用失败了，它们才会返回 -1，从原程序的调用点接着往下执行。

注意：

1. 内核区的id等不会改变，但是用户取的数据都被替换

2. 有p或者v后缀，一般filename直接写文件名，因为p会指定到环境变量查，v指定到数组envp[]中路径依次查找，没找到返回-1
3. 一般fork新进程后，替换子进程数据

* 函数族

~~~
exec 函数族
◼ int execl(const char *path, const char *arg, .../* (char *) NULL */);
◼ int execlp(const char *file, const char *arg, ... /* (char *) NULL */);
◼ int execle(const char *path, const char *arg, .../*, (char *) NULL, char *
const envp[] */);
◼ int execv(const char *path, char *const argv[]);
◼ int execvp(const char *file, char *const argv[]);
◼ int execvpe(const char *file, char *const argv[], char *const envp[]);
◼ int execve(const char *filename, char *const argv[], char *const envp[]);
l(list) 参数地址列表，以空指针结尾
v(vector) 存有各参数地址的指针数组的地址
p(path) 按 PATH 环境变量指定的目录搜索可执行文件
e(environment) 存有环境变量字符串地址的指针数组的地址
~~~

* execl()

    

         #include <unistd.h>
        int execl(const char *path, const char *arg, ...);
            - 参数：
                - path:需要指定的执行的文件的路径或者名称
                    a.out /home/nowcoder/a.out 推荐使用绝对路径
                    ./a.out hello world   
                - arg:是执行可执行文件所需要的参数列表
                第一个参数一般没有什么作用，为了方便，一般写的是执行的程序的名称
                从第二个参数开始往后，就是程序执行所需要的的参数列表。
                参数最后需要以NULL结束（哨兵）
     
        - 返回值：
            只有当调用失败，才会有返回值，返回-1，并且设置errno
            如果调用成功，没有返回值。

* execlp

    ```c
    #include <unistd.h>
    int execlp(const char *file, const char *arg, ... );
        - 会到环境变量中查找指定的可执行文件，如果找到了就执行，找不到就执行不成功。
        - 参数：
            - file:需要执行的可执行文件的文件名
                a.out
                ps
    - arg:是执行可执行文件所需要的参数列表
            第一个参数一般没有什么作用，为了方便，一般写的是执行的程序的名称
            从第二个参数开始往后，就是程序执行所需要的的参数列表。
            参数最后需要以NULL结束（哨兵）
    
    - 返回值：
        只有当调用失败，才会有返回值，返回-1，并且设置errno
        如果调用成功，没有返回值。
    ```

* execv


```c
    int execv(const char *path, char *const argv[]);
    argv是需要的参数的一个字符串数组
    char * argv[] = {"ps", "aux", NULL};
    execv("/bin/ps", argv);

    int execve(const char *filename, char *const argv[], char *const envp[]);
    char * envp[] = {"/home/nowcoder", "/home/bbb", "/home/aaa"};
```

### 2.3 进程退出

~~~c
/*标准库
    #include <stdlib.h>
    void exit(int status);
  Linux系统（没有缓冲区，退出过不刷新缓冲区，直接退出，可能导致缓冲区的数据还没有拿出来）
    #include <unistd.h>
    void _exit(int status);

    status参数：是进程退出时的一个状态信息。父进程回收子进程资源的时候可以获取到。
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

    printf("hello\n");
    printf("world");

    // exit(0);
    _exit(0);
    
    return 0;
}
~~~

![image-20230222142604615](D:\MyTxt\typoraPhoto\image-20230222142604615.png)

##### 孤儿进程 

◼ 父进程运行结束，但子进程还在运行（未运行结束），这样的子进程就称为孤儿进程（Orphan Process）。
◼ 每当出现一个孤儿进程的时候，内核就把孤儿进程的父进程设置为 init ，而 init
进程会循环地 wait() 它的已经退出的子进程。这样，当一个孤儿进程凄凉地结束
了其生命周期的时候，init 进程就会代表党和政府出面处理它的一切善后工作。
◼ 因此孤儿进程并不会有什么危害

##### 僵尸进程

◼ 每个进程结束之后, 都会释放自己地址空间中的用户区数据，**内核区的 PCB** 没有办法自己释放掉，需要**父进程**去释放。
◼ 进程终止时，父进程尚未回收，子进程残留资源（PCB）存放于内核中，变成僵尸（Zombie）进程。
◼ 僵尸进程不能被 kill -9 杀死，这样就会导致一个问题，如果父进程不调用 wait()或 waitpid() 的话，那么保留的那段信息就不会释放，其进程号就会一直被占用，但是系统所能使用的进程号是有限的，如果大量的产生僵尸进程，将因为没有可用的进程号而导致系统不能产生新的进程，此即为僵尸进程的危害，应当避免。

![image-20230222143746621](D:\MyTxt\typoraPhoto\image-20230222143746621.png)

##### 进程回收

◼ 在每个进程退出的时候，内核释放该进程所有的资源、包括打开的文件、占用的内存等。但是仍然为其保留一定的信息，这些信息主要主要指进程控制块PCB的信息（包括进程号、退出状态、运行时间等）。
◼ 父进程可以通过调用wait或waitpid得到它的退出状态同时彻底清除掉这个进程。

###### wait()函数

* ```c
  #include <sys/types.h>
  #include <sys/wait.h>
  pid_t wait(int *wstatus);
      功能：等待任意一个子进程结束，如果任意一个子进程结束了，此函数会回收子进程的资源。
      参数：int *wstatus
          进程退出时的状态信息，传入的是一个int类型的地址，传出参数。
          //wstatus指针传入到宏里面可得到 退出的具体信息
          //如果不需要查看直接 wait(NULL)
      返回值：
          - 成功：返回被回收的子进程的id
          - 失败：-1 (所有的子进程都结束，调用函数失败)
  
  调用wait函数的进程会被挂起（阻塞），直到它的一个子进程退出或者收到一个不能被忽略的信号时才被唤醒（相当于继续往下执行）
  如果没有子进程了，函数立刻返回，返回-1；如果子进程都已经结束了，也会立即返回，返回-1.
  ```

◼ wait() 和 waitpid() 函数的功能一样，区别在于：
**wait() 函数会阻塞，waitpid() 可以设置不阻塞，waitpid() 还可以指定等待哪个子进程结束。**
◼ 注意：一次wait或waitpid调用**只能清理一个子进程**，清理多个子进程应使用循环：while()

* 退出信息相关宏函数查询

* ~~~c
  ◼ WIFEXITED(status) 非0，进程正常退出
  ◼ WEXITSTATUS(status) 如果上宏为真，获取进程退出的状态（exit的参数）
  ◼ WIFSIGNALED(status) 非0，进程异常终止
  ◼ WTERMSIG(status) 如果上宏为真，获取使进程终止的信号编号
  ◼ WIFSTOPPED(status) 非0，进程处于暂停状态
  ◼ WSTOPSIG(status) 如果上宏为真，获取使进程暂停的信号的编号
  ◼ WIFCONTINUED(status) 非0，进程暂停后已经继续运行
  ~~~

  例子;

* ~~~c
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  
  int main(){
  
      pid_t pid;
      //创建5个子进程 
      for(int i=0;i!=5;++i){
          pid = fork();
          if(pid == 0){
              break;
          }
      }
      if(pid > 0){
          //父进程
          while(1){
              printf("父进程: %d\n",getpid());
              int st;
              int ret = wait(&st);
              if(ret == -1){
                  break;
              }
              if(WIFEXITED(st)){
                  //是不是正常退出
                  printf("退出状态码: %d\n",WEXITSTATUS(st));
              }
              if(WIFSIGNALED(st)){
                  //是不是被异常终止
                  printf("被哪个信号干掉: %d\n",WEXITSTATUS(st));
              }
              printf("child die,pid = %d\n",WTERMSIG(st));
              sleep(1);
          }
      }else if(pid == 0){
          //子进程
          printf("子进程: %d\n",getpid());
          exit(0);
          sleep(1);        
      }
      return 0;
  }
  ~~~

###### waitpid()函数

~~~c
    #include <sys/types.h>
    #include <sys/wait.h>
    pid_t waitpid(pid_t pid, int *wstatus, int options);
        功能：回收指定进程号的子进程，可以设置是否阻塞。
        参数：
            - pid:
                pid > 0 : 某个子进程的pid
                pid = 0 : 回收当前进程组的所有子进程    
                pid = -1 : 回收任意的子进程，相当于 wait()  （最常用）
                pid < -1 : 某个进程组的组id的绝对值，回收指定进程组中的子进程
            - 参数：int *wstatus
                     进程退出时的状态信息，传入的是一个int类型的地址，传出参数。
            - options：设置阻塞或者非阻塞
                0 : 阻塞
                WNOHANG : 非阻塞
            - 返回值：
 //这三个会返回值可以判断当前进程的子进程回收情况，可以以此来设计，很多子进程时，回收处理函数
                > 0 : 返回子进程的id
                = 0 : options=WNOHANG, 表示还有子进程活着 
                = -1 ：错误，或者没有子进程了
~~~

* 例子(设置非阻塞)：

* ~~~c
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  
  int main(){
  
      pid_t pid;
      //创建5个子进程
      for(int i=0;i!=5;++i){
          pid = fork();
          if(pid == 0){
              break;
          }
      }
      if(pid > 0){
          //父进程
          while(1){
              printf("父进程: %d\n",getpid());
              sleep(1);
              
              int st;
              //int ret = waitpid(-1,&st,0);
              int ret = waitpid(-1,&st,WNOHANG);
              
              if(ret == -1){
                  break;
              }else if(ret == 0){
                  //子进程还有在运行的
                  continue;
              }else if(ret > 0){
                  if(WIFEXITED(st)){
                      printf("退出的状态码：%d\n",WEXITSTATUS(st));
                  }
                  if(WIFSIGNALED(st)){
                      printf("被哪个信号干掉：%d",WTERMSIG(st));
                  }
              }
              printf("child die,pid = %d\n",WTERMSIG(st));
              sleep(1);
          }
      }else if(pid == 0){
          //子进程
          while (1)
          {
              printf("子进程: %d\n",getpid());
              sleep(1);
          }
          exit(0);
      }
      return 0;
  }
  ~~~

### 2.4进程间通信IPC



管道在使用完毕后应该被关闭，这是为了防止出现资源泄漏的情况。如果没有关闭管道，在程序运行过程中会一直占用系统资源，而且可能导致其他进程无法使用同名管道。同时，如果管道没有被及时关闭，在程序意外退出时也可能导致数据丢失或者磁盘空间占用问题。因此，在使用完毕后，最好及时关闭管道。

* TCP/IP方式： mysql -h127.0.0.1 -uroot -P3306 -p
* windowOS 的一台主机：命名管道和共享内存
* 类Unix的一台主机：套接字

<img src="D:\MyTxt\typoraPhoto\image-20230222170747041.png" alt="image-20230222170747041" style="zoom:50%;" />

* （匿名）管道，Unix系统最古老的通信方式，所有Unix系统都支持
* 统计目录中文件数量的命令： ls | wc -l
   实际上就是创建管道 把写入端导出读取端
* <img src="D:\MyTxt\typoraPhoto\image-20230222172058889.png" alt="image-20230222172058889" style="zoom:50%;" />

##### 管道特点

1. 管道其实是内核在内存中维护的缓冲区，缓冲能力有限，不同操作系统大小不同；
2. 管道拥有文件的特质：读操作、写操作，匿名管道没有文件实体，有名管道有文件实体，但是不存储数据，可以按照操作文件的方式操作管道。
3. 一个管道是一个字节流，使用管道不存在消息和消息边界的概念，从管道读取数据的进程可以读取任意大小的数据块，而不管写入进程写入管道的数据块是多少（每次都按照自己能力读取）
4. 通过管道传递是顺序的。
5. 管道中传递数据方向是单向的，一端写入，一端读取，也就是半双工。
6. 从管道中读取数据是一次性操作，数据一旦被读走了，就从管道抛弃了，(队列实现)释放空间以便写入更多的数据，在管道中无法使用lseek()随机访问数据
7. 管道只能在具有公共祖先的进程（父子进程，兄弟进程，具有亲缘关系）之间使用

<img src="D:\MyTxt\typoraPhoto\image-20230222173206683.png" alt="image-20230222173206683" style="zoom: 50%;" />

* 管道原理：子进程forkc()出来之后，可以共享文件描述符，可以操作建立管道

<img src="D:\MyTxt\typoraPhoto\image-20230222173922960.png" alt="image-20230222173922960" style="zoom:33%;" />

* 管道数据结构：环形队列

<img src="D:\MyTxt\typoraPhoto\image-20230222174026442.png" alt="image-20230222174026442" style="zoom: 50%;" />

##### 匿名管道传输pipe()函数

~~~c
    #include <unistd.h>
    int pipe(int pipefd[2]);
        功能：创建一个匿名管道，用来进程间通信。
        参数：int pipefd[2] 这个数组是一个传出参数。
            pipefd[0] 对应的是管道的读端
            pipefd[1] 对应的是管道的写端
        返回值：
            成功 0
            失败 -1

    管道默认是阻塞的：如果管道中没有数据，read阻塞，如果管道满了，write阻塞

    注意：匿名管道只能用于具有关系的进程之间的通信（父子进程，兄弟进程）
~~~

* 例子-父子间管道通信
* 匿名管道一般不会实现双向读写，因为没有了sleep()，本进程会抢读管道，一般就是一个读，一个写，并且关闭另一个操作，俺理解为互斥

~~~c
    //在fork()之前发送数据给父进程，父进程读取到数据输出
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    //在fork()之前创建管道
    int pipefd[2];
    int ret=pipe(pipefd);
    if(ret==-1){
        perror("pipe");
        exit(0);
    }
    //创建子进程
    pid_t pid = fork();
    if(pid>0){
        //父进程先读取，然后写
        printf("父进程");
        char buf[1024]={0};
        while(1){
            //先读
            //sizeof给出操作数的存储地址空间
            int len = read(pipefd[0],buf,sizeof(buf));
            printf("parent recv: %s ,pid : %d\n",buf,getpid());

            //向管道写入数据
            char* str="hello,this is parent process";
            write(pipefd[1],str,strlen(str));   
            sleep(1);             
        }
    }else if(pid ==0){
        //子进程先写，然后读
        //管道默认阻塞，sleep(10); 如果没有数据，读取端就等着
        char buf[1024]={0};
        printf("子进程");
        while(1){
            //先写
            char* str="hello,this is child process";
            //strlen()函数给出字符串的长度
            write(pipefd[1],str,strlen(str));   
            sleep(1);

            //读取数据
            int len = read(pipefd[0],buf,sizeof(buf));
            printf("child recv: %s ,pid : %d\n",buf,getpid());
            //清除一下
            bzero(buff,1024);
        }
    }
    return 0;
}
~~~

* 互斥读写操作的代码如下

* ~~~c
      //在fork()之前发送数据给父进程，父进程读取到数据输出
  #include <unistd.h>
  #include <sys/types.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  
  int main(){
      //在fork()之前创建管道
      int pipefd[2];
      int ret=pipe(pipefd);
      if(ret==-1){
          perror("pipe");
          exit(0);
      }
      //创建子进程
      pid_t pid = fork();
      if(pid>0){
          //父进程读取
          printf("父进程\n");
  
          //关闭写端
          close(pipefd[1]);
  
          char buf[1024]={0};
          while(1){
              int len = read(pipefd[0],buf,sizeof(buf));
              printf("parent recv: %s ,pid : %d\n",buf,getpid());           
          }
      }else if(pid ==0){
          //子进程写端
          printf("子进程\n");
  
          //关闭读端
          close(pipefd[0]);
  
          while(1){
              char* str="hello,this is child process";
              //strlen()函数给出字符串的长度
              write(pipefd[1],str,strlen(str));   
              sleep(1);                  
          }
          
      }
      return 0;
  }
  ~~~

* 获取管道大小

*     // 1. 函数获取管道的大小
      long size = fpathconf(pipefd[0], _PC_PIPE_BUF);
      // 2. 终端获取管道信息
      ulimit -a

##### ps aux | grep xxx 操作实现 -父子进程通信案例

~~~c
    //在fork()之前发送数据给父进程，父进程读取到数据输出
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

int main(){
    //创建管道
    int fd[2];
    int ret=pipe(fd);
    if(ret==-1){
        perror("pipe");
        exit(0);
    }  
    //创建子进程
    pid_t pid = fork();
    if(pid > 0){
    //父进程，读取管道信息输出到列表
        close(fd[1]);
        
        char buf[1024]={0};
        int len=-1;
        //空间设置 -1 留一个字符串的结束符位置
        while((len=read(fd[0],buf,sizeof(buf)-1))>0){
            //过滤数据输出
            printf("%s",buf);
            //清空buf内容
            memset(buf,0,1024);
        }
        wait(NULL);

    }else if(pid == 0){
    //子进程，把输出stdout_fileno的内容写入管道
        close(fd[0]);
        // 文件描述符的重定向 stdout_fileno -> fd[1]
        dup2(fd[1],STDOUT_FILENO);
        //替换到aux的内容
        execlp("ps","ps","aux",NULL);
        //如果没退出，报系统错误
        perror("execlp");
        exit(0);   
    }

    return 0;
}
~~~

##### 管道读写的特点

~~~c
管道的读写特点：
使用管道时，需要注意以下几种特殊的情况（假设都是阻塞I/O操作）
1.所有的指向管道写端的文件描述符都关闭了（管道写端引用计数为0），有进程从管道的读端
读数据，那么管道中剩余的数据被读取以后，再次read会返回0，就像读到文件末尾一样。

2.如果有指向管道写端的文件描述符没有关闭（管道的写端引用计数大于0），而持有管道写端的进程
也没有往管道中写数据，这个时候有进程从管道中读取数据，那么管道中剩余的数据被读取后，
再次read会阻塞，直到管道中有数据可以读了才读取数据并返回。

3.如果所有指向管道读端的文件描述符都关闭了（管道的读端引用计数为0），这个时候有进程
向管道中写数据，那么该进程会收到一个信号SIGPIPE, 通常会导致进程异常终止。

4.如果有指向管道读端的文件描述符没有关闭（管道的读端引用计数大于0），而持有管道读端的进程
也没有从管道中读数据，这时有进程向管道中写数据，那么在管道被写满的时候再次write会阻塞，
直到管道中有空位置才能再次写入数据并返回。

总结：
    读管道：
        管道中有数据，read返回实际读到的字节数。
        管道中无数据：
            写端被全部关闭，read返回0（相当于读到文件的末尾）
            写端没有完全关闭，read阻塞等待

    写管道：
        管道读端全部被关闭，进程异常终止（进程收到SIGPIPE信号）
        管道读端没有全部关闭：
            管道已满，write阻塞
            管道没有满，write将数据写入，并返回实际写入的字节数
~~~

* 设置非阻塞 - 此时设置为管道读端继续read，由于没有写入，read()返回-1

~~~c
        int flags = fcntl(pipefd[0], F_GETFL);  // 获取原来的flag
        flags |= O_NONBLOCK;            // 修改flag的值
        fcntl(pipefd[0], F_SETFL, flags);   // 设置新的flag
~~~

#### 有名管道

◼ 匿名管道，由于没有名字，只能用于亲缘关系的进程间通信。为了克服这个缺点，提 出了有名管道（FIFO），也叫命名管道、FIFO文件。 

有名管道（FIFO）不同于匿名管道之处在于它提供了一个路径名与之关联，以 FIFO 的文件形式存在于文件系统中，并且其打开方式与打开一个普通文件是一样的，这样 即使与 FIFO 的创建进程不存在亲缘关系的进程，只要可以访问该路径，就能够彼此 通过 FIFO 相互通信，因此，通过 FIFO 不相关的进程也能交换数据。 ◼ 

一旦打开了 FIFO，就能在它上面使用与操作匿名管道和其他文件的系统调用一样的 I/O系统调用了（如read()、write()和close()）。与管道一样，FIFO 也有一 个写入端和读取端，并且从管道中读取数据的顺序与写入的顺序是一样的。FIFO 的 名称也由此而来：先入先出。

##### 与匿名管道区别：

有名管道（FIFO)和匿名管道（pipe）有一些特点是相同的，不一样的地方在于： 1. FIFO 在文件系统中作为一个特殊文件存在，但 FIFO 中的内容却存放在内存中。 2. 当使用 FIFO 的进程退出后，FIFO 文件将继续保存在文件系统中以便以后使用。 3. FIFO 有名字，不相关的进程可以通过打开有名管道进行通信。

1. 通过命令创建有名管道 mkfifo 名字

2. 通过函数创建有名管道 
    int mkfifo(const char *pathname, mode_t mode); 

    ~~~c
    创建fifo文件
        1.通过命令： mkfifo 名字
        2.通过函数：int mkfifo(const char *pathname, mode_t mode);
    
        #include <sys/types.h>
        #include <sys/stat.h>
        int mkfifo(const char *pathname, mode_t mode);
            参数：
                - pathname: 管道名称的路径
                - mode: 文件的权限 和 open 的 mode 是一样的
                        是一个八进制的数
            返回值：成功返回0，失败返回-1，并设置错误号
    ~~~

3. 一旦使用 mkfifo 创建了一个 FIFO，就可以使用 **open** 打开它，常见的文件 I/O 函数都可用于 fifo。如：close、read、write、unlink 等。 

4. FIFO 严格遵循先进先出（First in First out），对管道及 FIFO 的读总是 从开始处返回数据，对它们的写则把数据添加到末尾。它们不支持诸如 lseek() 等文件定位操作。

##### 有名管道注意事项

~~~c
    有名管道的注意事项：
        1.一个为只读而打开一个管道的进程会阻塞，直到另外一个进程为只写打开管道
        2.一个为只写而打开一个管道的进程会阻塞，直到另外一个进程为只读打开管道

    读管道：
        管道中有数据，read返回实际读到的字节数
        管道中无数据：
            管道写端被全部关闭，read返回0，（相当于读到文件末尾）
            写端没有全部被关闭，read阻塞等待
    
    写管道：
        管道读端被全部关闭，进行异常终止（收到一个SIGPIPE信号）
        管道读端没有全部关闭：
            管道已经满了，write会阻塞
            管道没有满，write将数据写入，并返回实际写入的字节数。
~~~

![image-20230223161443567](D:\MyTxt\typoraPhoto\image-20230223161443567.png)

##### 同一路径下 有名管道创建并用于读写的例子

~~~c
write.c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {

    // 1.判断文件是否存在
    int ret = access("fifo1", F_OK);
    if(ret == -1) {
        printf("管道不存在，创建管道\n");
        //2.创建管道文件
        ret = mkfifo("fifo1", 0664);

        if(ret == -1) {
            perror("mkfifo");
            exit(0);
        }       

    }
    
    //以只写的方式打开管道
    int fd=open("fifo1",O_WRONLY);
    if(fd==-1){
        perror("open");
        exit(0);
    }

    //写入数据
    for (int i = 0; i < 100; i++)
    {
        char buf[1024];
        //sprintf将字符串写入buf中
        sprintf(buf,"hello,%d\n",i);
        printf("write data: %s\n",buf);
        write(fd,buf,strlen(buf));
        sleep(1);
    }

    close(fd);

    return 0;
}
~~~

```c
read.c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
     //1. 打开管道文件
     int fd = open("fifo1",O_RDONLY);
     if(fd == -1){
        perror("open");
        exit(0);
     }

     //读数据
     while(1){
        char buf[1024] = {0};
        int len = read(fd,buf,sizeof(buf));
        if(len == 0){
            printf("写端断开连接了。\n");
            break;
        }
        printf("recv buf :%s\n",buf);
     }
    
    close(fd);
    return 0;
}
```

### 2.5 内存映射

Memory-mapped I/O 是将磁盘文件的数据映射到内存，用户通过修改内存就可以修改磁盘文件

<img src="D:\MyTxt\typoraPhoto\image-20230224145114660.png" alt="image-20230224145114660" style="zoom:50%;" />

~~~c
    #include <sys/mman.h>
    void *mmap(void *addr, size_t length, int prot, int flags,int fd, off_t offset);
        - 功能：将一个文件或者设备的数据映射到内存中
        - 参数：
            - void *addr: NULL, 由内核指定
            - length : 要映射的数据的长度，这个值不能为0。建议使用文件的长度。
                    获取文件的长度：stat lseek
            - prot : 对申请的内存映射区的操作权限
                -PROT_EXEC ：可执行的权限
                -PROT_READ ：读权限
                -PROT_WRITE ：写权限
                -PROT_NONE ：没有权限
                要操作映射内存，必须要有读的权限。
                PROT_READ、PROT_READ|PROT_WRITE
            - flags :
                - MAP_SHARED : //映射区的数据会自动和磁盘文件进行同步，进程间通信，必须要设置这个选项
                - MAP_PRIVATE ：//不同步，内存映射区的数据改变了，对原来的文件不会修改，会重新创建一个新的文件。（copy on write）,可以用内存映射来存输入，之后用于写入
            - fd: 需要映射的那个文件的文件描述符
                - 通过open得到，open的是一个磁盘文件
                - 注意：文件的大小不能为0，open指定的权限不能和prot参数有冲突，也就是不小于需要的权限。
                    prot: PROT_READ                open:只读/读写 
                    prot: PROT_READ | PROT_WRITE   open:读写
            - offset：偏移量，一般不用。必须指定的是4k的整数倍，0表示不偏移。
        - 返回值：返回创建的内存的首地址
            失败返回MAP_FAILED，(void *) -1

    int munmap(void *addr, size_t length);
        - 功能：释放内存映射
        - 参数：
            - addr : 要释放的内存的首地址
            - length : 要释放的内存的大小，要和mmap函数中的length参数的值一样
~~~

##### 使用内存映射实现进程间通信：

~~~
    使用内存映射实现进程间通信：
    1.有关系的进程（父子进程）
        - 还没有子进程的时候
            - 通过唯一的父进程，先创建内存映射区
        - 有了内存映射区以后，创建子进程
        - 父子进程共享创建的内存映射区（类似于匿名管道）
    
    2.没有关系的进程间通信
        - 准备一个大小不是0的磁盘文件
        - 进程1 通过磁盘文件创建内存映射区
            - 得到一个操作这块内存的指针
        - 进程2 通过磁盘文件创建内存映射区
            - 得到一个操作这块内存的指针
        - 使用内存映射区通信（也就是映射同一块文件）		

    注意：内存映射区通信，是非阻塞。
~~~

###### 关系类进程通信

~~~c
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wait.h>   
#include <string.h>
#include <unistd.h>

int main(){
    //打开文件
    int fd = open("a.txt",O_RDWR);
    if(fd == -1){
        perror("open");
        exit(0);
    }
    int size = lseek(fd,0,SEEK_END);
    //创建内存映射区    
    void *ptr = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(ptr == MAP_FAILED){
        perror("mmap");
        exit(0);
    }
    //创建子进程，写入，父进程接受
    pid_t pid = fork();
    if(pid > 0){
        //父进程读取
        wait(NULL);
        char buf[64];
        strcpy(buf,(char*)ptr);//相当于 把指针指向的内存地址的内容，写到buf中
        printf("read data: %s",buf);

    }else if(pid == 0){
        //子进程写入
        strcpy((char*)ptr,"我是儿子，你好");//相当于往指针指向内存的位置写入这个字符串
    }
    //关闭内存映射区
    munmap(ptr,size);
    return 0;
}
~~~

###### 非关系类进程通信

~~~c
write.c
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wait.h>   
#include <string.h>
#include <unistd.h>

int main(){
    //打开文件
    int fd = open("a.txt",O_RDWR);
    if(fd == -1){
        perror("open");
        exit(0);
    }
    int size = lseek(fd,0,SEEK_END);

    //进程写入

        //创建内存映射区    
        void *ptr = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
        if(ptr == MAP_FAILED){
            perror("mmap");
            exit(0);
        }

        strcpy((char*)ptr,"好哥们 你好\n");       
        
        //关闭内存映射区
        munmap(ptr,size);        
    

    return 0;
}
~~~

~~~c
read.c
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wait.h>   
#include <string.h>
#include <unistd.h>

int main(){
    //打开文件
    int fd = open("a.txt",O_RDWR);
    if(fd == -1){
        perror("open");
        exit(0);
    }
    int size = lseek(fd,0,SEEK_END);

    //进程读取

        //创建内存映射区    
        void *ptr = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
        if(ptr == MAP_FAILED){
            perror("mmap");
            exit(0);
        }

        char buf[1024];
        strcpy(buf,(char*)ptr);
        printf("read data: %s",buf);
        
        //关闭内存映射区
        munmap(ptr,size);        

    return 0;
}
~~~

##### 内存映射注意事项

1. 如果对mmap的返回值(ptr)做++操作(ptr++), munmap是否能够成功?
   void * ptr = mmap(...);
   ptr++;  可以对其进行++操作
   munmap(ptr, len);   // 错误,要保存地址

2. 如果open时O_RDONLY, mmap时prot参数指定PROT_READ | PROT_WRITE会怎样?
   错误，返回MAP_FAILED
   open()函数中的权限建议和prot参数的权限保持一致。

3. 如果文件偏移量为1000会怎样?
   偏移量必须是4K的整数倍，返回MAP_FAILED

4. mmap什么情况下会调用失败?
       - 第二个参数：length = 0
           - 第三个参数：prot
           - 只指定了写权限
           - prot PROT_READ | PROT_WRITE
             第5个参数fd 通过open函数时指定的 O_RDONLY / O_WRONLY

5. 可以open的时候O_CREAT一个新文件来创建映射区吗?
       - 可以的，但是创建的文件的大小如果为0的话，肯定不行
           - 可以对新的文件进行扩展
           - lseek()
           - truncate()

6. mmap后关闭文件描述符，对mmap映射有没有影响？
       int fd = open("XXX");
       mmap(,,,,fd,0);
       close(fd); 
       映射区还存在，创建映射区的fd被关闭，没有任何影响。

7. 对ptr越界操作会怎样？
   void * ptr = mmap(NULL, 100,,,,,);
   4K
   越界操作操作的是非法的内存 -> 段错误

​       void *memcpy(void *dest, const void *src, size_t n);

#### 使用内存映射实现文件的拷贝

* 使用内存映射实现文件拷贝的功能
    思路：
        1.对原始的文件进行内存映射
        2.创建一个新文件（拓展该文件）
        3.把新文件的数据映射到内存中
        4.通过内存拷贝将第一个文件的内存数据拷贝到新的文件内存中
        5.释放资源

```c
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main() {

    // 1.对原始的文件进行内存映射
    int fd = open("english.txt", O_RDWR);
    if(fd == -1) {
        perror("open");
        exit(0);
    }

    // 获取原始文件的大小
    int len = lseek(fd, 0, SEEK_END);

    // 2.创建一个新文件（拓展该文件）
    int fd1 = open("cpy.txt", O_RDWR | O_CREAT, 0664);
    if(fd1 == -1) {
        perror("open");
        exit(0);
    }
    
    // 对新创建的文件进行拓展
    truncate("cpy.txt", len);
    write(fd1, " ", 1);

    // 3.分别做内存映射
    void * ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    void * ptr1 = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

    if(ptr == MAP_FAILED) {
        perror("mmap");
        exit(0);
    }

    if(ptr1 == MAP_FAILED) {
        perror("mmap");
        exit(0);
    }

    // 内存拷贝 memcpy内存拷贝函数 在man 3 中
    memcpy(ptr1, ptr, len);
    
    // 释放资源
    munmap(ptr1, len);
    munmap(ptr, len);

    close(fd1);
    close(fd);

    return 0;
}
```

##### 匿名内存映射

~~~c
/*
    匿名映射：不需要文件实体进程一个内存映射
*/

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {

    // 1.创建匿名内存映射区
    int len = 4096;
    //此处添加ANONYMOUS，表示映射没有任何文件支持，也就是匿名内存映射不需要任何文件支持，所以下一个文件描述符参数作为-1
    void * ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(ptr == MAP_FAILED) {
        perror("mmap");
        exit(0);
    }

    // 父子进程间通信
    pid_t pid = fork();

    if(pid > 0) {
        // 父进程
        strcpy((char *) ptr, "hello, world");
        wait(NULL);
    }else if(pid == 0) {
        // 子进程
        sleep(1);
        printf("%s\n", (char *)ptr);
    }

    // 释放内存映射区
    int ret = munmap(ptr, len);

    if(ret == -1) {
        perror("munmap");
        exit(0);
    }
    return 0;
}
~~~

### 2.6 信号

##### 概述

* 概述 : 信号是 Linux 进程间通信的最古老的方式之一，是事件发生时对进程的通知机制，有时也
  称之为软件中断，它是在软件层次上对中断机制的一种模拟，是一种异步通信的方式。信号
  可以导致一个正在运行的进程被另一个正在运行的异步进程中断，转而处理某一个突发事件。
* 发往进程的诸多信号，通常都是源于内核。引发内核为进程产生信号的各类事件如下：
  1. 对于前台进程，用户可以通过输入特殊的终端字符来给它发送信号。比如输入Ctrl+C
     通常会给进程发送一个中断信号。
  2. 硬件发生异常，即硬件检测到一个错误条件并通知内核，随即再由内核发送相应信号给
     相关进程。比如执行一条异常的机器语言指令，诸如被 0 除，或者引用了无法访问的
     内存区域。
  3. 系统状态变化，比如 alarm 定时器到期将引起 SIGALRM 信号，进程执行的 CPU
     时间超限，或者该进程的某个子进程退出。
  4.  运行 kill 命令或调用 kill 函数。

* 使用信号的两个主要目的是：

  1. 让进程知道已经发生了一个特定的事情。
  2. 强迫进程执行 它自己代码中的信号处理程序。

* 信号的特点：

  1. 简单
  2. 不能携带大量信息
  3.  满足某个特定条件才发送
  4.  优先级比较高

* ~~~shell
  1. 查看系统定义的信号列表：kill –l
  前 31 个信号为常规信号，其余为实时信号
  
  2. ◼ 查看信号的详细信息：man 7 signal
  ◼ 信号的 5 中默认处理动作
   Term 终止进程
   Ign 当前进程忽略掉这个信号
   Core 终止进程，并生成一个Core文件(Core文件包含了一些关于异常的信息)
   Stop 暂停当前进程
   Cont 继续执行当前被暂停的进程
  
  3. 信号的几种状态：产生、未决、递达
  4. SIGKILL 和 SIGSTOP 信号不能被捕捉、阻塞或者忽略，只能执行默认动作。
  ~~~

  <img src="D:\MyTxt\typoraPhoto\image-20230225153502429.png" alt="image-20230225153502429" style="zoom:50%;" />

  <img src="D:\MyTxt\typoraPhoto\image-20230225153518112.png" alt="image-20230225153518112" style="zoom:50%;" />

  <img src="D:\MyTxt\typoraPhoto\image-20230225153533345.png" alt="image-20230225153533345" style="zoom:50%;" />

  <img src="D:\MyTxt\typoraPhoto\image-20230225153713035.png" alt="image-20230225153713035" style="zoom:50%;" />

  * 这里注意kill 9不能杀死僵尸进程
  * 可以用kill -l查看信号对应的编号

##### kill() raise() abort()

```c
#include <sys/types.h>
#include <signal.h>

int kill(pid_t pid, int sig);
    - 功能：给任何的进程或者进程组pid, 发送任何的信号 sig
    - 参数：
        - pid ：
            > 0 : 将信号发送给指定的进程
            = 0 : 将信号发送给当前的进程组
            = -1 : 将信号发送给每一个有权限接收这个信号的进程
            < -1 : 这个pid=某个进程组的ID取反 （-12345）
        - sig : 需要发送的信号的编号或者是宏值，0表示不发送任何信号

    kill(getppid(), 9);
    kill(getpid(), 9);
    
int raise(int sig);
    - 功能：给当前进程或者线程发送信号
    - 参数：
        - sig : 要发送的信号
    - 返回值：
        - 成功 0
        - 失败 非0
    kill(getpid(), sig);   

void abort(void);
    - 功能： 发送SIGABRT信号给当前的进程，杀死当前进程
    类似于: kill(getpid(), SIGABRT);
```

例子：

```c
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int main() {

    pid_t pid = fork();

    if(pid == 0) {
        // 子进程
        int i = 0;
        for(i = 0; i < 5; i++) {
            printf("child process\n");
            sleep(1);
        }

    } else if(pid > 0) {
        // 父进程
        printf("parent process\n");
        sleep(2);
        printf("kill child process now\n");//这里传的是子进程pid
        kill(pid, SIGINT);
    }

    return 0;
}
```



##### alarm() 

```c
#include <unistd.h>
unsigned int alarm(unsigned int seconds);
    - 功能：设置定时器（闹钟）。函数调用，开始倒计时，当倒计时为0的时候，
            函数会给当前的进程发送一个信号：SIGALARM，（查表，会终止进程）
    - 参数：
        seconds: 倒计时的时长，单位：秒。如果参数为0，定时器无效（不进行倒计时，不发信号）。
                取消一个定时器，通过alarm(0)。
    - 返回值：
        - 之前没有定时器，返回0
        - 之前有定时器，返回之前的定时器剩余的时间

- SIGALARM ：默认终止当前的进程，每一个进程都有且只有唯一的一个定时器。
    alarm(10);  -> 返回0
    过了1秒
    alarm(5);   -> 返回9

alarm(100) -> 该函数是不阻塞的
```

* 例子

```c
#include <stdio.h>
#include <unistd.h>

//查看1秒计算机能数多少个数？
/*
    实际时间 = 内核时间 + 用户时间 + 消耗时间
    进行文件的IO操作比较消耗时间

    定时器，与进程的状态无关，alarm都会计时
*/
int main(){
    alarm(1);

    int i=0;
    while(1){
        printf("%d\n",i++);
    }

    return 0;
}
```

##### setitimer()

```c
    #include <sys/time.h>
    int setitimer(int which, const struct itimerval *new_value,
                        struct itimerval *old_value);
   - 功能：设置定时器（闹钟）。可以替代alarm函数。精度微妙us，可以实现周期性定时
    - 参数：
        - which : 定时器以什么时间计时
          ITIMER_REAL: 真实时间，时间到达，发送 SIGALRM   常用
          ITIMER_VIRTUAL: 用户时间，时间到达，发送 SIGVTALRM
          ITIMER_PROF: 以该进程在用户态和内核态下所消耗的时间来计算，时间到达，发送 SIGPROF

        - new_value: 设置定时器的属性
        
            struct itimerval {      // 定时器的结构体
            struct timeval it_interval;  // 每个阶段的时间，间隔时间
            struct timeval it_value;     // 延迟多长时间执行定时器
            };

            struct timeval {        // 时间的结构体
                time_t      tv_sec;     //  秒数     
                suseconds_t tv_usec;    //  微秒    
            };

        过10秒后，每个2秒定时一次
       
        - old_value ：记录上一次的定时的时间参数，一般不使用，指定NULL
    
    - 返回值：
        成功 0
        失败 -1 并设置错误号
```

##### signal 捕捉函数

```c
    #include <signal.h>
    typedef void (*sighandler_t)(int);
    sighandler_t signal(int signum, sighandler_t handler);
        - 功能：设置某个信号的捕捉行为
        - 参数：
            - signum: 要捕捉的信号
            - handler: 捕捉到信号要如何处理
                - SIG_IGN ： 忽略信号
                - SIG_DFL ： 使用信号默认的行为
                - 回调函数 :  这个函数是内核调用，程序员只负责写，捕捉到信号后如何去处理信号。
                回调函数：
                    - 需要程序员实现，提前准备好的，函数的类型根据实际需求，看函数指针的定义
                    - 不是程序员调用，而是当信号产生，由内核调用
                    - 函数指针是实现回调的手段，函数实现之后，将函数名放到函数指针的位置就可以了。

        - 返回值：
            成功，返回上一次注册的信号处理函数的地址。第一次调用返回NULL
            失败，返回SIG_ERR，设置错误号
            
    SIGKILL SIGSTOP不能被捕捉，不能被忽略。
```

* 例子： 用setitimer配合signal捕捉

```c
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>

//设置回调函数
void myAlarm(int num){
    //这里可以查表得到信号14是SIGALRM
    printf("捕捉到了信号编号是: %d\n",num);
    printf("xxxxxxxxxxxx\n");
}

int main(){

  // 注册信号捕捉
    // signal(SIGALRM, SIG_IGN);
    // signal(SIGALRM, SIG_DFL);
    // void (*sighandler_t)(int); 函数指针，int类型的参数表示捕捉到的信号的值。
    signal(SIGALRM,myAlarm);

    //设置间隔时间
    struct itimerval new_value;
    //时间变量必须要初始化
    new_value.it_interval.tv_sec = 2;
    new_value.it_interval.tv_usec = 0;

    //延迟时间,到3s后开始计时
    new_value.it_value.tv_sec = 3;
    new_value.it_value.tv_usec = 0;

     //是非阻塞的   
    int ret = setitimer(ITIMER_REAL,&new_value,NULL);
    printf("开始计时\n");
    if(ret == -1){
        perror("setitimer");
        exit(0);
    }
	//保存程序不关闭 阻塞
    getchar();

    return 0;
}
```

### 2.7信号集

* 许多信号相关的系统调用都需要能表示一组不同的信号，多个信号可使用一个称之为信号集的数据结构来表示，其系统数据类型为 sigset_t。
* 在 PCB 中有两个非常重要的信号集。一个称之为 “**阻塞信号集**”（**信号掩码（也就是阻塞信号集）**可以设置，比如empty和add） ，另一个称之为“**未决信号集**” （只读）。这两个信号集都是内核使用位图机制来实现的。但操作系统不允许我们直接对这两个信号集进行位操作。而需自定义另外一个集合，借助信号集操作函数来对 PCB 中的这两个信号集进行修改。
* 信号的 “未决” 是一种**状态**，指的是从**信号的产生到信号被处理前**的这一段时间。
* 信号的 “阻塞” 是一个**开关动作**，指的是**阻止信号被处理**，但不是阻止信号产生。
* 信号的**阻塞就是让系统暂时保留信号留待以后发送**。由于另外有办法让系统忽略信号，所以一般情况下信号的阻塞只是暂时的，只是为了防止信号打断敏感的操作。
* 下图，进程的虚拟地址空间，包括用户区和内核区，内核区中有PCB进程控制块，PCB中有未决信号集和阻塞信号集 

<img src="D:\MyTxt\typoraPhoto\image-20230226152103733.png" alt="image-20230226152103733" style="zoom:50%;" />

~~~shell
1.用户通过键盘  Ctrl + C, 产生2号信号SIGINT (信号被创建)

2.信号产生但是没有被处理 （未决）
    - 在内核中将所有的没有被处理的信号存储在一个集合中 （未决信号集）
    - SIGINT信号状态被存储在第二个标志位上
        - 这个标志位的值为0， 说明信号不是未决状态
        - 这个标志位的值为1， 说明信号处于未决状态
    
3.这个未决状态的信号，需要被处理，处理之前需要和另一个信号集（阻塞信号集），进行比较
    - 阻塞信号集默认不阻塞任何的信号
    - 如果想要阻塞某些信号需要用户调用系统的API

4.在处理的时候和阻塞信号集中的标志位进行查询，看是不是对该信号设置阻塞了
    - 如果没有阻塞，这个信号就被处理
    - 如果阻塞了，这个信号就继续处于未决状态，直到阻塞解除，这个信号就被处理
~~~

##### 信号集函数

```c
以下信号集相关的函数都是对     自定义的信号集!!     进行操作。

int sigemptyset(sigset_t *set);
    - 功能：清空信号集中的数据,将信号集中的所有的标志位置为0
    - 参数：set,传出参数，需要操作的信号集
    - 返回值：成功返回0， 失败返回-1

int sigfillset(sigset_t *set);
    - 功能：将信号集中的所有的标志位置为1
    - 参数：set,传出参数，需要操作的信号集
    - 返回值：成功返回0， 失败返回-1

int sigaddset(sigset_t *set, int signum);
    - 功能：设置信号集中的某一个信号对应的标志位为1，表示阻塞这个信号
    - 参数：
        - set：传出参数，需要操作的信号集
        - signum：需要设置阻塞的那个信号
    - 返回值：成功返回0， 失败返回-1

int sigdelset(sigset_t *set, int signum);
    - 功能：设置信号集中的某一个信号对应的标志位为0，表示不阻塞这个信号
    - 参数：
        - set：传出参数，需要操作的信号集
        - signum：需要设置不阻塞的那个信号
    - 返回值：成功返回0， 失败返回-1

int sigismember(const sigset_t *set, int signum);
    - 功能：判断某个信号是否阻塞
    - 参数：
        - set：需要操作的信号集
        - signum：需要判断的那个信号
    - 返回值：
        1 ： signum被阻塞
        0 ： signum不阻塞
        -1 ： 失败
```

* 例子

~~~c
#include <signal.h>
#include <stdio.h>

int main() {

    // 创建一个信号集
    sigset_t set;

    // 清空信号集的内容 初始化的时候常用
    sigemptyset(&set);

    // 判断 SIGINT 是否在信号集 set 里
    int ret = sigismember(&set, SIGINT);
    if(ret == 0) {
        printf("SIGINT 不阻塞\n");
    } else if(ret == 1) {
        printf("SIGINT 阻塞\n");
    }

    // 添加几个信号到信号集中
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);

    // 判断SIGINT是否在信号集中
    ret = sigismember(&set, SIGINT);
    if(ret == 0) {
        printf("SIGINT 不阻塞\n");
    } else if(ret == 1) {
        printf("SIGINT 阻塞\n");
    }

    // 判断SIGQUIT是否在信号集中
    ret = sigismember(&set, SIGQUIT);
    if(ret == 0) {
        printf("SIGQUIT 不阻塞\n");
    } else if(ret == 1) {
        printf("SIGQUIT 阻塞\n");
    }

    // 从信号集中删除一个信号
    sigdelset(&set, SIGQUIT);

    // 判断SIGQUIT是否在信号集中
    ret = sigismember(&set, SIGQUIT);
    if(ret == 0) {
        printf("SIGQUIT 不阻塞\n");
    } else if(ret == 1) {
        printf("SIGQUIT 阻塞\n");
    }

    return 0;
}
~~~

##### sigprocmask() 修改内核阻塞信号集

```c
    int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
        - 功能：将自定义信号集中的数据设置到内核中（设置阻塞，解除阻塞，替换）
        - 参数：
            - how : 如何对内核阻塞信号集进行处理
                SIG_BLOCK: 将用户设置的阻塞信号集添加到内核中，内核中原来的数据不变;
                    假设内核中默认的阻塞信号集是mask， mask | set //(因为1代表阻塞，所以用或)
                SIG_UNBLOCK: 根据用户设置的数据，对内核中的数据进行解除阻塞;
                    mask &= ~set //(先取反 再与）
                 //这里要记住 | 运算的取消是 &=~
                SIG_SETMASK:覆盖内核中原来的值
            
            - set ：已经初始化好的用户自定义的信号集
            - oldset : 保存设置之前的内核中的阻塞信号集的状态，可以是 NULL
        - 返回值：
            成功：0
            失败：-1
                设置错误号：EFAULT、EINVAL

    int sigpending(sigset_t *set);
        - 功能：获取内核中的未决信号集
        - 参数：set,传出参数，保存的是内核中的未决信号集中的信息。
```

例子：

```c
// 编写一个程序，把所有的常规信号（1-31）的未决状态打印到屏幕
// 设置某些信号是阻塞的，通过键盘产生这些信号
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    //设置信号集，设置2 ，3号信号阻塞
    sigset_t set;
    sigemptyset(&set);
    //把2,3号信号添加到set
    sigaddset(&set,SIGINT);
    sigaddset(&set,SIGQUIT);

    //修改内核中阻塞信号集
    sigprocmask(SIG_BLOCK,&set,NULL);

    int num = 0;
    while(1){
        num++;
        //获取当前未决的信号集
        sigset_t pendingset;
        sigemptyset(&pendingset);
        sigpending(&pendingset);

        //遍历前32位
        for (int i = 1; i < 32; i++)
        {
            if(sigismember(&pendingset,i) == 1){
                printf("1");
            }else if(sigismember(&pendingset,i) == 0){
                printf("0");
            }else{
                perror("sigprocmask");
                exit(0);
            }
            
        }
        printf("\n");
        sleep(1);
        if(num == 10){
            //解除阻塞
            sigprocmask(SIG_UNBLOCK,&set,NULL);
        }
    }
    return 0;
}
```

#####  sigaction()信号捕捉函数

* 最好用sigaction,而不是signal，因为标准可能不太一样

```c
    #include <signal.h>
    int sigaction(int signum, const struct sigaction *act,
                            struct sigaction *oldact);

        - 功能：检查或者改变信号的处理。信号捕捉
        - 参数：
            - signum : 需要捕捉的信号的编号或者宏值（信号的名称）
            - act ：捕捉到信号之后的处理动作
            - oldact : 上一次对信号捕捉相关的设置，一般不使用，传递NULL
        - 返回值：
            成功 0
            失败 -1
/*sigaction的回调函数int类型参数，就是信号类型的宏定义；
	这样可以用管道搭配epoll来读传入的信号：
		1.用sig的回调函数向fd[1]写，传到fd[0]读端 
		2.然后把fd[0]加入epoll实例，就可以IO多路复用，得到突然传入的sig，并且用Switch判断sig的类型
*/
     struct sigaction {
        // 函数指针，指向的函数就是信号捕捉到之后的处理函数
        void     (*sa_handler)(int);
        // 不常用
        void     (*sa_sigaction)(int, siginfo_t *, void *);
        // 临时阻塞信号集，在信号捕捉函数执行过程中，临时阻塞某些信号。回调函数执行完之后，会转换到系统的阻塞信号集。这里一般需要把该临时阻塞信号集清空sigemptyset()
         
        sigset_t   sa_mask;
        // 使用哪一个信号处理对捕捉到的信号进行处理
        // 这个值可以是0，表示使用sa_handler,也可以是SA_SIGINFO表示使用sa_sigaction
        int        sa_flags;
        // 被废弃掉了
        void     (*sa_restorer)(void);
    };

//信号捕捉特性：
1. 信号捕捉过程中会使用临时的阻塞信号集，处理完后，切换恢复到内核的阻塞信号集
2. 在回调sa_handler函数的期间，默认会屏蔽该函数的调用，执行完之后，才能继续处理
3. 信号集只是标志位，不能支持排队

```

<img src="D:\MyTxt\typoraPhoto\image-20230226165653806.png" alt="image-20230226165653806" style="zoom: 80%;" />

* 例子

```c

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void myalarm(int num) {
    printf("捕捉到了信号的编号是：%d\n", num);
    printf("xxxxxxx\n");
}

// 过3秒以后，每隔2秒钟定时一次
int main() {

    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = myalarm;
    sigemptyset(&act.sa_mask);  // 清空临时阻塞信号集
   
    // 注册信号捕捉
    sigaction(SIGALRM, &act, NULL);

    struct itimerval new_value;

    // 设置间隔的时间
    new_value.it_interval.tv_sec = 2;
    new_value.it_interval.tv_usec = 0;

    // 设置延迟的时间,3秒之后开始第一次定时
    new_value.it_value.tv_sec = 3;
    new_value.it_value.tv_usec = 0;

    int ret = setitimer(ITIMER_REAL, &new_value, NULL); // 非阻塞的
    printf("定时器开始了...\n");

    if(ret == -1) {
        perror("setitimer");
        exit(0);
    }

    // getchar();
    while(1);

    return 0;
}
```

##### SIGCHLD信号 - 回收子进程

*  SIGCHLD信号产生的条件

  1. 子进程终止时
  2. 子进程接收到 SIGSTOP 信号停止时
  3. 子进程处在停止态，接受到SIGCONT后唤醒时 

  以上三种条件都会给父进程发送 SIGCHLD 信号，**父进程默认会忽略该信号**

###### SIGCHLD 实现回收子进程 - 比较常用配合waitpid来批量回收子进程

* 不阻塞父进程；提前捕捉，防止全死完了

```c
//   使用SIGCHLD信号解决僵尸进程的问题。
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <unistd.h>
#include <sys/stat.h>

//回收子进程的Callback function
void myFun(int num){
    printf("捕捉到信号：%d\n",num);
    //回收子进程PCB
    while(1){
        //wait调用一次只能回收一个，如果多个子进程同时死了，例如4个，那么肯定会发送4次信号，但是有可能有些信号会被忽略掉，所以要循环把所有的死了的子进程都回收掉用waitpid()配合循环。
        int ret = waitpid(-1,NULL,WNOHANG);
        if(ret > 0){
            //回收了一个子进程
            printf("回收死亡的子进程：%d\n",ret);
        }else{
            //rer == 0还有子进程活着,等他们死
            //ret < 0没有活着的子进程了
            break;
        }       
    }
}

int main(){
    //提前设置好阻塞信号集，阻塞SIGCHLD，因为有可能子进程很快结束，父进程还没有注册完信号捕捉
    /*1. 这里理解好阻塞:阻塞是接受了不处理。
      2. 为什么先阻塞SIGCHLD就可以保证父进程能处理所有僵尸进程？设置了SIGCHLD在阻塞信号集中阻塞的1，你们只要是	          只要曾经收到过SIGCHLD，未决就会保持1；等待之后父进程sigaction去处理，sigcation处理完之后置0；这里父进          程去处理的时候，只要未决是1，也就是不用考虑有多少个已经死了，只要知道有僵尸子进程，就可以用while循环加上          waitpid全部回收完成；
      	补充：因为对SIGCHILD信号的默认处理是将其忽略（此时捕捉信号函数还没有注册），如果没有阻塞，直接忽略掉，那			父进程就不知道之前发生了什么事情。
      3.《Linux/UNIX系统编程手册》指出为了保障可移植性，应用应在创建任何子进程之前就设置信号捕捉函数。【牛客			789400243号】提出了这个观点，应该在fork之前就注册信号捕捉的。其实就是对应了书上这句话。
    */
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,SIGCHLD);
    sigprocmask(SIG_BLOCK,&set,NULL);

    //创建一些子进程
    pid_t pid;
    for (int i = 0; i < 20; i++)
    {
        pid = fork();
        if(pid == 0){
            break;
        }
    }
    //父进程在不阻塞情况下，回收子进程PCB
    if(pid >0){
        //设置捕捉子进程死亡的SIGCHLD
		
        struct sigaction act;
        act.sa_flags = 0;
        act.sa_handler = myFun;
        sigaction(SIGCHLD,&act,NULL);
        
        //为了不阻塞父进程自己的事情，在此处打开
        sigprocmask(SIG_UNBLOCK,&set,NULL);

        //父进程自己的事情
        while (1){
            printf("parent process pid: %d\n",getpid());
            sleep(2);
        }
        
    }else if(pid == 0){
        printf("child process pid: %d\n",getpid());
    }
    
    return 0;
}
```

### 2.8共享内存

1. 共享内存允许两个或者多个进程共享物理内存的同一块区域（通常被称为段）。由于一个共享内存段会成为一个进程用户空间的一部分，因此这种 IPC 机制**无需内核介入**（较少介入）。所有需要做的就是让一个进程将数据复制进共享内存中，并且这部分数据会对其他所有共享同一个段的进程可用。
2. 与管道等要求发送进程将数据从用户空间的缓冲区复制进内核内存和接收进程将数据从内核内存复制进用户空间的缓冲区的做法相比，这种 IPC 技术的速度更快。

##### 共享内存使用步骤

1. 调用 shmget() 创建一个新共享内存段或取得一个既有共享内存段的标识符（即由其他进程创建的共享内存段）。这个调用将返回后续调用中需要用到的**共享内存标识符**。
2. 使用 shmat() 来**附上**共享内存段，即使该段成为调用进程的虚拟内存的一部分。
3. 此刻在程序中可以像对待其他可用内存那样对待这个共享内存段。为引用这块共享内存，程序需要使用由 shmat() 调用返回的 addr 值，它是一个**指向进程的虚拟地址空间中该共享内存段的起点的指针**。
4. 调用 shmdt() 来**分离**共享内存段。在这个调用之后，进程就无法再引用这块共享内存了。这一步是可选的，并且在进程终止时会自动完成这一步。
5. 调用 shmctl() 来**删除**共享内存段。只有当当前所有附加内存段的进程都与之分离之后内存段才会销毁。只有一个进程需要执行这一步。

##### 共享内存函数

```c
共享内存相关的函数
#include <sys/ipc.h>
#include <sys/shm.h>

int shmget(key_t key, size_t size, int shmflg);
    - 功能：创建一个新的共享内存段，或者获取一个既有的共享内存段的标识。
        新创建的内存段中的数据都会被初始化为0
    - 参数：
        - key : key_t类型是一个整形，通过这个找到或者创建一个共享内存。
                一般使用16进制表示，非0值
        - size: 共享内存的大小
        - shmflg: 属性
            - 访问权限
            - 附加属性：创建/判断共享内存是不是存在
                - 创建：IPC_CREAT
                - 判断共享内存是否存在： IPC_EXCL , 需要和IPC_CREAT一起使用
                    IPC_CREAT | IPC_EXCL | 0664
        - 返回值：
            失败：-1 并设置错误号
            成功：>0 返回共享内存的引用的ID，后面操作共享内存都是通过这个值。


void *shmat(int shmid, const void *shmaddr, int shmflg);
    - 功能：和当前的进程进行关联
    - 参数：
        - shmid : 共享内存的标识（ID）,由shmget返回值获取
        - shmaddr: 申请的共享内存的起始地址，指定NULL，内核指定
        - shmflg : 对共享内存的操作
            - 读 ： SHM_RDONLY, 必须要有读权限
            - 读写： 0
    - 返回值：
        成功：返回共享内存的首（起始）地址。  失败(void *) -1


int shmdt(const void *shmaddr);
    - 功能：解除当前进程和共享内存的关联
    - 参数：
        shmaddr：共享内存的首地址
    - 返回值：成功 0， 失败 -1

int shmctl(int shmid, int cmd, struct shmid_ds *buf);
    - 功能：对共享内存进行操作。删除共享内存，共享内存要删除才会消失，创建共享内存的进行被销毁了对共享内存是没有任何影响。
    - 参数：
        - shmid: 共享内存的ID
        - cmd : 要做的操作
            - IPC_STAT : 获取共享内存的当前的状态
            - IPC_SET : 设置共享内存的状态
            - IPC_RMID: 标记共享内存被销毁 //因为有多个进程指向，类似于智能指针，全部标记销毁就销毁
        - buf：需要设置或者获取的共享内存的属性信息
            - IPC_STAT : buf存储数据
            - IPC_SET : buf中需要初始化数据，设置到内核中
            - IPC_RMID : 没有用，NULL

key_t ftok(const char *pathname, int proj_id);
    - 功能：根据指定的路径名，和int值，生成一个共享内存的key
    - 参数：
        - pathname:指定一个存在的路径
            /home/nowcoder/Linux/a.txt
            / 
        - proj_id: int类型的值，但是这系统调用只会使用其中的1个字节8个位
                   范围 ： 0-255  一般指定一个字符 'a'
```

* 例子：

```c
read_shm.c
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <memory.h>
int main(){
    //1.创建一个进程
    int shmid = shmget(100,4096,IPC_CREAT|0644);
    printf("share memery id:%d\n",shmid);
    //2.和当前进程进行关联
    void *ptr = shmat(shmid,NULL,0);
    //3. 读数据
    printf("%s\n",(char*)ptr);
    
    printf("按任意键继续\n");
    getchar();
    //4.解除关联
    shmdt(ptr);

    //5.删除共享内存
    shmctl(shmid,IPC_RMID,NULL);

    return 0;
}
```

```c
write_shm.c
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <memory.h>
int main(){
    //1.创建一个进程
    int shmid = shmget(100,4096,IPC_CREAT|0644);
    printf("share memery id:%d\n",shmid);
    //2.和当前进程进行关联
    void *ptr = shmat(shmid,NULL,0);

    char *str = "helloworld";

    //3. 写数据
    memcpy(ptr,str,strlen(str)+1);

    printf("按任意键继续\n");
    getchar();

    //4.解除关联
    shmdt(ptr);

    //5.删除共享内存
    shmctl(shmid,IPC_RMID,NULL);

    return 0;
}
```

##### 共享内存操作命令

1. ipcs 用法
    ipcs -a // 打印当前系统中**所有的进程间**通信方式的信息
    ipcs -m // 打印出**使用共享内存**进行进程间通信的信息
    ipcs -q // 打印出**使用消息队列**进行进程间通信的信息
    ipcs -s // 打印出**使用信号进行进程间通信**的信息
2.  ipcrm 用法
    ipcrm -M shmkey    // 移除用shmkey创建的共享内存段
    ipcrm -m shmid // 移除用shmid标识的共享内存段
    ipcrm -Q msgkey // 移除用msqkey创建的消息队列
    ipcrm -q msqid // 移除用msqid标识的消息队列
    ipcrm -S semkey // 移除用semkey创建的信号
    ipcrm -s semid // 移除用semid标识的信号

##### 基础问题：

```shell
问题1：操作系统如何知道一块共享内存被多少个进程关联？
    - 共享内存维护了一个结构体struct shmid_ds 这个结构体中有一个成员 shm_nattch
    - shm_nattach 记录了关联的进程个数

问题2：可不可以对共享内存进行多次删除 shmctl
    - 可以的
    - 因为shmctl 标记删除共享内存，不是直接删除
    - 什么时候真正删除呢?
        当和共享内存关联的进程数为0的时候，就真正被删除
    - 当共享内存的key为0的时候，表示共享内存被标记删除了
        如果一个进程和共享内存取消关联，那么这个进程就不能继续操作这个共享内存。也不能进行关联。

问题3：共享内存和内存映射的区别
    1.共享内存可以直接创建，内存映射需要磁盘文件（匿名映射除外）
    2.共享内存效果更高
    3.内存
        所有的进程操作的是同一块共享内存。
        内存映射，每个进程在自己的虚拟地址空间中有一个独立的内存。
    4.数据安全
        - 进程突然退出
            共享内存还存在
            内存映射区消失
        - 运行进程的电脑死机，宕机了
            数据存在在共享内存中，没有了
            内存映射区的数据 ，由于磁盘文件中的数据还在，所以内存映射区的数据还存在。

    5.生命周期
        - 内存映射区：进程退出，内存映射区销毁
        - 共享内存：进程退出，共享内存还在，标记删除（所有的关联的进程数为0），或者关机
            如果一个进程退出，会自动和共享内存进行取消关联。
```

### 2.9守护进程

##### 终端

 在 UNIX 系统中，用户通过终端登录系统后得到一个 shell 进程，这个终端成为 shell 进程的**控制终端（Controlling Terminal）**，进程中，控制终端是保存在 PCB 中的信息，而 fork() 会复制 PCB 中的信息，因此由 shell 进程启动的其它进程的控制终端也是这个终端。

默认情况下（没有重定向），每个进程的**标准输入、标准输出和标准错误输出都指向控制终端**，进程从标准输入读也就是读用户的键盘输入，进程往标准输出或标准错误输出写也就是输出到显示器上。

在控制终端输入一些特殊的控制键可以给前台进程发信号，例如 Ctrl + C 会产生 SIGINT 信号，Ctrl + \ 会产生 SIGQUIT 信号。

##### 进程组

进程组和会话在进程之间形成了一种两级层次关系：**进程组是一组相关进程的集合，会话是一组相关进程组的集合**。进程组和会话是为支持 shell 作业控制而定义的抽象概念，用户通过 shell 能够交互式地在前台或后台运行命令。

进程组由一个或多个共享**同一进程组标识符（PGID）**的进程组成。一个进程组拥有一个进程组**首进程**，该进程是创建该组的进程，其进程 ID 为该进程组的 ID，新进程会继承其父进程所属的进程组 ID。

进程组拥有一个生命周期，其开始时间为**首进程创建组的时刻**，结束时间为**最后一个**
**成员进程退出组的时刻**。一个进程可能会因为终止而退出进程组，也可能会因为加入
了另外一个进程组而退出进程组。进程组首进程无需是最后一个离开进程组的成员。

##### 会话

◼ 会话是一组进程组的集合。会话首进程是创建该新会话的进程，其进程 ID 会成为会
话 ID。新进程会继承其父进程的会话 ID（**SID**）。

◼ 一个会话中的所有进程**共享单个控制终端**。控制终端会在会话首进程首次打开一个终
端设备时被建立。一个终端最多可能会成为**一个**会话的控制终端。

◼ 在任一时刻，会话中的其中一个进程组会成为终端的前台进程组，其他进程组会成为
后台进程组。只有**前台进程组中的进程才能从控制终端中读取输入**。当用户在控制终
端中输入终端字符生成信号后，该信号会被发送到前台进程组中的所有成员。

◼ 当控制终端的连接建立起来之后，会话首进程会成为该终端的控制进程。

![image-20230227112850159](D:\MyTxt\typoraPhoto\image-20230227112850159.png)

* 进程组、会话操作函数

◼ pid_t getpgrp(void);
◼ pid_t getpgid(pid_t pid);
◼ int setpgid(pid_t pid, pid_t pgid);
◼ pid_t getsid(pid_t pid);
◼ pid_t setsid(void);

##### 守护进程

◼ 守护进程（Daemon Process），也就是通常说的 Daemon 进程（精灵进程），是
Linux 中的**后台服务进程**。它是一个生存期较长的进程，通常独立于控制终端并且周
期性地执行某种任务或等待处理某些发生的事件。一般采用以 d 结尾的名字。

◼ 守护进程具备下列特征：

1. 生命周期很长，守护进程会在系统启动的时候被**创建并一直运行直至系统被关闭**。
2. 它在后台运行并且**不拥有控制终端**。没有控制终端确保了内核永远不会为守护进程自动生成任何控制信号以及终端相关的信号（如 SIGINT、SIGQUIT）。

* Linux 的大多数服务器就是用守护进程实现的。比如，Internet 服务器 inetd，Web 服务器 httpd 等

###### 守护进程创建步骤

◼ 执行一个 fork()，之后父进程退出，子进程继续执行。（1. 防止父进程死亡后出现shell提示符；2.防止该进程成为组进程的首进程）

◼ 子进程调用 setsid() 开启一个新会话。

◼ 清除进程的 umask 以确保当守护进程创建文件和目录时拥有所需的权限。
◼ 修改进程的当前工作目录，通常会改为根目录（/）。
◼ 关闭守护进程从其父进程继承而来的所有打开着的文件描述符。
◼ 在关闭了文件描述符0、1、2之后，守护进程通常会打开/dev/null 并使用dup2()
使所有这些描述符指向这个设备。
◼ 核心业务逻辑

* 例子：

```c
//写一个守护进程，每隔两秒获取系统时间，并且把这个时间写入到磁盘文件

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <time.h>

void myFun(int num){
//捕捉到定时信号之后，获取系统时间，写入磁盘文件
    time_t tm =time(NULL);
    struct tm *loc = localtime(&tm);
    // //直接打印,但是文件描述符 1 2 3都已经重定向了，所以打印不到终端
    // char buf[1024];
    // sprintf(buf,"%d-%d-%d,%d:%d:%d\n",loc->tm_year,loc->tm_mon,loc->tm_mday,
    // loc->tm_hour,loc->tm_min,loc->tm_sec);
    // printf("%s\n",buf);

    //格式化时间，写入磁盘文件中
    char* str = asctime(loc);
    int fd = open("a.txt",O_RDWR|O_CREAT|O_APPEND,0664);
    write(fd,str,strlen(str));
    close(fd);
}

int main(){
//守护进程创建步骤
    //1.创建子进程，退出父进程
    pid_t pid = fork();
    if(pid > 0) exit(0);

    //2.子进程开启新会话
    setsid();
    
    //3.设置掩码 信号掩码（也就是阻塞信号集）
    umask(022);

    //4.修改进程的当前工作目录（注意权限）
    chdir("/home/klchen/");

    //5.关闭、重定向文件描述符(让标准输入、输出、错误，也就是0 1 2重定向到那个设备)
    int fd = open("/dev/null",O_RDWR);
    dup2(fd,STDIN_FILENO);
    dup2(fd,STDOUT_FILENO);
    dup2(fd,STDERR_FILENO);

    //5. 业务逻辑

    //捕捉定时信号
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = myFun;
    sigemptyset(&act.sa_mask);

    sigaction(SIGALRM,&act,NULL);

    //创建定时器
    struct itimerval val;
    val.it_value.tv_sec = 2;
    val.it_value.tv_usec = 0;
    val.it_interval.tv_sec =2;
    val.it_interval.tv_usec =0; 
    setitimer(ITIMER_REAL,&val,NULL);

    //保持进程不关闭
    while(1){
        sleep(10);
    }
    return 0;
}
```

## 3.Linux多线程开发

### 3.1线程概述

与进程（process）类似，线程（thread）是允许应用程序并发执行多个任务的一种机
制。一个进程可以包含多个线程。同一个程序中的所有线程均会独立执行相同程序，且共享同一份全局内存区域，其中包括初始化数据段、未初始化数据段，以及堆内存段。（传统意义上的 UNIX 进程只是多线程程序的一个特例，该进程只包含一个线程）

* **进程是 CPU 分配资源的最小单位，线程是操作系统调度执行的最小单位。**
* 线程是**轻量级的进程**（**LWP**：Light Weight Process），在 Linux 环境下线程的本
  质仍是进程。
* 查看指定进程的 LWP 号：ps –Lf pid

* 共享资源
   进程 ID 和父进程 ID
   进程组 ID 和会话 ID
   用户 ID 和 用户组 ID
   文件描述符表
   信号处置
   文件系统的相关信息：文件权限掩码
  （umask）、当前工作目录
   虚拟地址空间（除栈、.text）//这两个会被划分
* 非共享资源
   线程 ID
   信号掩码
   线程特有数据
   error 变量
   实时调度策略和优先级
   栈，本地变量和函数的调用链接信息
*  查看当前 pthread 库版本：`getconf GNU_LIBPTHREAD_VERSION`

##### 线程与进程的区别

1.  进程间的信息难以共享。由于除去只读代码段外，父子进程并未共享内存，因此必须采用一些进程间通信方式，在进程间进行信息交换。
2. 调用 fork() 来创建进程的代价相对较高，即便利用写时复制技术，仍然需要复制诸如内存页表和文件描述符表之类的多种进程属性，这意味着 fork() 调用在时间上的开销依然不菲。
3. 线程之间能够方便、快速地共享信息。只需将数据复制到共享（全局或堆）变量中即可。
4. 创建线程比创建进程通常要快 10 倍甚至更多。线程间是共享虚拟地址空间的，无需采用写时复制来复制内存，也无需复制页表。

### 3.2线程操作

##### 线程创建

```c
    一般情况下,main函数所在的线程我们称之为主线程（main线程），其余创建的线程
    称之为子线程。
    程序中默认只有一个进程，fork()函数调用，2个进程
    程序中默认只有一个线程，pthread_create()函数调用，2个线程。

    #include <pthread.h>
    int pthread_create(pthread_t *thread, const pthread_attr_t *attr, 
    void *(*start_routine) (void *), void *arg);

        - 功能：创建一个子线程
        - 参数：
            - thread：传出参数，线程创建成功后，子线程的线程ID被写到该变量中。
            - attr : 设置线程的属性，一般使用默认值，NULL
            - start_routine : //函数指针，这个函数是子线程需要处理的逻辑代码,注意函数返回值void*,俺的理解是调用函数指针后，还是需要返回值告诉主线程创建子线程及子线程工作的成功与否，与前面的sigaction信号触发后的回调函数不一样，那个不需要返回执行正确与否
            - arg : 给第三个参数使用，传参
        - 返回值：
            成功：0
            失败：返回错误号。这个错误号和之前errno不太一样。
            获取错误号的信息：  char * strerror(int errnum);
```

* 例子-gcc pthread_create.c -o pthread_create -lpthread

```c
//注意线程不是标准库 ，需要指定第三方库名称 -l pthread
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

//1. 子线程创建的callback Fuction, 子线程需要处理的逻辑代码，传入参数指定为任意类型指针 2. 此处返回的是void*类型 返回任意类型指针 
void* callback(void* arg){
    printf("child thread...\n");
    printf("arg: %d\n",*((int*)arg));
    return NULL;
}
//main()函数都在主线程中执行
int main(){
    pthread_t tid;

    //测试callback的传入参数
    int num = 10;
    //创建一个子线程,测试回调函数及其参数
    int ret = pthread_create(&tid,NULL,callback,(void*)&num);
    if(ret != 0){
        char *error = strerror(ret);
        printf("error: %s",error);
    }
    printf("1\n");
    sleep(1);
    return 0;
}
```

##### 线程退出

```c
    #include <pthread.h>
    void pthread_exit(void *retval);
        功能：终止一个线程，在哪个线程中调用，就表示终止哪个线程
        参数：
            retval:需要传递一个指针，作为一个返回值，可以在pthread_join()中获取到。(类似于return (void*)&value;)

    pthread_t pthread_self(void);
        功能：获取当前的线程的线程ID

    int pthread_equal(pthread_t t1, pthread_t t2);
        功能：比较两个线程ID是否相等
        不同的操作系统，pthread_t类型的实现不一样，有的是无符号的长整型，有的
        是使用结构体去实现的。
```

* 例子-线程退出

```c
#include <stdio.h>
#include <pthread.h>
#include <string.h>

void * callback(void * arg) {
    printf("child thread id : %ld\n", pthread_self());
    return NULL;    //相当于pthread_exit(NULL);
} 

int main() {

    // 创建一个子线程
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, callback, NULL);

    if(ret != 0) {
        char * errstr = strerror(ret);
        printf("error : %s\n", errstr);
    }

    // 主线程
    for(int i = 0; i < 5; i++) {
        printf("%d\n", i);
    }

    printf("tid : %ld, main thread id : %ld\n", tid ,pthread_self());

    // 让主线程退出,当主线程退出时，不会影响其他正常运行的线程。
    pthread_exit(NULL);
	//mian thread已经退出，之后代码不会运行
    printf("main thread exit\n");

    return 0;   // exit(0);
}
```

##### 连接已终止的线程 - 线程回收

```c
    #include <pthread.h>
    int pthread_join(pthread_t thread, void **retval);
        - 功能：和一个已经终止的线程进行连接
                回收子线程的资源
                这个函数是阻塞函数，调用一次只能回收一个子线程
                一般在主线程中使用
        - 参数：
            - thread：需要回收的子线程的ID
            - retval: 接收子线程退出时的返回值
        - 返回值：
            0 : 成功
            非0 : 失败，返回的错误号
```

* 例子-回收子线程

```c
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

int num = 10;
void* callback(void *arg){
    printf("child thread: %ld\n",pthread_self());
    //退出并且设置返回值
    pthread_exit((void*)&num);//相当于 return (void*)&num              
}
int main(){
    //创建子线程
    pthread_t tid;
    int ret = pthread_create(&tid,NULL,callback,NULL);
    if(ret != 0){
        char * error1 = strerror(ret);
        printf("error: %s",error1);

    }
    //主线程的事情
    for(int i =0 ;i != 5;i++){
        printf("i\n");
    }
    printf("tid:%ld,main thread tid:%ld",tid,pthread_self());

    //主进程调用pthread_exit()回收子线程的资源
    int *thread_return;
    //二级指针，因为传出参数是指针，需要用二级指针指向他
    ret = pthread_join(tid,(void**)&thread_return);
    if (ret !=0)
    {
        char *error2 = strerror(ret);
        printf("error: %s\n",error2);
    }
    printf("exit data: %d\n",*thread_return);
    printf("子线程回收成功!\n");

    
    // 让主线程退出,当主线程退出时，不会影响其他正常运行的线程。
    pthread_exit(NULL);
    return 0;
}
```

##### 线程分离

```c
    #include <pthread.h>
    int pthread_detach(pthread_t thread);
        - 功能：分离一个线程。被分离的线程在终止的时候，会自动释放资源返回给系统。
          1.不能多次分离，会产生不可预料的行为。
          2.不能去连接一个已经分离的线程，会报错。
        - 参数：需要分离的线程的ID
        - 返回值：
            成功：0
            失败：返回错误号
```

* 例子：线程分离

```c
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void * callback(void * arg) {
    printf("chid thread id : %ld\n", pthread_self());
    return NULL;
}

int main() {

    // 创建一个子线程
    pthread_t tid;

    int ret = pthread_create(&tid, NULL, callback, NULL);
    if(ret != 0) {
        char * errstr = strerror(ret);
        printf("error1 : %s\n", errstr);
    }

    // 输出主线程和子线程的id
    printf("tid : %ld, main thread id : %ld\n", tid, pthread_self());
  
    // 设置子线程分离,子线程分离后，子线程结束时对应的资源就不需要主线程释放
    ret = pthread_detach(tid);
    if(ret != 0) {
        char * errstr = strerror(ret);
        printf("error2 : %s\n", errstr);
    }

    // 设置分离后，对分离的子线程进行连接 ,连接不上了哦 pthread_join()
    // ret = pthread_join(tid, NULL);
    // if(ret != 0) {
    //     char * errstr = strerror(ret);
    //     printf("error3 : %s\n", errstr);
    // }

    pthread_exit(NULL);

    return 0;
}
```

##### 线程取消

```c
    #include <pthread.h>
    int pthread_cancel(pthread_t thread);
        - 功能：取消线程（让线程终止）
            取消某个线程，可以终止某个线程的运行，
            但是并不是立马终止，而是当子线程执行到一个取消点，线程才会终止。
            取消点cancellaction point：系统规定好的一些系统调用，我们可以粗略的理解为从用户区到内核区的切换，这个位置称之为取消点。
```

```c
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void * callback(void * arg) {
    printf("chid thread id : %ld\n", pthread_self());
    for(int i = 0; i < 5; i++) {
        //printf是一个取消点
        printf("child : %d\n", i);
    }
    return NULL;
}

int main() {
    
    // 创建一个子线程
    pthread_t tid;

    int ret = pthread_create(&tid, NULL, callback, NULL);
    if(ret != 0) {
        char * errstr = strerror(ret);
        printf("error1 : %s\n", errstr);
    }

    // 取消线程
    pthread_cancel(tid);

    for(int i = 0; i < 5; i++) {
        printf("%d\n", i);
    }

    // 输出主线程和子线程的id
    printf("tid : %ld, main thread id : %ld\n", tid, pthread_self());

    
    pthread_exit(NULL);

    return 0;
}
```

##### 线程属性

```c
    int pthread_attr_init(pthread_attr_t *attr);
        - 初始化线程属性变量

    int pthread_attr_destroy(pthread_attr_t *attr);
        - 释放线程属性的资源

    int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
        - 获取线程分离的状态属性

    int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
        - 设置线程分离的状态属性
```

* 例子

```c
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void* callback(void *arg){
    printf("child thread id: %ld\n",pthread_self());
    return (NULL);
}

int main(){
    //创建线程属性变量
    pthread_attr_t attr;
    //初始化attr
    pthread_attr_init(&attr);

    //设置属性变量为分离
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    
    //创建子线程
    pthread_t tid;
    //创建的时候添加参数attr，设置分离
    int ret = pthread_create(&tid,&attr,callback,NULL);
    if(ret != 0){
        char *errst = strerror(ret);
        printf("error1:%s\n",errst);
    }
    //输出线程id
    printf("tid:%ld,main thread id: %ld\n",tid,pthread_self());
    
    //释放线程资源
    pthread_attr_destroy(&attr);

    pthread_exit(NULL);

    return 0;
}
```

### 3.3线程同步

* 线程同步
  1. 线程的主要优势在于，能够通过**全局变量来共享信息**。不过，这种便捷的共享是有代价的：必须确保多个线程不会同时修改同一变量，或者某一线程不会读取正在由其他线程修改的变量。
  2.  **临界区**是指访问某一**共享资源的代码片段**，并且这段代码的执行应为**原子操作**，也就是同时访问同一共享资源的其他线程不应终端该片段的执行。
  3.  **线程同步**：即当有一个线程在对内存进行操作时，其他线程都不可以对这个内存地址进行操作，直到该线程完成操作，其他线程才能对该内存地址进行操作，而其他线程则处于等待状态。

* 互斥量

为避免线程更新共享变量时出现问题，可以使用互斥量（mutex 是 mutual exclusion的缩写）来确保同时仅有一个线程可以访问某项共享资源。可以使用互斥量来保证对任意共
享资源的原子访问。

1. 互斥量有两种状态：已锁定（locked）和未锁（unlocked）。任何时候，至多只有一个线程可以锁定该互斥量。试图对已经锁定的某一互斥量再次加锁，将可能阻塞线程或者报错失败，具体取决于加锁时使用的方法 。
2. 一旦线程锁定互斥量，随即成为该互斥量的所有者，只有所有者才能给互斥量解锁。一般情况下，对每一共享资源（可能由多个相关变量组成）会使用不同的互斥量，每一线程在访问
3. 同一资源时将采用如下协议 ：
   ⚫ 针对共享资源锁定互斥量
   ⚫ 访问共享资源
   ⚫ 对互斥量解锁

##### 互斥锁

如果多个线程试图执行这一块代码（一个临界区），事实上只有一个线程能够持有该互斥量（其他线程将遭到阻塞），即同时只有一个线程能够进入这段代码区域。

<img src="D:\MyTxt\typoraPhoto\image-20230228103959862.png" alt="image-20230228103959862" style="zoom: 50%;" />

```c
    互斥量的类型 pthread_mutex_t
    int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr);
        - 初始化互斥量
        - 参数 ：
            - mutex ： 需要初始化的互斥量变量
            - attr ： 互斥量相关的属性，NULL
        - restrict : C语言的修饰符，被修饰的指针，不能由另外的一个指针进行操作。
            pthread_mutex_t *restrict mutex = xxx;
            pthread_mutex_t * mutex1 = mutex;//报错

    int pthread_mutex_destroy(pthread_mutex_t *mutex);
        - 释放互斥量的资源

    int pthread_mutex_lock(pthread_mutex_t *mutex);
        - 加锁，阻塞的，如果有一个线程加锁了，那么其他的线程只能阻塞等待

    int pthread_mutex_trylock(pthread_mutex_t *mutex);
        - 尝试加锁，如果加锁失败，不会阻塞，会直接返回。

    int pthread_mutex_unlock(pthread_mutex_t *mutex);
        - 解锁
```

* 例子 - 用三个线程并发卖票，保证同步互斥

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//创建全局的互斥量
pthread_mutex_t mutex;
int ticket = 1000;
//子线程 共享函数
void *sellticket(void *arg){
//卖票
    //加锁
    pthread_mutex_lock(&mutex);
    while(1){
        if(ticket > 0){
            printf("%ld正在卖第 %d 张票\n",pthread_self(),ticket--);
        }else{
            //注意加锁之后要保证能够解锁，特别注意break,exit()这些直接退出的
            pthread_mutex_unlock(&mutex);
            break;
        }
        //解锁
        pthread_mutex_unlock(&mutex);
        usleep(6000);
    }
}


int main(){
    //初始化锁
    pthread_mutex_init(&mutex,NULL);
    
    //创建子线程
    pthread_t tid1,tid2,tid3;
    pthread_create(&tid1,NULL,sellticket,NULL);
    pthread_create(&tid2,NULL,sellticket,NULL);
    pthread_create(&tid3,NULL,sellticket,NULL);

    //回收子线程资源，阻塞
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);
    
    //释放互斥资源
    pthread_mutex_destroy(&mutex);
        //主线程退出
    pthread_exit(NULL);

    return 0;
}
```

##### 死锁

 有时，一个线程需要同时访问两个或更多不同的共享资源，而每个资源又都由不同的互
斥量管理。当超过一个线程加锁同一组互斥量时，就有可能发生死锁。
◼ **两个或两个以上的进程在执行过程中，因争夺共享资源而造成的一种互相等待的现象**，
若无外力作用，它们都将无法推进下去。此时称系统处于死锁状态或系统产生了死锁。

* 死锁的几种场景：
  1. 忘记释放锁
  2. 重复加锁
  3. 多线程多锁，抢占锁资源

<img src="D:\MyTxt\typoraPhoto\image-20230228111719038.png" alt="image-20230228111719038" style="zoom: 50%;" />

* 演示死锁 

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 创建2个互斥量
pthread_mutex_t mutex1, mutex2;

void * workA(void * arg) {

    pthread_mutex_lock(&mutex1);
    //首先，在 workA 中使用 pthread_mutex_lock(&mutex1) 对线程A进行加锁，但是由于 cpu 执行的速度很快，导致在 workB 中，还没有使用 pthread_mutex_lock(&mutex2) 对线程B进行加锁，workA就继续向下执行，因为资源 B 并没有被线程 B 加锁，所以在 workA 中的 pthread_mutex_lock(&mutex2) 并未处于阻塞状态
    sleep(1);
    //如果B已经锁住了，那这里A就阻塞
    pthread_mutex_lock(&mutex2);

    printf("workA....\n");

    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    return NULL;
}


void * workB(void * arg) {
    pthread_mutex_lock(&mutex2);
    sleep(1);
    //这个因为A已经锁了，B阻塞
    pthread_mutex_lock(&mutex1);

    printf("workB....\n");

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);

    return NULL;
}

int main() {

    // 初始化互斥量
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    // 创建2个子线程
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, workA, NULL);
    pthread_create(&tid2, NULL, workB, NULL);

    // 回收子线程资源
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // 释放互斥量资源
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    return 0;
}
```

#####  读写锁

读写锁
◼ 当有一个线程已经持有互斥锁时，互斥锁将所有试图进入临界区的线程都阻塞住。但是考虑一种情形，当前持有互斥锁的线程只是要读访问共享资源，而同时有其它几个线程也想读取这个共享资源，但是由于互斥锁的**绝对排它性**，所有其它线程都无法获取锁，也就无法读访问共享资源了，但是实际上多个线程同时读访问共享资源并不会导致问题。
◼ 在对数据的读写操作中，更多的是读操作，写操作较少，例如对数据库数据的读写应用。
为了满足当前能够**允许多个读出，但只允许一个写入的需求**，线程提供了读写锁来实现。

* 读写锁的特点：
  1. 如果有其它线程读数据，则允许其它线程执行读操作，但不允许写操作。
  2. 如果有其它线程写数据，则其它线程都不允许读、写操作。
  3. 写是独占的，写的优先级高。



```c
    读写锁的类型 pthread_rwlock_t
    int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t *restrict attr);
    int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
    int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
    int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
    int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
    int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
    int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

    案例：8个线程操作同一个全局变量。
    3个线程不定时写这个全局变量，5个线程不定时的读这个全局变量
```

* 例子 - 读写锁

```c
//8个线程操作一个全局变量，3个写，5个读
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//全局变量
int num =1;
pthread_rwlock_t rwlock;

//子线程
void* writeNum(void *arg){
    //写区锁
    while (1)
    {
        pthread_rwlock_wrlock(&rwlock);
        num++;
        printf("++write,tid: %ld,num: %d\n",pthread_self(),num);
        pthread_rwlock_unlock(&rwlock);
        usleep(100);        
    }
    return NULL;
}

void* readNum(void *arg){
    //读区锁
    while (1)
    {
        pthread_rwlock_rdlock(&rwlock);
        printf("===read, tid : %ld, num : %d\n", pthread_self(), num);
        pthread_rwlock_unlock(&rwlock);
        usleep(100);        
    }
    return NULL;
}

int main(){
    pthread_rwlock_init(&rwlock,NULL);

    //子进程创建
    pthread_t wtid[3],rtid[5];
    for (int i = 0; i < 3; i++)
    {
        pthread_create(&wtid[i],NULL,writeNum,NULL);
    }
    for (int i = 0; i < 5; i++)
    {
        pthread_create(&rtid[i],NULL,readNum,NULL);
    }
    
    //设置线程分离
     for (int i = 0; i < 3; i++)
    {
        pthread_detach(wtid[i]);
    }
    for (int i = 0; i < 5; i++)
    {
        pthread_detach(rtid[i]);
    }
    //释放锁，主线程退出
    pthread_exit(NULL);

    pthread_rwlock_destroy(&rwlock);

    return 0;
}
```

##### 生产者消费者模型 

```c
//互斥结构-粗略版本的生产消费者模型，可能会导致已放弃 (核心已转储)

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
//全局变量
pthread_mutex_t mutex;
struct Node
{
    int num;
    struct Node *next;
};
    //队列方式读取，实际上是back指针，插入的时候用头插法
struct Node *head = NULL; 

//生产者 不断添加节点
void *producer(void *arg){
    while(1){
        pthread_mutex_lock(&mutex);
        struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
        newNode ->next = head;
        head = newNode;
        newNode ->num = rand()%1000;
        printf("add node, num: %d,tid: %ld\n",newNode->num,pthread_self());
        pthread_mutex_unlock(&mutex);
        usleep(100);
    }
    return NULL;
}
//消费者 获取节点
void *custmer(void *arg){
    while(1){
        //同一块临界区，设置为都互斥
        pthread_mutex_lock(&mutex);
        //保存头结点的指针
        struct Node *tmp = head;
        if(head != NULL){
            printf("del node,num: %d,tid: %ld\n",tmp->num,pthread_self());
            free(tmp);
            pthread_mutex_unlock(&mutex);
            usleep(100);
        }else{
            printf("没了，快做\n");
            pthread_mutex_unlock(&mutex);
            usleep(100);
        }
    }
    return NULL;
}

int main(){
    pthread_mutex_init(&mutex,NULL);
    //5个生产者线程，5个消费者线程
    pthread_t ptid[5],ctid[5];
    for (int i = 0; i < 5; i++)
    {
        pthread_create(&ptid[i],NULL,producer,NULL);
        pthread_create(&ctid[i],NULL,custmer,NULL);
    }
    for (int i = 0; i < 5; i++)
    {
        pthread_detach(ptid[i]);
        pthread_detach(ctid[i]);
    }
    
    //退出主线程，释放锁
    pthread_exit(NULL);

    pthread_mutex_destroy(&mutex);

    return 0;

}
```

* 完整的生产者消费者模型代码 - 固定缓冲区大小

```c
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <semaphore.h>
 
//缓冲区
int *buf;
int bufSize = 100;
int bufPtr;
int count;
//三个信号量
sem_t full, empty, mutex;
 
//生产者线程
void *producer(void *arg)
{
    while (bufPtr < bufSize)
    {
        //信号量模型
        sem_wait(&full);
        sem_wait(&mutex);
        buf[++bufPtr] = bufPtr;
        sem_post(&mutex);
        sem_post(&empty);
    }
}
 
//消费者线程
void *consumer(void *arg)
{
    while (1)
    {
        //信号量模型
        sem_wait(&empty);
        sem_wait(&mutex);
        count = (count + 1) % __INT32_MAX__;
        printf("pid[%ld], count[%d], data[%d]\n", pthread_self(), count, buf[bufPtr--]);
 
        sem_post(&mutex);
        sem_post(&full);
    }
}
int main()
{
    //初始化三个信号量
    sem_init(&full, 0, bufSize);
    sem_init(&empty, 0, 0);
    sem_init(&mutex, 0, 1);
    //初始化读写指针、缓冲区
    bufPtr = -1;
    count = 0;
    buf = (int *)malloc(sizeof(int) * bufSize);
    //创建6个线程，一个作生产者，5个消费者
    pthread_t ppid, cpids[5];
    pthread_create(&ppid, NULL, producer, NULL);
    for (int i = 0; i < 5; ++i)
    {
        pthread_create(&cpids[i], NULL, consumer, NULL);
    }
 
    //detach分离，线程自动回收资源
    pthread_detach(ppid);
    for (int i = 0; i < 5; ++i)
    {
        pthread_detach(cpids[i]);
    }
    //主线程结束
    pthread_exit(NULL);
    return 0;
}
```



##### 条件变量

某个条件满足之后开启或者解除阻塞;

* 《Liunx/UNIX系统编程手册》第531页有句话，条件变量并不保存状态信息，只是传递应用程序状态信息的一种通讯机制。发送信号时若无任何线程在等待该条件变量，这个也就会不了了之。线程如在此后等待该条件变量，只有当再次收到此变量的下一信号时，方可解除阻塞状态。

```c
    条件变量的类型 pthread_cond_t
    int pthread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr);
    int pthread_cond_destroy(pthread_cond_t *cond);
    int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);
        - 等待，调用了该函数，线程会阻塞。//1.这个函数调用阻塞的时候，会对互斥锁mutex解锁；当收到signal或者broadcasr的时候，取消阻塞，这个时候会重新加上锁mutex
    int pthread_cond_timedwait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime);
        - 等待多长时间，调用了这个函数，线程会阻塞，直到指定的时间结束。
    int pthread_cond_signal(pthread_cond_t *cond);
        - //唤醒一个或者多个等待的线程
    int pthread_cond_broadcast(pthread_cond_t *cond);
        - 唤醒//所有的等待的线程
```

* 例子：资源不足的时候，条件变量wait等待生产者，生产者生产后，条件变量signal通知生产者

```c

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
//全局变量
pthread_mutex_t mutex;
    //创建条件变量
pthread_cond_t cond;
struct Node
{
    int num;
    struct Node *next;
};
    //队列方式读取，实际上是back指针，插入的时候用头插法
struct Node *head = NULL; 

//生产者 不断添加节点
void *producer(void *arg){
    while(1){
        pthread_mutex_lock(&mutex);
        struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
        newNode ->next = head;
        head = newNode;
        newNode ->num = rand()%1000;
        printf("add node, num: %d,tid: %ld\n",newNode->num,pthread_self());
        
        //生产一个，就同志消费者，唤醒一个等待的线程
        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex);
        usleep(100);
    }
    return NULL;
}
//消费者 获取节点
void *custmer(void *arg){
    while(1){
        //同一块临界区，设置为都互斥
        pthread_mutex_lock(&mutex);
        //保存头结点的指针
        struct Node *tmp = head;
        if(head != NULL){
            head = head ->next;
            printf("del node,num: %d,tid: %ld\n",tmp->num,pthread_self());
            free(tmp);
            pthread_mutex_unlock(&mutex);
            usleep(100);
        }else{
            /*没有数据，需要等待
            当函数调用阻塞的时候，对互斥锁进行解锁，
            当不阻塞的时候，继续向下执行，会重新加锁
            */
            pthread_cond_wait(&cond,&mutex);
            printf("没了，快做\n");
            pthread_mutex_unlock(&mutex);
            //usleep(100);
        }
    }
    return NULL;
}

int main(){
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    //5个生产者线程，5个消费者线程
    pthread_t ptid[5],ctid[5];
    for (int i = 0; i < 5; i++)
    {
        pthread_create(&ptid[i],NULL,producer,NULL);
        pthread_create(&ctid[i],NULL,custmer,NULL);
    }
    for (int i = 0; i < 5; i++)
    {
        pthread_detach(ptid[i]);
        pthread_detach(ctid[i]);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    //退出主线程
    pthread_exit(NULL);

    return 0;

}
```

##### 信号量

```c
    信号量的类型 sem_t
    #include <semaphore.h>
    int sem_init(sem_t *sem, int pshared, unsigned int value);
        - 初始化信号量
        - 参数： 
            - sem : 信号量变量的地址
            - pshared : 0 用在线程间 ，非0 用在进程间
            - value : 信号量中的值

    int sem_destroy(sem_t *sem);
        - 释放资源

    int sem_wait(sem_t *sem);
        - 对信号量加锁，调用一次对信号量的值-1，如果值为0，就阻塞

    int sem_trywait(sem_t *sem);

    int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
    int sem_post(sem_t *sem);
        - 对信号量解锁，调用一次对信号量的值+1

    int sem_getvalue(sem_t *sem, int *sval);

    sem_t psem;
    sem_t csem;
    init(psem, 0, 8);
    init(csem, 0, 0);

    producer() {
        sem_wait(&psem);
        sem_post(&csem)
    }

    customer() {
        sem_wait(&csem);
        sem_post(&psem)
    }
```

* 例子 - 用信号量实现生产者消费者

```c
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

// 创建一个互斥量
pthread_mutex_t mutex;
// 创建两个信号量
sem_t psem;
sem_t csem;

struct Node{
    int num;
    struct Node *next;
};

// 头结点
struct Node * head = NULL;

void * producer(void * arg) {

    // 不断的创建新的节点，添加到链表中
    while(1) {
        sem_wait(&psem);
        pthread_mutex_lock(&mutex);
        struct Node * newNode = (struct Node *)malloc(sizeof(struct Node));
        newNode->next = head;
        head = newNode;
        newNode->num = rand() % 1000;
        printf("add node, num : %d, tid : %ld\n", newNode->num, pthread_self());
        pthread_mutex_unlock(&mutex);
        sem_post(&csem);
    }

    return NULL;
}

void * customer(void * arg) {

    while(1) {
        sem_wait(&csem);
        pthread_mutex_lock(&mutex);
        // 保存头结点的指针
        struct Node * tmp = head;
        head = head->next;
        printf("del node, num : %d, tid : %ld\n", tmp->num, pthread_self());
        free(tmp);
        pthread_mutex_unlock(&mutex);
        sem_post(&psem);
       
    }
    return  NULL;
}

int main() {

    pthread_mutex_init(&mutex, NULL);
    sem_init(&psem, 0, 8);
    sem_init(&csem, 0, 0);

    // 创建5个生产者线程，和5个消费者线程
    pthread_t ptids[5], ctids[5];

    for(int i = 0; i < 5; i++) {
        pthread_create(&ptids[i], NULL, producer, NULL);
        pthread_create(&ctids[i], NULL, customer, NULL);
    }

    for(int i = 0; i < 5; i++) {
        pthread_detach(ptids[i]);
        pthread_detach(ctids[i]);
    }

    while(1) {
        sleep(10);
    }

    pthread_mutex_destroy(&mutex);

    pthread_exit(NULL);

    return 0;
}
```

## 4.Linux网络编程

### 4.1网络基础

看pdf

### 4.2 socket通信基础

##### socket简介

所谓 socket（套接字），就是对**网络中不同主机上的应用进程之间进行双向通信的端点的抽象**。一个套接字就是网络上进程通信的一端，提供了应用层进程利用网络协议交换数据的机制。从所处的地位来讲，套接字上联应用进程，下联网络协议栈，是应用程序通过网络协议进行通信的接口，是应用程序与网络协议根进行交互的接口。

socket 可以看成是两个网络应用程序进行通信时，各自通信连接中的端点，这是一个逻辑上的概念。它是网络环境中进程间通信的 API，也是可以被命名和寻址的通信端点，使用中的每一个套接字都有其类型和一个与之相连进程。通信时其中一个网络应用程序将要传输的一段信息写入它所在主机的 socket 中，该 socket 通过与网络接口卡（NIC）相连的传输介质将这段信息送到另外一台主机的 socket 中，使对方能够接收到这段信息。**socket 是由 IP 地址和端口结合的，提供向应用层进程传送数据包的机制**。

socket 本身有“插座”的意思，在 Linux 环境下，用于表示进程间网络通信的特殊文件类型。本质为**内核借助缓冲区形成的伪文件**。既然是文件，那么理所当然的，我们可以使用文件描述符引用套接字。与管道类似的，Linux 系统将其封装成文件的目的是为了统一接口，使得读写套接字和读写文件的操作一致。区别是管道主要应用于**本地**进程间通信，而套接字多应用于**网络进程间**数据的传递。

```c
// 套接字通信分两部分：
- 服务器端：被动接受连接，一般不会主动发起连接
- 客户端：主动向服务器发起连接
 
socket是一套通信的接口，Linux 和 Windows 都有，但是有一些细微的差别。
```

##### 字节序

现代 CPU 的**累加器**一次都能装载（至少）4 字节（这里考虑 32 位机），即一个整数。那么这 4字节在**内存中排列的顺序**将影响它被累加器装载成的整数的值，这就是字节序问题。在各种计算机体系结构中，对于**字节、字等的存储机制**有所不同，因而引发了计算机通信领域中一个很重要的问题，即通信双方交流的信息单元（比特、字节、字、双字等等）应该以什么样的顺序进行传送。如
果不达成一致的规则，通信双方将无法进行正确的编码/译码从而导致通信失败。

* **字节序，顾名思义字节的顺序，就是大于一个字节类型的数据在内存中的存放顺序(一个字节的数**
  **据当然就无需谈顺序的问题了)。**
* 字节序分为大端字节序（Big-Endian） 和小端字节序（Little-Endian）。大端字节序是指一个整
  数的最高位字节（23 ~ 31 bit）存储在内存的低地址处，低位字节（0 ~ 7 bit）存储在内存的高地
  址处；小端字节序则是指整数的高位字节存储在内存的高地址处，而低位字节则存储在内存的低地
  址处。

![image-20230301104201183](D:\MyTxt\typoraPhoto\image-20230301104201183.png)

```c
//用union查找字节序
#include <stdio.h>

int main(){
    //共用体占用的内存等于最长的成员占用的内存。共用体使用了内存覆盖技术，同一时刻只能保存一个成员的值，如果对新的成员赋值，就会把原来成员的值覆盖掉。
    union{
        short value;
        char bytes[sizeof(short)];
    }test;

    test.value = 0x0102;
    if((test.bytes[0] == 1)&&(test.bytes[1] == 2)){
        printf("大端字节序\n");
    }else if((test.bytes[0] == 2)&&(test.bytes[1] == 1)){
        printf("小端字节序\n");
    }else{
        printf("未知\n");
    }
    return 0;
}
```

##### 字节序转换函数

* 当格式化的数据在两台使用不同字节序的主机之间直接传递时，接收端必然错误的解释之。
  解决问题的方法是：
  		发送端总是把要发送的数据转换成**大端字节序数据后再发送**，而接收端知道对方传送过来的数据总是采用大端字节序，所以接收端可以根据自身采用的字节序决定是否对接收到的数据进行转换（小端机转换，大端机不转换）。

* **网络字节顺序**是 TCP/IP 中规定好的一种数据表示格式，它**与具体的 CPU 类型、操作系统等无关**，从而可以保证数据在不同主机之间传输时能够被正确解释，网络字节顺序采用大端排序方式。
  BSD Socket提供了封装好的转换接口，方便程序员使用。
  1. 从主机字节序到网络字节序的转换函数：htons、htonl；
  2. 从网络字节序到主机字节序的转换函数：ntohs、ntohl。

```c
h - host 主机，主机字节序
to - 转换成什么
n - network  网络字节序
s - short  unsigned short
l  - long  unsigned int
    
#include <arpa/inet.h>
// 转换端口 因为端口本身是unsigned short类型
uint16_t htons(uint16_t hostshort); // 主机字节序 - 网络字节序
uint16_t ntohs(uint16_t netshort); // 网络字节序 - 主机字节序
// 转IP	因为端口本身是unsigned int类型
uint32_t htonl(uint32_t hostlong); // 主机字节序 - 网络字节序
uint32_t ntohl(uint32_t netlong);  // 网络字节序 - 主机字节序
```

```c
#include <stdio.h>
#include <arpa/inet.h>

int main(){

    //htons 两个字节
    unsigned short a = 0x0102;
    printf("%x\n",a);
    unsigned short b = htons(a);
    printf("%x\n",b);

    //htonl 一般转化ip 四个字节
    char  buf[4] = {196,168,1.100};
    int num = *(int*)buf;
    int sum = htonl(num);
    unsigned char *p = (char*)&sum;
    printf("%d %d %d %d\n",*p,*(p+1),*(p+2),*(p+3));

    return 0;  
}
```

#### socket地址

客户端和服务器通信，需要IP Port ....，所以封装好了一个socket地址。

// socket地址其实是一个结构体，封装端口号和IP等信息。后面的socket相关的api中需要使用到这个socket地址。

##### 通用socket地址

socket 网络编程接口中表示 socket 地址的是结构体 sockaddr，其定义如下：

```c
#include <bits/socket.h>
struct sockaddr {
sa_family_t sa_family;
char     sa_data[14];
};
typedef unsigned short int sa_family_t;
```

1. sa_family 成员是地址族类型（sa_family_t）的变量。地址族类型通常与协议族类型对应。常见的协议
   族（protocol family，也称 domain）和对应的地址族入下所示

<img src="D:\MyTxt\typoraPhoto\image-20230301142702830.png" alt="image-20230301142702830" style="zoom:50%;" />

* 宏 PF\_\* 和 AF\_\* 都定义在 bits/socket.h 头文件中，且后者与前者有完全相同的值，所以二者通常混用。

2. sa_data 成员用于存放 socket 地址值。但是，不同的协议族的地址值具有不同的含义和长度，如下所示：

<img src="D:\MyTxt\typoraPhoto\image-20230301142805645.png" alt="image-20230301142805645" style="zoom:50%;" />

由上表可知，14 字节的 sa_data 根本无法容纳多数协议族的地址值。因此，Linux 定义了下面这个新的通用的 socket 地址结构体，这个结构体不仅提供了足够大的空间用于存放地址值，而且是内存对齐的。

```c
#include <bits/socket.h>
struct sockaddr_storage
{
sa_family_t sa_family;
unsigned long int __ss_align;
char __ss_padding[ 128 - sizeof(__ss_align) ];
};
typedef unsigned short int sa_family_t;
```

##### 专用socket地址

很多网络编程函数诞生早于 IPv4 协议，那时候都使用的是 struct sockaddr 结构体，为了向前兼容，现在sockaddr **退化成了（void *）**的作用，传递一个地址给函数，至于这个函数是 sockaddr_in 还是sockaddr_in6，由地址族确定，然后函数内部再**强制类型转化为所需的地址类型**。

<img src="D:\MyTxt\typoraPhoto\image-20230301143653912.png" alt="image-20230301143653912" style="zoom:50%;" />

UNIX 本地域协议族使用如下专用的 socket 地址结构体：

```c
#include <sys/un.h>
struct sockaddr_un
{
sa_family_t sin_family;
char sun_path[108];
}
```

TCP/IP 协议族有 sockaddr_in 和 sockaddr_in6 两个专用的 socket 地址结构体，它们分别用于 IPv4 和IPv6

```c
#include <netinet/in.h>
struct sockaddr_in//这个用较多
{
  sa_family_t sin_family; /* __SOCKADDR_COMMON(sin_) */ //这个用较多
  in_port_t sin_port;     /* Port number. */
  struct in_addr sin_addr;   /* Internet address. */  //这个用较多
  /* Pad to size of `struct sockaddr'. */
  unsigned char sin_zero[sizeof (struct sockaddr) - __SOCKADDR_COMMON_SIZE -
       sizeof (in_port_t) - sizeof (struct in_addr)];
}; 
socklen_t 是sockaddr_in的长度类型
struct in_addr
{
  in_addr_t s_addr;
};
struct sockaddr_in6
{
  sa_family_t sin6_family;
  in_port_t sin6_port; /* Transport layer port # */
  uint32_t sin6_flowinfo; /* IPv6 flow information */
  struct in6_addr sin6_addr; /* IPv6 address */
  uint32_t sin6_scope_id; /* IPv6 scope-id */
};
typedef unsigned short  uint16_t;
typedef unsigned int   uint32_t;
typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;
#define __SOCKADDR_COMMON_SIZE (sizeof (unsigned short int))
```

* 所有**专用** socket 地址（以及 sockaddr_storage）类型的变量在实际使用时都需要**转化为通用** socket 地址类型 sockaddr（强制转化即可），因为所有 socket 编程接口使用的地址参数类型都是 sockaddr。

### 4.3 IP地址转换

字符串ip-整数 ，主机-网络字节序的转换

通常，人们习惯用可读性好的字符串来表示 IP 地址，比如用**点分十进制字符串**表示 IPv4 地址，以及用**十六进制字符串**表示 IPv6 地址。但编程中我们需要先把它们**转化为整数（二进制数）方能使用**。而记录日志时则相反，我们要把整数表示的 IP 地址转化为可读的字符串。下面 3 个函数可用于用点分十进制字符串表示的 IPv4 地址和用网络字节序整数表示的 IPv4 地址之间的转换：

* 旧接口- 以下函数比较久，且只使用于IPv4，而且是不可重用的函数，比如第三个因为直接用了那个结构体变量

```c
//以下函数比较久，且只使用于IPv4，而且是不可重用的函数，比如第三个因为直接用了那个结构体变量
#include <arpa/inet.h>
//转换成整数
in_addr_t inet_addr(const char *cp);
//转换成整数，保存在结构体in_addr中，成功返回1，失败返回0
int inet_aton(const char *cp, struct in_addr *inp);
//网络字节序的证书转换成点分十进制的字符串
char *inet_ntoa(struct in_addr in);
```

* 新接口-下面这对更新的函数也能完成前面 3 个函数同样的功能，并且它们同时适用 IPv4 地址和 IPv6 地址：

```c
//下面这对更新的函数也能完成前面 3 个函数同样的功能，并且它们同时适用 IPv4 地址和 IPv6 地址
#include <arpa/inet.h>
// p:点分十进制的IP字符串，n:表示network，网络字节序的整数
int inet_pton(int af, const char *src, void *dst);
af:地址族： AF_INET  AF_INET6
  src:需要转换的点分十进制的IP字符串
  dst:转换后的结果保存在这个里面
// 将网络字节序的整数，转换成点分十进制的IP地址字符串
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
af:地址族： AF_INET  AF_INET6
  src: 要转换的ip的整数的地址
  dst: 转换成IP地址字符串保存的地方
  size：第三个参数的大小//（数组的大小）
返回值：返回转换后的数据的地址（字符串），和 dst 是一样的
```

* IP地址转换例子

```c
#include <stdio.h>
#include <arpa/inet.h>

int main(){
    //创建点分十进制ip地址字符串
    char buf[] = "192.163.1.4";
    unsigned int num = 0;
    
    //ipv4字符串转化网络字节序的整数
    inet_pton(AF_INET,buf,&num);
        //输出
    unsigned char *p = (unsigned char*)&num;
    printf("%d %d %d %d\n",*p,*(p+1),*(p+2),*(p+3));

    //网络字节序的整数转化为IPv4字符串
        //15个字符加上一个字符串结束符
    char ip[16] = "";
    //这里获取后，使用str和ip其实是一样	
    const char *str = inet_ntop(AF_INET,&num,ip,16);
    printf("str:%s\n",str);
    printf("ip:%s\n",ip);
    printf("%d\n",ip == str);
    return 0;
}   
```

### 4.4TCP通信流程

```c
// TCP 和 UDP -> 传输层的协议
UDP:用户数据报协议，面向无连接，可以单播，多播，广播， 面向数据报，不可靠
TCP:传输控制协议，面向连接的，可靠的，基于字节流，仅支持单播传输
 
              UDP   TCP
是否创建连接 无连接 ||面向连接
是否可靠  不可靠 ||可靠的
连接的对象个数   一对一、一对多、多对一、多对多 ||支持一对一
传输的方式 面向数据报  ||面向字节流
首部开销 8个字节 ||最少20个字节
适用场景 实时应用（视频会议，直播）  ||可靠性高的应用（文件传输）
```

<img src="D:\MyTxt\typoraPhoto\image-20230301155418481.png" alt="image-20230301155418481" style="zoom: 67%;" />

##### TCP通信流程

```c
// TCP 通信的流程
// 服务器端 （被动接受连接的角色）
1. 创建一个用于监听的套接字
  - 监听：监听有客户端的连接
  - 套接字：这个套接字其实就是一个文件描述符
2. 将这个监听文件描述符和本地的IP和端口绑定（IP和端口就是服务器的地址信息）
  - 客户端连接服务器的时候使用的就是这个IP和端口
3. 设置监听，监听的fd开始工作
4. 阻塞等待，当有客户端发起连接，解除阻塞，接受客户端的连接，会得到一个和客户端通信的套接字
（fd）
5. 通信
 - 接收数据
 - 发送数据
6. 通信结束，断开连接
```

```c
// 客户端
1. 创建一个用于通信的套接字（fd）
2. 连接服务器，需要指定连接的服务器的 IP 和 端口
3. 连接成功了，客户端可以直接和服务器通信
  - 接收数据
  - 发送数据
4. 通信结束，断开连接
```

### 4.4套接字函数

```c
#include <sys/types.h>   
#include <sys/socket.h>
#include <arpa/inet.h> // 包含了这个头文件，上面两个就可以省略
int socket(int domain, int type, int protocol);
- 功能：创建一个套接字	
  - 参数：
    - domain: 协议族
   AF_INET : ipv4
   AF_INET6 : ipv6
   AF_UNIX, AF_LOCAL : 本地套接字通信（进程间通信）
    - type: 通信过程中使用的协议类型
      SOCK_STREAM : 流式协议//TC一般 用这个
      SOCK_DGRAM : 报式协议//UDP等
    - protocol : 具体的一个协议。一般写0
      - SOCK_STREAM : 流式协议默认使用 TCP
      - SOCK_DGRAM : 报式协议默认使用 UDP
    - 返回值：
      - 成功：返回文件描述符，操作的就是内核缓冲区。
      - 失败：-1    
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); // socket命名
- 功能：绑定，将fd 和本地的IP + 端口进行绑定 //也就是讲文件描述符告诉连接方
  - 参数：
   - sockfd : 通过socket函数得到的文件描述符
      - addr : 需要绑定的socket地址，这个地址封装了ip和端口号的信息
      - addrlen : 第二个参数结构体占的内存大小
 
int listen(int sockfd, int backlog); // cat /proc/sys/net/core/somaxconn可以查看，本机是4096
- 功能：监听这个socket上的连接
  - 参数：
    - sockfd : 通过socket()函数得到的文件描述符
    - backlog : 未连接的和已经连接的和的最大值， //一般给个 5
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
- 功能：接收客户端连接，默认是一个阻塞的函数，阻塞等待客户端连接
  - 参数：
   - sockfd : 用于监听的文件描述符
      - addr : 传出参数，记录了连接成功后客户端的地址信息（ip，port）
      - addrlen : 指定第二个参数的对应的内存大小
  - 返回值：
      - 成功 ：用于通信的文件描述符
      - -1 ： 失败
       
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
- 功能： 客户端连接服务器
  - 参数：
   - sockfd : 用于通信的文件描述符
      - addr : 客户端要连接的服务器的地址信息
      - addrlen : 第二个参数的内存大小
  - 返回值：成功 0， 失败 -1
ssize_t write(int fd, const void *buf, size_t count); // 写数据
ssize_t read(int fd, void *buf, size_t count); // 读数据
```

### 4.5 TCP通信并发实现

##### TCP通信 单连

1. 服务器端设置端口999；客户端的端口是随机分配的，只要去连接服务器端的999就行
2. 服务器端要去连的IP是任意的；客户端要去连接的IP是服务器端的192.168.31.128

###### 服务器端

```c
/*服务器端两个文件描述符
1. 监听文件描述符lfd，相当于创建socket分配了，只不过后续要绑定到struct sockaddr_in saddr结构体（ip，port）上；
2.连接文件描述符cfd,在accept之后分配，只不过针对当前客户端的fd
*/
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main(){
    //1.创建socket用于监听
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1){
        perror("socket");
        exit(-1);//exit(0):正常运行程序并退出程序; exit(1):非正常运行导致退出程序,exit（-1）：非正常运行导致退出程序，与1类似；
    }

    //2.绑定文件描述符
        //socket地址结构体
    struct sockaddr_in saddr; 
        //初始化协议族
    saddr.sin_family = AF_INET;
    //方法一：设置ip，保持网络字节序；这儿注意一下结构体嵌套，最后存的变量
    //inet_pton(AF_INET,"192.168.31.128",saddr.sin_addr.s_addr);
    //方法二：直接写in_addr_t类型的变量算了；INADDR_ANY是linux下的任博 
    	//设置ip号
    saddr.sin_addr.s_addr = INADDR_ANY; //0.0.0.0
        //设置端口号 
    saddr.sin_port = htons(9999);
    int ret = bind(lfd,(struct sockaddr *)&saddr,sizeof(saddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }

    //3.设置监听
    ret = listen(lfd,8);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }

    //4，接受客户端连接 - 阻塞
    struct sockaddr_in clientaddr;
    int len = sizeof(clientaddr);
    int cfd = accept(lfd,(struct sockaddr *)&clientaddr,&len);
    if(cfd == -1){
        perror("accept");
        exit(-1);
    }

    //输出客户端数据
    char clientIP[16];
    //获取IP，网络字节序转化成点分十进制
    inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,clientIP,sizeof(clientIP));
    //获取端口
    unsigned short clientPort = ntohs(clientaddr.sin_port);
    printf("client ip is %s, port is %d\n", clientIP, clientPort);
    //5.通信
    char recvBuf[1024] = {0};
    while(1){
    //服务器接受客户端数据
        //cfd是客户端socket发过来的文件描述符
        int num = read(cfd,recvBuf,sizeof(recvBuf));
        if(ret == -1){
            perror("read");
            exit(-1);
        }else if(num > 0){
            //接受了num个字节   
            printf("recv client data: %s\n",recvBuf);
        }else if(num == 0){
            //接受完了
            printf("client closed...");
            break;
        }

    //服务器回复客户端数据
    char* data = "连上了，我是服务器端";
        write(cfd,data,strlen(data));
    }

    //6.断开关闭文件描述符
    close(cfd);
    close(lfd);

    return 0;
}
```

###### 客户端

```c
//客户端用自己socket生成的fd写读，并且把fd发给服务器端，服务器端用针对该客户端的cfd		
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(){
    //1.创建套接字
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }
    //2.连接服务器端
    struct sockaddr_in serveraddr;
        //设置协议族
    serveraddr.sin_family = AF_INET;
        //设置ip
    inet_pton(AF_INET,"192.168.31.128",&serveraddr.sin_addr.s_addr);
        //设置端口 
    serveraddr.sin_port = htons(9999);
        //connect
    int ret = connect(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    if(ret == -1){
        perror("connect");
        exit(-1);
    }
    //3.通信
    char recvBuf[1024]={0};
    while(1){
        char *data = "这是客户端";
        write(fd,data,strlen(data));
        sleep(1);

        int len = read(fd,recvBuf,sizeof(recvBuf)+1);
        if(len == -1){
            perror("read");
            exit(-1);
        }else if(len > 0){
            printf("recv server data:%s\n",recvBuf);
        }else if(len == 0){
            printf("server closed...\n");
            break;
        }
    }

    //关闭文件描述符，退出
    close(fd);    
    return 0;
}
```

###### 小作业-读取输入并且回射

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
    //1.创建socket用于监听
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1){
        perror("socket");
        exit(-1);//exit(0):正常运行程序并退出程序; exit(1):非正常运行导致退出程序,exit（-1）：非正常运行导致退出程序，与1类似；
    }

    //2.绑定文件描述符
        //socket地址结构体
    struct sockaddr_in saddr;
        //初始化协议族
    saddr.sin_family = AF_INET;
    //方法一：设置ip，保持网络字节序；这儿注意一下结构体嵌套，最后存的变量
    //inet_pton(AF_INET,"192.168.31.128",saddr.sin_addr.s_addr);
    //方法二：直接写in_addr_t类型的变量算了；INADDR_ANY是linux下的任博
    saddr.sin_addr.s_addr = INADDR_ANY; //0.0.0.0
        //设置端口号 
    saddr.sin_port = htons(9988);
    int ret = bind(lfd,(struct sockaddr *)&saddr,sizeof(saddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }

    //3.设置监听
    ret = listen(lfd,8);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }

    //4，接受客户端连接 - 阻塞
    struct sockaddr_in clientaddr;
    int len = sizeof(clientaddr);
    int cfd = accept(lfd,(struct sockaddr *)&clientaddr,&len);
    if(cfd == -1){
        perror("accept");
        exit(-1);
    }

    //输出客户端数据
    char clientIP[16];
    //获取IP，网络字节序转化成点分十进制
    inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,clientIP,sizeof(clientIP));
    //获取端口
    unsigned short clientPort = ntohs(clientaddr.sin_port);
    printf("client ip is %s, port is %d\n", clientIP, clientPort);
    //5.通信
    char recvBuf[1024] = {0};
    while(1){
    //服务器接受客户端数据
        //cfd是客户端socket发过来的文件描述符
        int num = read(cfd,recvBuf,sizeof(recvBuf));
        if(ret == -1){
            perror("read");
            exit(-1);
        }else if(num > 0){
            //接受了num个字节   
            printf("recv client data: %s\n",recvBuf);
        }else if(num == 0){
            //接受完了
            printf("client closed...");
            break;
        }

    //服务器回复客户端数据
    //char* data = "连上了，我是服务器端";
        // char data[1024];
        // scanf("%s",data);
        // printf("请输入\n");
        write(cfd,recvBuf,strlen(recvBuf));
    }

    //6.断开关闭文件描述符
    close(cfd);
    close(lfd);

    return 0;
}
```

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(){
    //1.创建套接字
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }
    //2.连接服务器端
    struct sockaddr_in serveraddr;
        //设置协议族
    serveraddr.sin_family = AF_INET;
        //设置ip
    inet_pton(AF_INET,"192.168.31.128",&serveraddr.sin_addr.s_addr);
        //设置端口 
    serveraddr.sin_port = htons(9988);
        //connect
    int ret = connect(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    if(ret == -1){
        perror("connect");
        exit(-1);
    }
    //3.通信
    char recvBuf[1024]={0};
    while(1){
        //申请内存并且清空
        char data[1024];
        memset(data,0,sizeof(data));
        //输入
        printf("请输入\n");
        scanf("%s",data);
        write(fd,data,strlen(data));
        sleep(1);

        int len = read(fd,recvBuf,sizeof(recvBuf)+1);
        if(len == -1){
            perror("read");
            exit(-1);
        }else if(len > 0){
            printf("recv server data:%s\n",recvBuf);
        }else if(len == 0){
            printf("server closed...\n");
            break;
        }
    }

    //关闭文件描述符，退出
    close(fd);    
    return 0;
}
```



##### 三次握手

TCP 是一种面向连接的单播协议，在发送数据前，通信双方必须在彼此间建立一条连接。所谓的“连接”，其实是**客户端和服务器的内存里保存的一份关于对方的信息，如 IP 地址、端口号等**。

TCP 可以看成是一种字节流，它会处理 IP 层或以下的层的丢包、重复以及错误问题。在连接的建立过程中，双方需要交换一些连接的参数。这些参数可以放在 TCP 头部。

TCP 提供了一种可靠、面向连接、字节流、传输层的服务，采用三次握手建立一个连接。采用四次挥手
来关闭一个连接。
三次握手的目的是保证双方互相之间建立了连接。
三次握手发生在客户端连接的时候，当调用connect()，底层会通过TCP协议进行三次握手

###### TCP报文头

<img src="D:\MyTxt\typoraPhoto\image-20230302114709452.png" alt="image-20230302114709452" style="zoom:50%;" />

* 16 位端口号（port number）：告知主机报文段是来自哪里（源端口）以及传给哪个上层协议或
  应用程序（目的端口）的。进行 TCP 通信时，客户端通常使用系统自动选择的临时端口号
* 32 位序号（sequence number）：一次 TCP 通信（从 TCP 连接建立到断开）过程中某一个传输
  方向上的字节流的每个字节的编号。假设主机 A 和主机 B 进行 TCP 通信，A 发送给 B 的第一个
  TCP 报文段中，**序号值被系统初始化为某个随机值 ISN**（Initial Sequence Number，初始序号
  值）。那么在该传输方向上（从 A 到 B），后续的 TCP 报文段中序号值将被系统设置成 ISN 加上
  该报文段所携带数据的第一个字节在整个字节流中的**偏移**。例如，某个 TCP 报文段传送的数据是字
  节流中的第 1025 ~ 2048 字节，那么该报文段的序号值就是 ISN + 1025。另外一个传输方向（从
  B 到 A）的 TCP 报文段的序号值也具有相同的含义
* 32 位确认号（acknowledgement number）：用作对另一方发送来的 TCP 报文段的响应。其值是
  收到的 **TCP 报文段的序号值 + 标志位长度（SYN，FIN） + 数据长度** 。假设主机 A 和主机 B 进行
  TCP 通信，那么 A 发送出的 TCP 报文段不仅携带自己的序号，而且包含对 B 发送来的 TCP 报文段
  的确认号。反之，B 发送出的 TCP 报文段也同样携带自己的序号和对 A 发送来的报文段的确认序号。
* 6 位标志位包含如下几项：
  1. URG 标志，表示紧急指针（urgent pointer）是否有效。
  2. ACK 标志，表示确认号是否有效。我们称携带 ACK 标志的 TCP 报文段为确认报文段。
  3. PSH 标志，提示接收端应用程序应该立即从 TCP 接收缓冲区中读走数据，为接收后续数据腾
     出空间（如果应用程序不将接收到的数据读走，它们就会一直停留在 TCP 接收缓冲区中）。
  4. RST 标志，表示要求对方重新建立连接。我们称携带 RST 标志的 TCP 报文段为复位报文段。
  5. SYN 标志，表示请求建立一个连接。我们称携带 SYN 标志的 TCP 报文段为同步报文段。
  6. FIN 标志，表示通知对方本端要关闭连接了。我们称携带 FIN 标志的 TCP 报文段为结束报文
     段。
* 16 位窗口大小（window size）：是 TCP 流量控制的一个手段。这里说的窗口，指的是**接收**
  **通告窗口**（Receiver Window，RWND）。它告诉对方**本端的 TCP 接收缓冲区还能容纳多少**
  **字节的数据**，这样对方就可以控制发送数据的速度
* 16 位校验和（TCP checksum）：由发送端填充，接收端对 TCP 报文段执行 CRC 算法以校验
  TCP 报文段在传输过程中是否损坏。注意，这个校验不仅包括 TCP 头部，也包括数据部分。
  这也是 TCP 可靠传输的一个重要保障
* 16 位紧急指针（urgent pointer）：是一个正的偏移量。它和序号字段的值相加表示最后一
  个紧急数据的下一个字节的序号。因此，确切地说，这个字段是紧急指针相对当前序号的偏
  移，不妨称之为紧急偏移。TCP 的紧急指针是发送端向接收端发送紧急数据的方法。

###### 三次握手连接过程

<img src="D:\MyTxt\typoraPhoto\image-20230302135058522.png" alt="image-20230302135058522" style="zoom:50%;" />

```c
第一次握手：
  1.客户端将SYN标志位置为1
  2.生成一个随机的32位的序号seq=J ， 这个序号后边第三次握手的时候是可以携带数据（数据的大小）
第二次握手：
  1.服务器端接收客户端的连接： ACK=1
  2.服务器会回发一个确认序号： ack=客户端的序号 + 数据长度 + SYN/FIN(按一个字节算)
  3.服务器端会向客户端发起连接请求： SYN=1
  4.服务器会生成一个随机序号：seq = K
第三次握手：
  1.客户单应答服务器的连接请求：ACK=1
  2.客户端回复收到了服务器端的数据：ack=服务端的序号 + 数据长度 + SYN/FIN(按一个字节算)
```

##### TCP滑动窗口

滑动窗口（Sliding window）是一种流量控制技术。早期的网络通信中，通信双方不会考虑网络的
拥挤情况直接发送数据。由于大家不知道网络拥塞状况，同时发送数据，导致中**间节点阻塞掉包**，
谁也发不了数据，所以就有了滑动窗口机制来解决此问题。滑动窗口协议是用来改善吞吐量的一种
技术，即**容许发送方在接收任何应答之前传送附加的包。接收方告诉发送方在某一时刻能送多少包**
**（称窗口尺寸）。**
TCP 中采用滑动窗口来进行传输控制，**滑动窗口的大小意味着接收方还有多大的缓冲区可以用于**
**接收数据**。发送方可以通过滑动窗口的大小来确定应该发送多少字节的数据。当滑动窗口为 0
时，发送方一般不能再发送数据报。

* 滑动窗口是 TCP 中实现诸如 ACK 确认、流量控制、拥塞控制的承载结构

窗口理解为缓冲区的大小
滑动窗口的大小会随着发送数据和接收数据而变化。
通信的双方都有发送缓冲区和接收数据的缓冲区
服务器：
发送缓冲区（发送缓冲区的窗口）
接收缓冲区（接收缓冲区的窗口）
客户端
发送缓冲区（发送缓冲区的窗口）
接收缓冲区（接收缓冲区的窗口）

![image-20230302142113013](D:\MyTxt\typoraPhoto\image-20230302142113013.png)

```c
发送方的缓冲区：
  白色格子：空闲的空间
  灰色格子：数据已经被发送出去了，但是还没有被接收
  紫色格子：还没有发送出去的数据
接收方的缓冲区：
  白色格子：空闲的空间
  紫色格子：已经接收到的数据
```

![image-20230302142141901](D:\MyTxt\typoraPhoto\image-20230302142141901.png)

```c
# mss: Maximum Segment Size(一条数据的最大的数据量)
# win: 滑动窗口
1. 客户端向服务器发起连接，客户单的滑动窗口是4096，一次发送的最大数据量是1460
2. 服务器接收连接情况，告诉客户端服务器的窗口大小是6144，一次发送的最大数据量是1024
3. 第三次握手
4. 4-9 客户端连续给服务器发送了6k的数据，每次发送1k
5. 第10次，服务器告诉客户端：发送的6k数据以及接收到，存储在缓冲区中，缓冲区数据已经处理了2k,窗
口大小是2k
6. 第11次，服务器告诉客户端：发送的6k数据以及接收到，存储在缓冲区中，缓冲区数据已经处理了4k,窗
口大小是4k
7. 第12次，客户端给服务器发送了1k的数据
8. 第13次，客户端主动请求和服务器断开连接，并且给服务器发送了1k的数据
9. 第14次，服务器回复ACK 8194, a:同意断开连接的请求 b:告诉客户端已经接受到方才发的2k的数据
c:滑动窗口2k
10.第15、16次，通知客户端滑动窗口的大小
11.第17次，第三次挥手，服务器端给客户端发送FIN,请求断开连接
12.第18次，第四次挥手，客户端同意了服务器端的断开请求
```



##### 四次挥手

四次挥手发生在断开连接的时候，在程序中当调用了close()会使用TCP协议进行四次挥手。
客户端和服务器端都可以主动发起断开连接，谁先调用close()谁就是发起。
因为在TCP连接的时候，采用三次握手建立的的连接是双向的，在断开的时候需要双向断开。

<img src="D:\MyTxt\typoraPhoto\image-20230302143729767.png" alt="image-20230302143729767" style="zoom:50%;" />

##### 多进程实现并发服务器

```c
要实现TCP通信服务器处理并发的任务，使用多线程或者多进程来解决。
 
思路：
  1. 一个父进程，多个子进程
  2.父进程负责等待并接受客户端的连接
  3.子进程：完成通信，接受一个客户端连接，就创建一个子进程用于通信
```

###### server_process

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <wait.h>

void recycleChild(int arg){
    while(1){
        int ret = waitpid(-1,NULL,WNOHANG);
        if(ret == -1){
            //子进程回收完
            break;
        }else if(ret == 0){
            //还有子进程活着
            break;
        }else if(ret >0 ){
            //回收该子进程完成
            printf("子进程%d被回收了\n",ret);
        }
    }
}
int main(){

    //用SIGCHLD回收子进程
    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = recycleChild;
    //注册信号捕捉
    sigaction(SIGCHLD,&act,NULL);

    //1.创建socket用于监听
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }

    //2.绑定文件描述符
        //socket地址结构体
    struct sockaddr_in saddr;
        //初始化协议族
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY; //0.0.0.0
        //设置端口号 
    saddr.sin_port = htons(9898);
    int ret = bind(lfd,(struct sockaddr *)&saddr,sizeof(saddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }

    //3.设置监听
    ret = listen(lfd,128);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }

    //4.不停处理新客户端的链接

    while(1){
        struct sockaddr_in clientaddr;
        int len = sizeof(clientaddr);
        int cfd = accept(lfd,(struct sockaddr *)&clientaddr,&len);
        if(cfd == -1){
            perror("accept");
            exit(-1);
        }

        //每连接上一个客户端，创建一个子进程处理
        pid_t pid = fork();
        if(pid == 0){
        //子进程    
            //获取客户端信息
            char clientIP[16];
            //获取IP，网络字节序转化成点分十进制
            inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,clientIP,sizeof(clientIP));
            //获取端口
            unsigned short clientPort = ntohs(clientaddr.sin_port);
            printf("client ip is %s, port is %d\n", clientIP, clientPort);
            
        //不停读写
            //服务器读取客户端数据
            char recvBuf[1024];
            while(1){
                int num = read(cfd,recvBuf,sizeof(recvBuf));
                if(ret == -1){
                    perror("read");
                    exit(-1);
                }else if(num > 0){
                    //接受了num个字节   
                    printf("recv client data: %s\n",recvBuf);
                }else if(num == 0){
                    //接受完了
                    printf("client closed...\n");
                    break;
                }
                //服务器回复客户端数据
                write(cfd,recvBuf,strlen(recvBuf)+1);
            }
            //子进程退出，等待回收 多进程别忘了回收子进程资源！！！！
            close(cfd);
            exit(0);
        }
    }
    close(lfd);

    return 0;
}
```

###### client

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(){
    //1.创建套接字
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }
    //2.连接服务器端
    struct sockaddr_in serveraddr;
        //设置协议族
    serveraddr.sin_family = AF_INET;
        //设置ip
    inet_pton(AF_INET,"192.168.31.128",&serveraddr.sin_addr.s_addr);
        //设置端口 
    serveraddr.sin_port = htons(9898);
        //connect
    int ret = connect(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    if(ret == -1){
        perror("connect");
        exit(-1);
    }
    //3.通信
    char recvBuf[1024];
    int i = 0;
    while(1){
        sprintf(recvBuf,"recvBuf: %d\n",i++);
        //输入 把\0也写入
        write(fd,recvBuf,strlen(recvBuf)+1);
        
        int len = read(fd,recvBuf,sizeof(recvBuf)+1);
        if(len == -1){
            perror("read");
            exit(-1);
        }else if(len > 0){
            printf("recv server data:%s\n",recvBuf);
        }else if(len == 0){
            printf("server closed...\n");
            break;
        }
        sleep(1);
    }

    //关闭文件描述符，退出
    close(fd);    
    return 0;
}
```

##### 多线程实现并发服务器 - 类似于线程池

###### server_process

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//传到选定子线程的数据结构体
struct sockInfo
{
    int fd;
    struct sockaddr_in addr;
    pthread_t tid;
};
struct sockInfo sockinfos[128];


//子线程通信
void* working(void* arg){
    struct sockInfo *pinfo = (struct sockInfo *)arg;
    //不停读写
        //服务器读取客户端数据
        char recvBuf[1024];
        while(1){
            int num = read(pinfo->fd,recvBuf,sizeof(recvBuf));
            if(num == -1){
                perror("read");
                exit(-1);
            }else if(num > 0){
                //接受了num个字节   
                printf("recv client data: %s\n",recvBuf);
            }else if(num == 0){
                //接受完了
                printf("client closed...\n");
                break;
            }
            //服务器回复客户端数据
            write(pinfo->fd,recvBuf,strlen(recvBuf)+1);
        }
        //子线程退出，记得分离！！！！！
        close(pinfo->fd);
        return NULL;   
}

int main(){
     //1.创建socket用于监听
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }

    //2.绑定文件描述符
        //socket地址结构体
    struct sockaddr_in saddr;
        //初始化协议族
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY; //0.0.0.0
        //设置端口号 
    saddr.sin_port = htons(9898);
    int ret = bind(lfd,(struct sockaddr *)&saddr,sizeof(saddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }

    //3.设置监听
    ret = listen(lfd,128);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }
    //4.连接
        //初始化数据
        int max = sizeof(sockinfos)/sizeof(sockinfos[0]);
        for(int i=0;i<max;i++){
            //清空数据       The  bzero()  function erases the data in the n bytes of the memory starting at the location pointed to by s, by writing zeros (bytes containing '\0') to that area.
            bzero(&sockinfos[i],sizeof(sockinfos[i]));
            //设置初始值
            sockinfos[i].fd=-1;
            sockinfos[i].tid=-1;
        }
    while(1){
        struct sockaddr_in cliaddr;
        int len = sizeof(cliaddr);
        int cfd = accept(lfd,(struct sockaddr*)&cliaddr,&len);

        //从线程数组中找到可用的子线程，进行连接通信
        struct sockInfo *pinfo;
        for(int i=0;i<max;i++){
            if(sockinfos[i].tid == -1){
                pinfo = &sockinfos[i];
                break;
            }
            //没有空闲线程,等会，重新找
            if(i == max -1){
                sleep(1);
                i=-1;
            }
        }
        pinfo->fd = cfd;
        memcpy(&pinfo->addr,&cliaddr,len);

        //创建子进程
        pthread_create(&pinfo->tid,NULL,working,pinfo);
        //子线程分离
        pthread_detach(pinfo->tid);
    }
    
    //退出
    close(lfd);
    return 0;
}
```

###### client - 和前面一样

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(){
    //1.创建套接字
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }
    //2.连接服务器端
    struct sockaddr_in serveraddr;
        //设置协议族
    serveraddr.sin_family = AF_INET;
        //设置ip
    inet_pton(AF_INET,"192.168.31.128",&serveraddr.sin_addr.s_addr);
        //设置端口 
    serveraddr.sin_port = htons(9898);
        //connect
    int ret = connect(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    if(ret == -1){
        perror("connect");
        exit(-1);
    }
    //3.通信
    char recvBuf[1024];
    int i = 0;
    while(1){
        sprintf(recvBuf,"recvBuf: %d\n",i++);
        //输入 把\0也写入
        write(fd,recvBuf,strlen(recvBuf)+1);
        
        int len = read(fd,recvBuf,sizeof(recvBuf)+1);
        if(len == -1){
            perror("read");
            exit(-1);
        }else if(len > 0){
            printf("recv server data:%s\n",recvBuf);
        }else if(len == 0){
            printf("server closed...\n");
            break;
        }
        sleep(1);
    }

    //关闭文件描述符，退出
    close(fd);    
    return 0;
}
```

##### TCP状态转换

<img src="D:\MyTxt\typoraPhoto\image-20230302175535303.png" alt="image-20230302175535303" style="zoom: 80%;" />

<img src="D:\MyTxt\typoraPhoto\image-20230302175650175.png" alt="image-20230302175650175" style="zoom:67%;" />

* 上图红线是发送方，虚线是接收方

2MSL（Maximum Segment Lifetime）
主动断开连接的一方, 最后进入一个 TIME_WAIT状态, 这个状态会持续: 2msl

* msl: 官方建议: 2分钟, 实际是30s

```c
当 TCP 连接主动关闭方接收到被动关闭方发送的 FIN 和最终的 ACK 后，连接的主动关闭方
必须处于TIME_WAIT 状态并持续 2MSL 时间。
这样就能够让 TCP 连接的主动关闭方在它发送的 ACK 丢失的情况下重新发送最终的 ACK。
主动关闭方重新发送的最终 ACK 并不是因为被动关闭方重传了 ACK（它们并不消耗序列号，
被动关闭方也不会重传），而是因为被动关闭方重传了它的 FIN。//事实上，被动关闭方总是重传 FIN 直到它收到一个最终的 ACK。
    //为什么不需要第五次挥手？如果被动关闭方没有接受到ACK，肯定对继续传送FIN，这时候主动方就知道第四次的ACK没穿送到位
```

* 半关闭

```c
当 TCP 链接中 A 向 B 发送 FIN 请求关闭，另一端 B 回应 ACK 之后（A 端进入 FIN_WAIT_2
状态），并没有立即发送 FIN 给 A，A 方处于半连接状态（半开关），此时 A 可以接收 B 发
送的数据，但是 A 已经不能再向 B 发送数据。//为什么四次：第二次和第三次需要分开 ！服务端收到客户端的 FIN 报文时，先回一个 ACK 应答报文，而服务端可能还有数据需要处理和发送，等服务端不再发送数据时，才发送 FIN 报文给客户端来表示同意现在关闭连接。
```

* 从程序的角度，可以使用 API 来控制实现**半连接状态**:

```c
//区别于close()
#include <sys/socket.h>
int shutdown(int sockfd, int how);
sockfd: 需要关闭的socket的描述符
how: 允许为shutdown操作选择以下几种方式:
SHUT_RD(0)： 关闭sockfd上的读功能，此选项将不允许sockfd进行读操作。
   该套接字不再接收数据，任何当前在套接字接受缓冲区的数据将被无声的丢弃掉。
SHUT_WR(1): 关闭sockfd的写功能，此选项将不允许sockfd进行写操作。进程不能在对此套接字发
出写操作。
SHUT_RDWR(2):关闭sockfd的读写功能。相当于调用shutdown两次：首先是以SHUT_RD,然后以
SHUT_WR。
```

使用 close 中止一个连接，但它只是**减少描述符的引用计数**，并不直接关闭连接，只有当描述符的引用
计数为 0 时才关闭连接。shutdown 不考虑描述符的引用计数，**直接关闭描述符**。也可选择中止一个方
向的连接，只中止读或只中止写。
注意:

1. 如果有多个进程共享一个套接字，close 每被调用一次，计数减 1 ，直到计数为 0 时，也就是所用
    进程都调用了 close，套接字将被释放。
2. 在多进程中如果一个进程调用了 shutdown(sfd, SHUT_RDWR) 后，其它的进程将无法进行通信。
    但如果一个进程 close(sfd) 将不会影响到其它进程。

##### 端口复用

* 在不调用端口复用的时候， 如果服务器端处于time_wait状态，就会占用当前端口号，直到释放掉。

  比如，如果重启服务器的话，就会显示地址已经使用，导致不能重启

* 端口复用最常用的用途是:
  1. 防止服务器重启时之前绑定的端口还未释放
  2. 程序突然退出而系统**没有释放端口**

```c
#include <sys/types.h> 
#include <sys/socket.h>
// 设置套接字的属性（不仅仅能设置端口复用）
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t 
optlen);
    参数：
        - sockfd : 要操作的文件描述符
        - level : 级别 - SOL_SOCKET (端口复用的级别)
        - optname : 选项的名称
            - SO_REUSEADDR
            - SO_REUSEPORT
        - optval : 端口复用的值（整形）
            - 1 : 可以复用
            - 0 : 不可以复用
        - optlen : optval参数的大小
端口复用，设置的时机是在服务器绑定bind端口之前。
setsockopt();
bind();
```

* 查看网络相关信息的命令
  netstat 
   参数：-a 所有的socket
     -p 显示正在使用socket的程序的名称
     -n 直接使用IP地址，而不通过域名服务器

```c
#include <stdio.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    // 创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);

    if(lfd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(9999);
    
    //int optval = 1;
    //setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    // 绑定
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        return -1;
    }

    // 监听
    ret = listen(lfd, 8);
    if(ret == -1) {
        perror("listen");
        return -1;
    }

    // 接收客户端连接
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);
    if(cfd == -1) {
        perror("accpet");
        return -1;
    }

    // 获取客户端信息
    char cliIp[16];
    inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, cliIp, sizeof(cliIp));
    unsigned short cliPort = ntohs(cliaddr.sin_port);

    // 输出客户端的信息
    printf("client's ip is %s, and port is %d\n", cliIp, cliPort );

    // 接收客户端发来的数据
    char recvBuf[1024] = {0};
    while(1) {
        int len = recv(cfd, recvBuf, sizeof(recvBuf), 0);
        if(len == -1) {
            perror("recv");
            return -1;
        } else if(len == 0) {
            printf("客户端已经断开连接...\n");
            break;
        } else if(len > 0) {
            printf("read buf = %s\n", recvBuf);
        }

        // 小写转大写
        for(int i = 0; i < len; ++i) {
            recvBuf[i] = toupper(recvBuf[i]);
        }

        printf("after buf = %s\n", recvBuf);

        // 大写字符串发给客户端
        ret = send(cfd, recvBuf, strlen(recvBuf) + 1, 0);
        if(ret == -1) {
            perror("send");
            return -1;
        }
    }
    
    close(cfd);
    close(lfd);

    return 0;
}
```

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {

    // 创建socket
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in seraddr;
    inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9999);

    // 连接服务器
    int ret = connect(fd, (struct sockaddr *)&seraddr, sizeof(seraddr));

    if(ret == -1){
        perror("connect");
        return -1;
    }

    while(1) {
        char sendBuf[1024] = {0};
        fgets(sendBuf, sizeof(sendBuf), stdin);

        write(fd, sendBuf, strlen(sendBuf) + 1);

        // 接收
        int len = read(fd, sendBuf, sizeof(sendBuf));
        if(len == -1) {
            perror("read");
            return -1;
        }else if(len > 0) {
            printf("read buf = %s\n", sendBuf);
        } else {
            printf("服务器已经断开连接...\n");
            break;
        }
    }

    close(fd);

    return 0;
}
```

### 4.6  IO多路复用

* I/O 多路复用使得**程序能同时监听多个文件描述符**，能够提高程序的性能，Linux 下实现 I/O 多路复用的
  系统调用主要有 select、poll 和 epoll

##### IO多路复用概述

又叫IO多路转接，IO指的是针对内存

* BIO模型（线进程和客户端一一对应 accept、read会阻塞）的缺点：
   1. 线程或者进程会消耗资源；

   2. 线程或者进程的调度消耗CPU资源；

   3. 根本问题：BOLCKING阻塞

      <img src="D:\MyTxt\typoraPhoto\image-20230303164114741.png" alt="image-20230303164114741" style="zoom: 67%;" />

* 非阻塞，忙轮询的模型：

  1. 提高了程序的执行效率；

  2. 但是轮询需要占用更多的CPU和系统资源

     <img src="D:\MyTxt\typoraPhoto\image-20230303164324869.png" alt="image-20230303164324869" style="zoom: 33%;" />

* NIO模型：
  1. 需要不停调用，read就要调用n次

<img src="D:\MyTxt\typoraPhoto\image-20230303164452309.png" alt="image-20230303164452309" style="zoom:50%;" />

* IO多路转接技术：
  	委托内核，比较快，而且直接可以知道哪些有数据，不需要全部read一遍（select需要）
  1. select只会告诉你有几个文件描述符，有数据到达
  2. epoll很勤快，她不仅会告诉你有几个快递到了，还会告诉你是哪个快递公司的快递

##### select

>主旨思想：
>1. 首先要构造一个关于文件描述符的列表，将要监听的文件描述符添加到该列表中。
>2. 调用一个系统函数，监听该列表中的文件描述符，直到这些描述符中的一个或者多个进行I/O
>操作时，该函数才返回。
>a.这个函数是阻塞
>b.函数对文件描述符的检测的操作是由内核完成的
>3. 在返回时，它会告诉进程有多少（哪些）描述符要进行I/O操作。

```c
// sizeof(fd_set) = 128   1024
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
//需要检测的文件描述符，传入内核，然后通过传入传出参数得到	
int select(int nfds, fd_set *readfds, fd_set *writefds,
     fd_set *exceptfds, struct timeval *timeout);
- 参数：
   - nfds : 委托内核检测的最大文件描述符的值 + 1//也就是检测位+1，因为数组从0开始
      - readfds : 要检测的文件描述符的读的集合，委托内核检测哪些文件描述符的读的属性
        - 一般检测读操作
        - 对应的是对方发送过来的数据，因为读是被动的接收数据，检测的就是读缓冲区
        - 是一个传入传出参数
      - writefds : 要检测的文件描述符的写的集合，委托内核检测哪些文件描述符的写的属性
        - //委托内核检测写缓冲区是不是还可以写数据（不满的就可以写） 一般不检测
	 - exceptfds : 检测发生异常的文件描述符的集合 //一般用不到
      - ti meout : 设置的超时时间 //检测的最大时间，因为不能一直阻塞，除非设置NULL
        struct timeval {
       		long   tv_sec;     /* seconds */
       		long   tv_usec;     /* microseconds */
    	 };
		- NULL : 永久阻塞，直到检测到了文件描述符有变化//永久阻塞
        - tv_sec = 0 tv_usec = 0， 不阻塞
       	- tv_sec > 0 tv_usec > 0， 阻塞对应的时间
         
   - 返回值 :
	  - >1 : 失败
      - >0(n) : 检测的集合中有n个文件描述符发生了变化
// 将参数文件描述符fd对应的标志位设置为0
void FD_CLR(int fd, fd_set *set);
// 判断fd对应的标志位是0还是1， 返回值 ： fd对应的标志位的值，0，返回0， 1，返回1
int  FD_ISSET(int fd, fd_set *set);
// 将参数文件描述符fd 对应的标志位，设置为1
void FD_SET(int fd, fd_set *set);
// fd_set一共有1024 bit, 全部初始化为0
void FD_ZERO(fd_set *set);
```

![image-20230303114212143](D:\MyTxt\typoraPhoto\image-20230303114212143.png)

*  对应 3 4 100 101的fd：
  1. 先定义reads 1024bit（0-1023），然后监听 3 4 100 101的fd放到reads中
  2. 用select把reads读到内核中，并且检测，检测到有数据的置位1，没数据置位0，（比如只有3 4 有数据）
  3. 然后从内核态拷贝到用户态（用户态遍历之后知道3 4 有数据），然后对有数据的进行通信

###### select实现多路转接

```c
# include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(){
    //创建socket
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;

    //绑定
    bind(lfd,(struct sockaddr *)&saddr,sizeof(saddr));

    //监听
    listen(lfd,8);

    //多路转接模型
        //创建fd_set集合，存放需要检测的文件描述符
    fd_set rdset,tmp;//底层表示1024个文件描述符
        //清空rdset
    FD_ZERO(&rdset);
        //添加lfd
    FD_SET(lfd,&rdset);
    int maxfd = lfd;

    while(1){
        tmp = rdset;
        //调用系统函数，让内核帮检测哪个有数据
        int ret = select(maxfd+1,&tmp,NULL,NULL,NULL);
        if(ret == -1){
            perror("select");
            exit(-1);
        }else if(ret == 0){
            //没有数据变化
            continue;
        }else if(ret > 0){
            //说明有文件描述符对应的缓冲区数据有ret个发生了改变
            if(FD_ISSET(lfd,&tmp)){
                //表示有新的客户端连接进来
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr); 
                int cfd = accept(lfd,(struct sockaddr *)&cliaddr,&len);

                //新的文件描述符加入到集合中 
                FD_SET(cfd,&rdset);

                //更新最大的文件描述符
                maxfd = maxfd > cfd ? maxfd : cfd;
            }
            //内核判断集合中有数据的文件描述符后，依次读取数据
            for(int i = lfd+1;i <= maxfd;i++){
                if(FD_ISSET(i,&tmp)){
                    //说明这个文件描述符对应的客户端发来数据
                    char buf[1024] = {0};
                    int len = read(i,&buf,sizeof(buf));
                    if(len == -1){
                        perror("read");
                        exit(-1);
                    }else if(len == 0){
                        //读取完了 清空文件描述符
                        printf("client closed...\n");
                        FD_CLR(i,&rdset);
                    }else if(len > 0){
                        printf("read buf = %s\n",buf);
                        write(i,buf,strlen(buf)+1);
                    }
                }
            }        
        }
    }
    close(lfd);
    return 0;
}
```

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {

    // 创建socket
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in seraddr;
    inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9999);

    // 连接服务器
    int ret = connect(fd, (struct sockaddr *)&seraddr, sizeof(seraddr));

    if(ret == -1){
        perror("connect");
        return -1;
    }

    int num = 0;
    while(1) {
        char sendBuf[1024] = {0};
        sprintf(sendBuf, "send data %d", num++);
        write(fd, sendBuf, strlen(sendBuf) + 1);

        // 接收
        int len = read(fd, sendBuf, sizeof(sendBuf));
        if(len == -1) {
            perror("read");
            return -1;
        }else if(len > 0) {
            printf("read buf = %s\n", sendBuf);
        } else {
            printf("服务器已经断开连接...\n");
            break;
        }
        // sleep(1);
        sleep(1);
    }

    close(fd);

    return 0;
}
```

##### poll

* select() 的缺点：
  1.每次调用select，都需要把fd集合从用户态**拷贝**到内核态，这个开销在fd很多时会很大
  2.同时每次调用select都需要在内核**遍历传递进来的所有fd**，这个开销在fd很多时也很大
  3.select**支持的文件描述符数量太小**了，默认是1024
  4.**fds集合不能重用**，每次都需要重置
* poll解决了缺点3 4

<img src="D:\MyTxt\typoraPhoto\image-20230303165739608.png" alt="image-20230303165739608" style="zoom:50%;" />

* poll接口说明

```c
#include <poll.h>
struct pollfd {
int  fd;     /* 委托内核检测的文件描述符 */
short events;   /* 委托内核检测文件描述符的什么事件 */
short revents;   /* 文件描述符实际发生的事件 */              //调用poll的时候会传出
};
//示例
struct pollfd myfd;
myfd.fd = 5;
myfd.events = POLLIN | POLLOUT;


int poll(struct pollfd *fds, nfds_t nfds, int timeout);
- 参数：
    - fds : 是一个struct pollfd 结构体数组，这是一个需要检测的文件描述符的集合
    - nfds : 这个是第一个参数数组中最后一个有效元素的下标 + 1
    - timeout : 阻塞时长
      0 : 不阻塞
      -1 : 阻塞，当检测到需要检测的文件描述符有变化，解除阻塞
      >0 : 阻塞的时长
  - 返回值：
    -1 : 失败
    >0（n） : 成功,n表示检测到集合中有n个文件描述符发生变化
```

<img src="D:\MyTxt\typoraPhoto\image-20230303170358379.png" alt="image-20230303170358379" style="zoom: 67%;" />

###### poll实现多路转接

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>


int main() {

    // 创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    // 监听
    listen(lfd, 8);

    // 初始化检测的文件描述符数组
    struct pollfd fds[1024];
    for(int i = 0; i < 1024; i++) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    fds[0].fd = lfd;
    int nfds = 0;

    while(1) {

        // 调用poll系统函数，让内核帮检测哪些文件描述符有数据
        int ret = poll(fds, nfds + 1, -1);
        if(ret == -1) {
            perror("poll");
            exit(-1);
        } else if(ret == 0) {
            continue;
        } else if(ret > 0) {
            // 说明检测到了有文件描述符的对应的缓冲区的数据发生了改变
            if(fds[0].revents & POLLIN) {
                // 表示有新的客户端连接进来了
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);

                // 将新的文件描述符加入到集合中
                for(int i = 1; i < 1024; i++) {
                    if(fds[i].fd == -1) {
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;
                        break;
                    }
                }

                // 更新最大的文件描述符的索引
                nfds = nfds > cfd ? nfds : cfd;
            }

            for(int i = 1; i <= nfds; i++) {
                if(fds[i].revents & POLLIN) {
                    // 说明这个文件描述符对应的客户端发来了数据
                    char buf[1024] = {0};
                    int len = read(fds[i].fd, buf, sizeof(buf));
                    if(len == -1) {
                        perror("read");
                        exit(-1);
                    } else if(len == 0) {
                        printf("client closed...\n");
                        close(fds[i].fd);
                        fds[i].fd = -1;
                    } else if(len > 0) {
                        printf("read buf = %s\n", buf);
                        write(fds[i].fd, buf, strlen(buf) + 1);
                    }
                }
            }

        }

    }
    close(lfd);
    return 0;
}
```

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {

    // 创建socket
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in seraddr;
    inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9999);

    // 连接服务器
    int ret = connect(fd, (struct sockaddr *)&seraddr, sizeof(seraddr));

    if(ret == -1){
        perror("connect");
        return -1;
    }

    int num = 0;
    while(1) {
        char sendBuf[1024] = {0};
        sprintf(sendBuf, "send data %d", num++);
        write(fd, sendBuf, strlen(sendBuf) + 1);

        // 接收
        int len = read(fd, sendBuf, sizeof(sendBuf));
        if(len == -1) {
            perror("read");
            return -1;
        }else if(len > 0) {
            printf("read buf = %s\n", sendBuf);
        } else {
            printf("服务器已经断开连接...\n");
            break;
        }
        // sleep(1);
        sleep(1);
    }

    close(fd);

    return 0;
}
```

##### epoll

* 直接在内核操作，内核用rbtree比较快，事件驱动，红黑树节点上注册有回调函数，事件到来后执行回调函数
* 去掉拷贝到内核的开销，并且能告知哪些文件描述符发送改变，而不只是个数

```c
#include <sys/epoll.h>
// 创建一个新的epoll实例。在内核中创建了一个数据，这个数据中有两个比较重要的数据，一个是需要检
测的文件描述符的信息（红黑树），还有一个是就绪列表，存放检测到数据发送改变的文件描述符信息（双向
链表）。
int epoll_create(int size);
    - 参数：
        size : 目前没有意义了。随便写一个数，必须大于0
    - 返回值：
        -1 : 失败
        > 0 : 文件描述符//操作epoll实例的

struct epoll_event {
    uint32_t     events;      /* Epoll events */
    epoll_data_t data;        /* User data variable */
};
	常见的Epoll检测事件：
    - EPOLLIN
    - EPOLLOUT
    - EPOLLERR
typedef union epoll_data {
    void        *ptr;
    int          fd;
    uint32_t     u32;
    uint64_t     u64;
} epoll_data_t;
// 对epoll实例进行管理：添加文件描述符信息，删除信息，修改信息
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
    - 参数：
            - epfd : epoll实例对应的文件描述符
            - op : 要进行什么操作
                EPOLL_CTL_ADD:  添加 //记得提前设置一个epoll_event结构
                EPOLL_CTL_MOD:  修改
                EPOLL_CTL_DEL:  删除 //删除处理，第四的个参数为NULL
            - fd : 要检测的文件描述符
            - event : 检测文件描述符什么事情
// 检测函数                
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
    - 参数：
        - epfd : epoll实例对应的文件描述符
        - events : 传出参数，保存了发送了变化的文件描述符的信息 //之后遍历读取处理
        - maxevents : 第二个参数结构体数组的大小
        - timeout : 阻塞时间
            - 0 : 不阻塞
            - -1 : 阻塞，直到检测到fd数据发生变化，解除阻塞
            - > 0 : 阻塞的时长（毫秒）
                
    - 返回值：
         - 成功，返回发送变化的文件描述符的个数 > 0
         - 失败 -1
```

###### epoll实现多路转接

建立epoll实例，添加EPOLLIN，用epoll_wait返回所有事件，可以得到所有事件；
然后按照事件类型逐个处理，把新连接进来的客户端添加到epoll实例中监听，把处理好的事件从epoll实例中去掉；

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

int main() {

    // 创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    // 监听
    listen(lfd, 8);

    //创建epoll实例
    int epfd = epoll_create(100);

    //将需要监听的文件描述符相关的检测信息添加到epoll实例中
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&epev);

    struct epoll_event epevs[1024];

    while(1){
        int ret = epoll_wait(epfd,epevs,1024,-1);
        if(ret == -1){
            perror("epoll_wait");
            exit(-1);
        }

        printf("ret = %d\n",ret);

        for(int i = 0; i < ret; i++){

            int curfd =epevs[i].data.fd;

            if(curfd == lfd){
                //监听的文件描述符有数据到达，有客户端连接
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr); 
                int cfd = accept(lfd,(struct sockaddr *)&cliaddr,&len);
                //添加到需要监听的部分 可以重用之前的epoll_event数据结构
                epev.events = EPOLLIN;
                epev.data.fd = cfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&epev);
            }else{
                //过滤写事件，也就是说对于不同时间要单独处理
                if(epevs[i].events & EPOLLOUT){
                    continue;
                }
                // 有数据到达
                char buf[1024] = {0};
                int len = read(curfd,buf,sizeof(buf));
                if(len == -1){
                    perror("read");
                    exit(-1);
                }else if(len == 0){
                    //读取完了 清空文件描述符
                    printf("client closed...\n");
                    //从epoll实例中删除掉
                    epoll_ctl(epfd,EPOLL_CTL_DEL,curfd,NULL);
                    close(curfd);
                }else if(len > 0){
                    printf("read buf = %s\n",buf);
                    write(curfd,buf,strlen(buf)+1);
                } 
            }
        }
    }
    close(epfd);    
    close(lfd);
}
```



##### Epoll的工作模式

* LT 模式 （水平触发）
  假设委托内核检测读事件 -> 检测fd的读缓冲区
   读缓冲区有数据 - > epoll检测到了会给用户通知
     a.用户不读数据，数据一直在缓冲区，epoll 会一直通知
     b.用户只读了一部分数据，epoll会通知
     c.缓冲区的数据读完了，不通知

> LT（level - triggered）是缺省的工作方式，并且同时支持 block 和 no-block socket。在这
> 种做法中，内核告诉你一个文件描述符是否就绪了，然后你可以对这个就绪的 fd 进行 IO 操
> 作。如果你不作任何操作，内核还是会继续通知你的。

* ET 模式（边沿触发）

  假设委托内核检测读事件 -> 检测fd的读缓冲区
   读缓冲区有数据 - > epoll检测到了会给用户通知
     a.用户不读数据，数据一直在缓冲区中，epoll下次检测的时候就不通知了
     b.用户只读了一部分数据，epoll不通知
     c.缓冲区的数据读完了，不通知

> ET（edge - triggered）是高速工作方式，只支持 no-block socket。在这种模式下，当描述
> 符从未就绪变为就绪时，内核通过epoll告诉你。然后它会假设你知道文件描述符已经就绪，
> 并且不会再为那个文件描述符发送更多的就绪通知，直到你做了某些操作导致那个文件描述
> 符不再为就绪状态了。但是请注意，如果一直不对这个 fd 作 IO 操作（从而导致它再次变成
> 未就绪），内核不会发送更多的通知（only once）。
> ET 模式在很大程度上减少了 epoll 事件被重复触发的次数，因此效率要比 LT 模式高。epoll 
> 工作在 ET 模式的时候，必须使用非阻塞套接口，以避免由于一个文件句柄的阻塞读/阻塞写
> 操作把处理多个文件描述符的任务饿死。

```c
 struct epoll_event {
    uint32_t     events;      /* Epoll events */
    epoll_data_t data;        /* User data variable */
};
常见的Epoll检测事件：
    - EPOLLIN
    - EPOLLOUT
    - EPOLLERR
    - EPOLLET	//边沿触发 用的时候|上前面的检测事件
```

###### 水平触发 - 默认

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>


int main() {

    // 创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    // 监听
    listen(lfd, 8);

    //创建epoll实例
    int epfd = epoll_create(100);

    //将需要监听的文件描述符相关的检测信息添加到epoll实例中
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&epev);

    struct epoll_event epevs[1024];

    while(1){
        int ret = epoll_wait(epfd,epevs,1024,-1);
        if(ret == -1){
            perror("epoll_wait");
            exit(-1);
        }

        printf("ret = %d\n",ret);

        for(int i = 0; i < ret; i++){

            int curfd =epevs[i].data.fd;

            if(curfd == lfd){
                //监听的文件描述符有数据到达，有客户端连接
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr); 
                int cfd = accept(lfd,(struct sockaddr *)&cliaddr,&len);
                //添加到需要监听的部分 可以重用之前的epoll_event数据结构
                epev.events = EPOLLIN;
                epev.data.fd = cfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&epev);
            }else{
                //过滤写事件，也就是说对于不同时间要单独处理
                if(epevs[i].events & EPOLLOUT){
                    continue;
                }
                // 有数据到达
                char buf[5] = {0};
                int len = read(curfd,buf,sizeof(buf));
                if(len == -1){
                    perror("read");
                    exit(-1);
                }else if(len == 0){
                    //读取完了 清空文件描述符
                    printf("client closed...\n");
                    //从epoll实例中删除掉
                    epoll_ctl(epfd,EPOLL_CTL_DEL,curfd,NULL);
                    close(curfd);
                }else if(len > 0){
                    printf("read buf = %s\n",buf);
                    write(curfd,buf,strlen(buf)+1);
                } 
            }
        }
    }
    close(epfd);    
    close(lfd);
}
```

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {

    // 创建socket
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in seraddr;
    inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9999);

    // 连接服务器
    int ret = connect(fd, (struct sockaddr *)&seraddr, sizeof(seraddr));

    if(ret == -1){
        perror("connect");
        return -1;
    }

    int num = 0;
    while(1) {
        char sendBuf[5] = {0};
        //sprintf(sendBuf, "send data %d", num++);
        //fgets获取stdin的输入，阻塞
        fgets(sendBuf,sizeof(sendBuf),stdin);
        write(fd, sendBuf, strlen(sendBuf) + 1);

        // 接收
        int len = read(fd, sendBuf, sizeof(sendBuf));
        if(len == -1) {
            perror("read");
            return -1;
        }else if(len > 0) {
            printf("read buf = %s\n", sendBuf);
        } else {
            printf("服务器已经断开连接...\n");
            break;
        }
        // sleep(1);
        //usleep(1000);
    }

    close(fd);

    return 0;
}
```

###### 边沿触发 - EPOLLET

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

int main() {

    // 创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    // 监听
    listen(lfd, 8);

    // 调用epoll_create()创建一个epoll实例
    int epfd = epoll_create(100);

    // 将监听的文件描述符相关的检测信息添加到epoll实例中
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev);

    struct epoll_event epevs[1024];

    while(1) {

        int ret = epoll_wait(epfd, epevs, 1024, -1);
        if(ret == -1) {
            perror("epoll_wait");
            exit(-1);
        }

        printf("ret = %d\n", ret);

        for(int i = 0; i < ret; i++) {

            int curfd = epevs[i].data.fd;

            if(curfd == lfd) {
                // 监听的文件描述符有数据达到，有客户端连接
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);

                // 设置cfd属性非阻塞
                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);

                epev.events = EPOLLIN | EPOLLET;    // 设置边沿触发
                epev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);
            } else {
                if(epevs[i].events & EPOLLOUT) {
                    continue;
                }  

                // 循环读取出所有数据
                char buf[5];
                int len = 0;
                while( (len = read(curfd, buf, sizeof(buf))) > 0) {
                    // 打印数据
                    // printf("recv data : %s\n", buf);
                    write(STDOUT_FILENO, buf, len);
                    write(curfd, buf, len);
                }
                if(len == 0) {
                    printf("client closed....");
                }else if(len == -1) {
                    if(errno == EAGAIN) {
                        printf("data over.....");
                    }else {
                        perror("read");
                        exit(-1);
                    }
                    
                }

            }

        }
    }

    close(lfd);
    close(epfd);
    return 0;
}
```

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {

    // 创建socket
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in seraddr;
    inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9999);

    // 连接服务器
    int ret = connect(fd, (struct sockaddr *)&seraddr, sizeof(seraddr));

    if(ret == -1){
        perror("connect");
        return -1;
    }

    int num = 0;
    while(1) {
        char sendBuf[1024] = {0};
        // sprintf(sendBuf, "send data %d", num++);
        fgets(sendBuf, sizeof(sendBuf), stdin);

        write(fd, sendBuf, strlen(sendBuf) + 1);

        // 接收
        int len = read(fd, sendBuf, sizeof(sendBuf));
        if(len == -1) {
            perror("read");
            return -1;
        }else if(len > 0) {
            printf("read buf = %s\n", sendBuf);
        } else {
            printf("服务器已经断开连接...\n");
            break;
        }
    }

    close(fd);

    return 0;
}

```

### 4.7  UDP通信

<img src="D:\MyTxt\typoraPhoto\image-20230306102024691.png" alt="image-20230306102024691" style="zoom: 80%;" />

```c
 #include <sys/types.h>
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen);
        - 参数：
            - sockfd : 通信的fd
            - buf : 要发送的数据
            - len : 发送数据的长度
            - flags : 0
            - dest_addr : 通信的另外一端的地址信息
            - addrlen : 地址的内存大小
                
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen);
        - 参数：
            - sockfd : 通信的fd
            - buf : 接收数据的数组
            - len : 数组的大小
            - flags : 0
            - src_addr : 用来保存另外一端的地址信息，不需要可以指定为NULL //传出参数 不需要最后两个参数都可以指定NULL
            - addrlen : 地址的内存大小
```

##### UDP实现流程

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


int main(){
    
    //1.创建socket 选择数据报而不是流
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }
    //2.绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(fd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }

    //3.通信
    while(1){
        char recvbuf[128];
        char ipbuf[16];
        struct sockaddr_in cliaddr;
        int len = sizeof(cliaddr);
        //接受数据
        recvfrom(fd,recvbuf,sizeof(recvbuf),0,(struct sockaddr*)&cliaddr,&len);

        printf("client IP: %s,Port: %d\n",
            inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,ipbuf,sizeof(ipbuf)),
            ntohs(cliaddr.sin_port));

        printf("client say: %s\n",recvbuf);

        //发送数据
        sendto(fd,recvbuf,strlen(recvbuf)+1,0,(struct sockaddr*)&cliaddr,len);
    }

    close(fd);

    return 0;
}
```

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


int main(){
    
    //1.创建socket 选择数据报而不是流
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    inet_pton(AF_INET,"127.0.0.1",&saddr.sin_addr.s_addr);

    int num = 0;
    //2.通信
    while(1){

        //发送数据
        char sendBuf[128];
        sprintf(sendBuf,"hello,i am client %d\n",num++);
        sendto(fd,sendBuf,strlen(sendBuf)+1,0,(struct sockaddr*)&saddr,sizeof(saddr));
    
        //接受数据
        recvfrom(fd,sendBuf,sizeof(sendBuf),0,NULL,NULL);
        printf("server say: %s\n",sendBuf);

        sleep(1);
    }   

    close(fd);
    return 0;
}
```



* 广播和多播都用UDP

##### 广播

向子网中多台计算机发送消息，并且子网中所有的计算机都可以接收到发送方发送的消息，每个广
播消息都包含一个特殊的IP地址，这个IP中子网内主机标志部分的二进制全部为1。
a.只能在局域网中使用。
b.客户端需要绑定服务器广播使用的端口，才可以接收到广播消息

<img src="D:\MyTxt\typoraPhoto\image-20230306114206431.png" alt="image-20230306114206431" style="zoom: 50%;" />

```c
// 设置广播属性的函数
int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t 
optlen);
    - sockfd : 文件描述符
    - level : SOL_SOCKET
    - optname : SO_BROADCAST
    - optval : int类型的值，为1表示允许广播//1
    - optlen : optval的大小
//使用的时候创建广播地址的套接字IP地址设置为广播地址，也就是最后一位255
```

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


int main(){
    
    //1.创建socket 选择数据报而不是流
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }

    //2.设置广播属性
    int op = 1;
    setsockopt(fd,SOL_SOCKET,SO_BROADCAST,&op,sizeof(op));

    //3.创建广播地址  因为这里服务端是主动给别人发送数据了，不需要手动绑定一个端口了，但是底层肯定还会分配一个端口给他的。
    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(9999);
    //广播地址
    inet_pton(AF_INET,"192.168.31.255",&cliaddr.sin_addr.s_addr);

    //4.通信
    int num=0;
    while(1){
        char sendbuf[128];
        sprintf(sendbuf,"hello,client...%d\n",num++);
        //发送数据
        sendto(fd,sendbuf,strlen(sendbuf)+1,0,(struct sockaddr*)&cliaddr,sizeof(cliaddr));
        printf("广播的数据: %d\n",num);
        sleep(1);
    }

    close(fd);
    return 0;
}
```

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


int main(){
    
    //1.创建socket 选择数据报而不是流
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }

    //2.客户端绑定本地接受的ip和端口
    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(9999);
    cliaddr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(fd,(struct sockaddr*)&cliaddr,sizeof(cliaddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }

    //3.通信
    while(1){
        char buf[128];
        recvfrom(fd,buf,sizeof(buf),0,NULL,NULL);
        printf("server say : %s\n", buf);
    }

    close(fd);
    return 0;
}
```



##### 组播（多播）

单播地址标识单个 IP 接口，广播地址标识某个子网的所有 IP 接口，多播地址标识一组 IP 接口。
单播和广播是寻址方案的两个极端（要么单个要么全部），多播则意在两者之间提供一种折中方
案。多播数据报只应该由对它感兴趣的接口接收，也就是说由**运行相应多播会话应用系统的主机上**
**的接口接收**。另外，广播一般局限于局域网内使用，而多播则既可以用于局域网，也可以跨广域网
使用。
a.组播既可以用于局域网，也可以用于广域网
b.客户端需要加入多播组，才能接收到多播的数据

<img src="D:\MyTxt\typoraPhoto\image-20230306140538652.png" alt="image-20230306140538652" style="zoom: 50%;" />

* 组播地址：
  IP 多播通信必须依赖于 IP 多播地址，在 IPv4 中它的范围从  224.0.0.0 到 239.255.255.255 ，
  并被划分为局部链接多播地址、预留多播地址和管理权限多播地址三类:

<img src="D:\MyTxt\typoraPhoto\image-20230306140713969.png" alt="image-20230306140713969" style="zoom:50%;" />

```c
int setsockopt(int sockfd, int level, int optname,const void *optval, 
socklen_t optlen);
    
    // 服务器设置多播的信息，外出接口
	- level : IPPROTO_IP
    - optname : IP_MULTICAST_IF
    - optval : struct in_addr //
    
    // 客户端加入到多播组：
    - level : IPPROTO_IP
    - optname : IP_ADD_MEMBERSHIP
    - optval : struct ip_mreq
struct ip_mreq
{
    /* IP multicast address of group.  */
    struct in_addr imr_multiaddr;   // 组播的IP地址
    /* Local IP address of interface.  */
    struct in_addr imr_interface;   // 本地的IP地址
};
typedef uint32_t in_addr_t;
struct in_addr
{
    in_addr_t s_addr;
};
```

* 多播通信示例

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main() {

    // 1.创建一个通信的socket
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        perror("socket");
        exit(-1);
    }   

    // 2.设置多播的属性，设置外出接口
    struct in_addr imr_multiaddr;
    // 初始化多播地址 从表中选一个多播地址
    inet_pton(AF_INET, "239.0.0.10", &imr_multiaddr.s_addr);
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &imr_multiaddr, sizeof(imr_multiaddr));
    
    // 3.初始化客户端的地址信息
    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(9999);
    inet_pton(AF_INET, "239.0.0.10", &cliaddr.sin_addr.s_addr);

    // 3.通信
    int num = 0;
    while(1) {
       
        char sendBuf[128];
        sprintf(sendBuf, "hello, client....%d\n", num++);
        // 发送数据
        sendto(fd, sendBuf, strlen(sendBuf) + 1, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
        printf("组播的数据：%s\n", sendBuf);
        sleep(1);
    }

    close(fd);
    return 0;
}
```

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main() {

    // 1.创建一个通信的socket
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        perror("socket");
        exit(-1);
    }   

    struct in_addr in;
    // 2.客户端绑定IP和端口
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1) {
        perror("bind");
        exit(-1);
    }

    struct ip_mreq op;
    inet_pton(AF_INET, "239.0.0.10", &op.imr_multiaddr.s_addr);
    op.imr_interface.s_addr = INADDR_ANY;

    // 加入到多播组
    setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &op, sizeof(op));

    // 3.通信
    while(1) {
        
        char buf[128];
        // 接收数据
        int num = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
        printf("server say : %s\n", buf);

    }

    close(fd);
    return 0;
}
```

### 4.8 本地套接字

* 本地套接字的作用：本地的进程间通信
   有关系的进程间的通信
   没有关系的进程间的通信
  本地套接字实现流程和网络套接字类似，一般呢采用TCP的通信流程。

<img src="D:\MyTxt\typoraPhoto\image-20230306153412355.png" alt="image-20230306153412355" style="zoom:50%;" />

```c
 // 本地套接字通信的流程 - tcp

// 头文件:  sys/un.h
#define UNIX_PATH_MAX 108
struct sockaddr_un {
    sa_family_t sun_family; // 地址族协议 af_local
    char sun_path[UNIX_PATH_MAX];   // 套接字文件的路径, 这是一个伪文件, 大小永远=0
};

// 服务器端
1. 创建监听的套接字
    int lfd = socket(AF_UNIX/AF_LOCAL, SOCK_STREAM, 0);
2. 监听的套接字绑定本地的套接字文件 -> server端
    struct sockaddr_un addr; //对local socket的数据结构
    // 绑定成功之后，指定的sun_path中的套接字文件会自动生成。
    bind(lfd, addr, len);
3. 监听
    listen(lfd, 100);
4. 等待并接受连接请求
    struct sockaddr_un cliaddr;
    int cfd = accept(lfd, &cliaddr, len);
5. 通信
    接收数据：read/recv
    发送数据：write/send
6. 关闭连接
    close();
// 客户端的流程
1. 创建通信的套接字
    int fd = socket(AF_UNIX/AF_LOCAL, SOCK_STREAM, 0);
2. 监听的套接字绑定本地的IP 端口
    struct sockaddr_un addr;
    // 绑定成功之后，指定的sun_path中的套接字文件会自动生成。
    bind(lfd, addr, len);
3. 连接服务器
    struct sockaddr_un serveraddr;
    connect(fd, &serveraddr, sizeof(serveraddr));
4. 通信
    接收数据：read/recv
    发送数据：write/send
5. 关闭连接
    close();
```

* 例子

```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main() {

    unlink("server.sock");

    // 1.创建监听的套接字
    int lfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(lfd == -1) {
        perror("socket");
        exit(-1);
    }

    // 2.绑定本地套接字文件
    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, "server.sock");
    int ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1) {
        perror("bind");
        exit(-1);
    }

    // 3.监听
    ret = listen(lfd, 100);
    if(ret == -1) {
        perror("listen");
        exit(-1);
    }

    // 4.等待客户端连接
    struct sockaddr_un cliaddr;
    int len = sizeof(cliaddr);
    int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);
    if(cfd == -1) {
        perror("accept");
    
        exit(-1);
    }

    printf("client socket filename: %s\n", cliaddr.sun_path);

    // 5.通信
    while(1) {

        char buf[128];
        int len = recv(cfd, buf, sizeof(buf), 0);

        if(len == -1) {
            perror("recv");
            exit(-1);
        } else if(len == 0) {
            printf("client closed....\n");
            break;
        } else if(len > 0) {
            printf("client say : %s\n", buf);
            send(cfd, buf, len, 0);
        }

    }

    close(cfd);
    close(lfd);

    return 0;
}
```

```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main() {

    unlink("client.sock");

    // 1.创建套接字
    int cfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(cfd == -1) {
        perror("socket");
        exit(-1);
    }

    // 2.绑定本地套接字文件
    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, "client.sock");
    int ret = bind(cfd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1) {
        perror("bind");
        exit(-1);
    }

    // 3.连接服务器
    struct sockaddr_un seraddr;
    seraddr.sun_family = AF_LOCAL;
    strcpy(seraddr.sun_path, "server.sock");
    ret = connect(cfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
    if(ret == -1) {
        perror("connect");
        exit(-1);
    }

    // 4.通信
    int num = 0;
    while(1) {

        // 发送数据
        char buf[128];
        sprintf(buf, "hello, i am client %d\n", num++);
        send(cfd, buf, strlen(buf) + 1, 0);
        printf("client say : %s\n", buf);

        // 接收数据
        int len = recv(cfd, buf, sizeof(buf), 0);

        if(len == -1) {
            perror("recv");
            exit(-1);
        } else if(len == 0) {
            printf("server closed....\n");
            break;
        } else if(len > 0) {
            printf("server say : %s\n", buf);
        }

        sleep(1);

    }

    close(cfd);
    return 0;
}
```

## 5.WebServer项目

### 5.1 阻塞非阻塞、同步异步

![image-20230306165424115](D:\MyTxt\typoraPhoto\image-20230306165424115.png)

* 典型的一次IO的两个阶段是什么？数据就绪 和 数据读写

1. 数据就绪：根据系统IO操作（现在只考虑网络IO）的就绪状态
   阻塞
   非阻塞
2. 数据读写：根据应用程序和内核的交互方式
   同步
   异步

* 陈硕：在处理 IO 的时候，阻塞和非阻塞都是同步 IO，只有使用了特殊的 API 才是异步 IO

<img src="D:\MyTxt\typoraPhoto\image-20230306165722791.png" alt="image-20230306165722791" style="zoom:67%;" />

>  一个典型的网络IO接口调用，分为两个阶段，分别是“数据就绪” 和 “数据读写”，数据就绪阶段分为
> 阻塞和非阻塞，表现得结果就是，阻塞当前线程或是直接返回。
> 同步表示A向B请求调用一个网络IO接口时（或者调用某个业务逻辑API接口时），数据的读写都是
> 由请求方A**自己来完成**的（不管是阻塞还是非阻塞）；异步表示A向B请求调用一个网络IO接口时
> （或者调用某个业务逻辑API接口时），向B传入请求的事件以及事件发生时通知的方式，A就可以
> 处理其它逻辑了，当**B监听到事件处理**完成后，会用事先约定好的通知方式，通知A处理结果。

 同步阻塞 	同步非阻塞	异步阻塞	异步非阻塞

### 5.2 Unix/Linux上的五种IO模型

##### a.阻塞 blocking

调用者调用了某个函数，等待这个函数返回，期间什么也不做，不停的去检查这个函数有没有返回，必
须等这个函数返回才能进行下一步动作

1. 有等待 阻塞 2. 有数据拷贝 同步

<img src="D:\MyTxt\typoraPhoto\image-20230306170646903.png" alt="image-20230306170646903" style="zoom: 67%;" />

##### b.非阻塞 non-blocking（NIO）

非阻塞等待，每隔一段时间就去检测IO事件是否就绪。没有就绪就可以做其他事。非阻塞I/O执行系统调
用总是**立即返回**，不管事件是否已经发生，若事件没有发生，则返回-1，此时可以根据 errno 区分这两
种情况，对于accept，recv 和 send，事件未发生时，errno 通常被设置成 **EAGAIN**。

<img src="D:\MyTxt\typoraPhoto\image-20230306171245018.png" alt="image-20230306171245018" style="zoom: 67%;" />

##### c.IO复用（IO multiplexing）

Linux 用 select/poll/epoll 函数实现 IO 复用模型，这些函数也会使进程阻塞，但是和阻塞IO所不同的是
这些函数可以**同时(非)阻塞地处理多个IO操作**。而且可以同时对多个读操作、写操作的IO函数进行**检测**。直到有数
据可读或可写时，才真正调用IO操作函数。//主要不是处理高并发，而是同时处理多个IO的优点

<img src="D:\MyTxt\typoraPhoto\image-20230306171636481.png" alt="image-20230306171636481" style="zoom: 67%;" />

##### d.信号驱动（signal-driven）

Linux 用套接口进行信号驱动 IO，安装一个信号处理函数，进程继续运行并不阻塞，当IO事件就绪，进
程收到SIGIO 信号，然后处理 IO 事件 

<img src="D:\MyTxt\typoraPhoto\image-20230306172310990.png" alt="image-20230306172310990" style="zoom:67%;" />

内核在第一个阶段是异步，在第二个阶段是同步；与非阻塞IO的区别在于它提供了消息通知机制，不需
要用户进程不断的轮询检查，减少了系统API的调用次数，提高了效率 

//从内核空间拷贝到用户空间还是需要拷贝，也就是同步操作，不常用

##### e.异步（asynchronous）

Linux中，可以调用 aio_read 函数告诉内核描述字缓冲区指针和缓冲区的大小、文件偏移及通知的方
式，然后立即返回，当内核将数据拷贝到缓冲区后，再通知应用程序。

<img src="D:\MyTxt\typoraPhoto\image-20230306175126070.png" alt="image-20230306175126070" style="zoom:67%;" />

```c
/* Asynchronous I/O control block.  */
struct aiocb
{
  int aio_fildes;       /* File desriptor.  */
  int aio_lio_opcode;       /* Operation to be performed.  */
  int aio_reqprio;      /* Request priority offset.  */
  volatile void *aio_buf;   /* Location of buffer.  */
  size_t aio_nbytes;        /* Length of transfer.  */
  struct sigevent aio_sigevent; /* Signal number and value.  */
  /* Internal members.  */
  struct aiocb *__next_prio;
  int __abs_prio;
  int __policy;
  int __error_code;
  __ssize_t __return_value;
#ifndef __USE_FILE_OFFSET64
  __off_t aio_offset;       /* File offset.  */
  char __pad[sizeof (__off64_t) - sizeof (__off_t)];
#else
  __off64_t aio_offset;     /* File offset.  */
#endif
  char __glibc_reserved[32];
};
```

### 5.3 Web Server网页服务器

##### 网页服务器简介

一个 Web Server 就是一个服务器软件（程序），或者是运行这个服务器软件的硬件（计算机）。其主
要功能是通过 HTTP 协议与客户端（通常是浏览器（Browser））进行通信，来接收，存储，处理来自
客户端的 HTTP 请求，并对其请求做出 HTTP 响应，返回给客户端其请求的内容（文件、网页等）或返
回一个 Error 信息。

通常用户使用 Web 浏览器与相应服务器进行通信。在浏览器中键入“域名”或“IP地址:端口号”，浏览器则
先将你的域名解析成相应的 IP 地址或者直接根据你的IP地址向对应的 Web 服务器发送一个 HTTP 请
求。这一过程首先要通过 TCP 协议的三次握手建立与目标 Web 服务器的连接，然后 HTTP 协议生成针
对目标 Web 服务器的 HTTP 请求报文，通过 TCP、IP 等协议发送到目标 Web 服务器上。

<img src="D:\MyTxt\typoraPhoto\image-20230306222359507.png" alt="image-20230306222359507" style="zoom: 67%;" />

#### HTTP

##### 概述

超文本传输协议（Hypertext Transfer Protocol，HTTP）是一个简单的请求 - 响应协议，它通常运行在 
TCP 之上。它指定了客户端可能发送给服务器什么样的消息以及得到什么样的响应。请求和响应消息的
头以 ASCII 形式给出；而消息内容则具有一个类似 MIME 的格式。HTTP是万维网的数据通信的基础。

> HTTP 是一个客户端终端（用户）和服务器端（网站）请求和应答的标准（TCP）。通过使用网页浏览器、网络爬虫或者其它的工具，客户端发起一个HTTP请求到服务器上指定端口（**默认端口为80,HTTPS是443**）。我们称这个客户端为用户代理程序（user agent）。应答的服务器上存储着一些资源，比如 HTML 文件和图像。我们称这个应答服务器为源服务器（origin server）。在用户代理和源服务器中间可能存在多个“中间层”，比如代理服务器、网关或者隧道（tunnel）。尽管 TCP/IP 协议是互联网上最流行的应用，HTTP 协议中，并没有规定必须使用它或它支持的层。事实上，**HTTP可以在任何互联网协议上，或其他网络上实现**。HTTP 假定其下层协议提供可靠的传输。因此，任何能够提供这种保证的协议都可以被其使用。因此也就是其在 TCP/IP 协议族使用 **TCP 作为其传输层**。
> 通常，由HTTP客户端发起一个请求，创建一个到服务器指定端口（默认是80端口）的 TCP 连接。HTTP 服务器则在那个端口监听客户端的请求。一旦收到请求，服务器会向客户端返回一个状态，比如"HTTP/1.1 200 OK"，以及返回的内容，如请求的文件、错误消息、或者其它信息。

##### 工作原理

HTTP 协议定义 Web 客户端如何从 Web 服务器请求 Web 页面，以及服务器如何把 Web 页面传送给客
户端。HTTP 协议采用了请求/响应模型。客户端向服务器发送一个请求报文，请求报文包含请求的方
法、URL、协议版本、请求头部和请求数据。服务器以一个状态行作为响应，响应的内容包括协议的版
本、成功或者错误代码、服务器信息、响应头部和响应数据。

以下是 HTTP 请求/响应的步骤：
1. 客户端连接到 Web 服务器
    一个HTTP客户端，通常是浏览器，与 Web 服务器的 HTTP 端口（默认为 80 ）建立一个 TCP 套接
    字连接。例如，http://www.baidu.com。（URL）
2. 发送 HTTP 请求
    通过 TCP 套接字，客户端向 Web 服务器发送一个文本的请求报文，一个请求报文由请求行、请求
    头部、空行和请求数据 4 部分组成。
3. 服务器接受请求并返回 HTTP 响应
    Web 服务器解析请求，定位请求资源。服务器将资源复本写到 TCP 套接字，由客户端读取。一个
    响应由状态行、响应头部、空行和响应数据 4 部分组成。
4. 释放连接 TCP 连接
    若 connection 模式为 close，则服务器主动关闭 TCP连接，客户端被动关闭连接，释放 TCP 连
    接；若connection 模式为 keepalive，则该连接会保持一段时间，在该时间内可以继续接收请求;
5. 客户端浏览器解析 HTML 内容
    客户端浏览器首先解析状态行，查看表明请求是否成功的状态代码。然后解析每一个响应头，响应
    头告知以下为若干字节的 HTML 文档和文档的字符集。客户端浏览器读取响应数据 HTML，根据 
    HTML 的语法对其进行格式化，并在浏览器窗口中显示。

* 例如：在浏览器地址栏键入URL，按下回车之后会经历以下流程：

1. 浏览器向 DNS 服务器请求解析该 URL 中的域名所对应的 IP 地址;
2. 解析出 IP 地址后，根据该 IP 地址和默认端口 80，和服务器建立 TCP 连接;
3. 浏览器发出读取文件（ URL 中域名后面部分对应的文件）的 HTTP 请求，该请求报文作为 TCP 三
   次握手的第三个报文的数据发送给服务器;
4. 服务器对浏览器请求作出响应，并把对应的 HTML 文本发送给浏览器;
5. 释放 TCP 连接;
6. 浏览器将该 HTML 文本并显示内容。

![image-20230306223145625](D:\MyTxt\typoraPhoto\image-20230306223145625.png)

HTTP 协议是基于 TCP/IP 协议之上的应用层协议，基于 请求-响应 的模式。HTTP 协议规定，请求从客
户端发出，最后服务器端响应该请求并返回。换句话说，肯定是先从客户端开始建立通信的，服务器端
在没有接收到请求之前不会发送响应

##### 请求报文

<img src="D:\MyTxt\typoraPhoto\image-20230307094705215.png" alt="image-20230307094705215" style="zoom: 67%;" />

```http
GET / HTTP/1.1
Host: www.baidu.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:86.0) Gecko/20100101 Firefox/86.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,/;q=0.8
Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2
Accept-Encoding: gzip, deflate, br
Connection: keep-alive
Cookie: BAIDUID=6729CB682DADC2CF738F533E35162D98:FG=1; 
BIDUPSID=6729CB682DADC2CFE015A8099199557E; PSTM=1614320692; BD_UPN=13314752; 
BDORZ=FFFB88E999055A3F8A630C64834BD6D0; 
__yjs_duid=1_d05d52b14af4a339210722080a668ec21614320694782; BD_HOME=1; 
H_PS_PSSID=33514_33257_33273_31660_33570_26350; 
BA_HECTOR=8h2001alag0lag85nk1g3hcm60q
Upgrade-Insecure-Requests: 1
Cache-Control: max-age=
```

##### 响应报文

<img src="D:\MyTxt\typoraPhoto\image-20230307094824169.png" alt="image-20230307094824169" style="zoom:50%;" />

```http
HTTP/1.1 200 OK
Bdpagetype: 1
Bdqid: 0xf3c9743300024ee4
Cache-Control: private
Connection: keep-alive
Content-Encoding: gzip
Content-Type: text/html;charset=utf-8
Date: Fri, 26 Feb 2021 08:44:35 GMT
Expires: Fri, 26 Feb 2021 08:44:35 GMT
Server: BWS/1.1
Set-Cookie: BDSVRTM=13; path=/
Set-Cookie: BD_HOME=1; path=/
Set-Cookie: H_PS_PSSID=33514_33257_33273_31660_33570_26350; path=/; domain=.baidu.com
Strict-Transport-Security: max-age=172800
Traceid: 1614329075128412289017566699583927635684
X-Ua-Compatible: IE=Edge,chrome=1
Transfer-Encoding: chunked
```

##### http状态码

所有HTTP响应的第一行都是状态行，依次是当前HTTP版本号，3位数字组成的状态代码，以及描述状态
的短语，彼此由空格分隔。
状态代码的第一个数字代表当前响应的类型：
1xx消息——请求已被服务器接收，继续处理
2xx成功——请求已成功被服务器接收、理解、并接受
3xx重定向——需要后续操作才能完成这一请求
4xx请求错误——请求含有词法错误或者无法被执行
5xx服务器错误——服务器在处理某个正确请求时发生错误
虽然 RFC 2616 中已经推荐了描述状态的短语，例如"200 OK"，"404 Not Found"，但是WEB开发者仍
然能够自行决定采用何种短语，用以显示本地化的状态描述或者自定义信息。

<img src="D:\MyTxt\typoraPhoto\image-20230307095103231.png" alt="image-20230307095103231" style="zoom:67%;" />

更多状态码：https://baike.baidu.com/item/HTTP%E7%8A%B6%E6%80%81%E7%A0%81/5053660?fr=aladdin

#### 事件处理模式

服务器程序通常需要处理三类事件：I/O 事件、信号及定时事件。有两种高效的事件处理模式：Reactor 
和 Proactor，同步 I/O 模型通常用于实现 Reactor 模式，异步 I/O 模型通常用于实现 Proactor 模式。

##### Reactor模式

要求主线程（I/O处理单元）只负责监听文件描述符上是否有事件发生，有的话就立即将该事件通知工作
线程（逻辑单元），将 socket 可读可写事件放入请求队列，交给工作线程处理。除此之外，主线程不做
任何其他实质性的工作。读写数据，接受新的连接，以及处理客户请求均在工作线程中完成。

* 使用同步 I/O（以 epoll_wait 为例）实现的 Reactor 模式的工作流程是

<img src="D:\MyTxt\typoraPhoto\image-20230307103757704.png" alt="image-20230307103757704" style="zoom: 67%;" />

1. 主线程往 epoll 内核事件表中注册 socket 上的读就绪事件。
2. 主线程调用 epoll_wait 等待 socket 上有数据可读。
3. 当 socket 上有数据可读时， epoll_wait 通知主线程。主线程则将 socket 可读事件放入请求队列。
4. 睡眠在请求队列上的某个工作线程被唤醒，它从 socket 读取数据，并处理客户请求，然后往 epoll 
内核事件表中注册该 socket 上的写就绪事件。
5. 当主线程调用 epoll_wait 等待 socket 可写。
6. 当 socket 可写时，epoll_wait 通知主线程。主线程将 socket 可写事件放入请求队列。
7. 睡眠在请求队列上的某个工作线程被唤醒，它往 socket 上写入服务器处理客户请求的结果。

##### Proactor模式

Proactor 模式将所有 I/O 操作都交给主线程和内核来处理（进行读、写），工作线程仅仅负责业务逻
辑。使用异步 I/O 模型（以 aio_read 和 aio_write 为例）实现的 Proactor 模式的工作流程是：

<img src="D:\MyTxt\typoraPhoto\image-20230307103954411.png" alt="image-20230307103954411" style="zoom:67%;" />

1. 主线程调用 aio_read 函数向内核注册 socket 上的读完成事件，并告诉内核用户读缓冲区的位置，
以及读操作完成时如何通知应用程序（这里以信号为例）。
2. 主线程继续处理其他逻辑。
3. 当 socket 上的数据被读入用户缓冲区后，内核将向应用程序发送一个信号，以通知应用程序数据
已经可用。
4. 应用程序预先定义好的信号处理函数选择一个工作线程来处理客户请求。工作线程处理完客户请求
后，调用 aio_write 函数向内核注册 socket 上的写完成事件，并告诉内核用户写缓冲区的位置，以
及写操作完成时如何通知应用程序。
5. 主线程继续处理其他逻辑。
6. 当用户缓冲区的数据被写入 socket 之后，内核将向应用程序发送一个信号，以通知应用程序数据
已经发送完毕。
7. 应用程序预先定义好的信号处理函数选择一个工作线程来做善后处理，比如决定是否关闭 socket。

##### 模拟 Proactor 模式

使用同步 I/O 方式模拟出 Proactor 模式。原理是：主线程执行数据读写操作，读写完成之后，主线程向
工作线程通知这一”完成事件“。那么从工作线程的角度来看，它们就直接获得了数据读写的结果，接下
来要做的只是对读写的结果进行逻辑处理。
使用同步 I/O 模型（以 epoll_wait为例）模拟出的 Proactor 模式的工作流程如下：

<img src="D:\MyTxt\typoraPhoto\image-20230307104242846.png" alt="image-20230307104242846" style="zoom:67%;" />

1. 主线程往 epoll 内核事件表中注册 socket 上的读就绪事件。
2. 主线程调用 epoll_wait 等待 socket 上有数据可读。
3. 当 socket 上有数据可读时，epoll_wait 通知主线程。主线程从 socket 循环读取数据，直到没有更
多数据可读，然后将读取到的数据封装成一个请求对象并插入请求队列。
4. 睡眠在请求队列上的某个工作线程被唤醒，它获得请求对象并处理客户请求，然后往 epoll 内核事
件表中注册 socket 上的写就绪事件。
5. 主线程调用 epoll_wait 等待 socket 可写。
6. 当 socket 可写时，epoll_wait 通知主线程。主线程往 socket 上写入服务器处理客户请求的结果。

#### 线程池

线程池是由服务器预先创建的一组子线程，线程池中的线程数量应该和 **CPU 数量差不多**。线程池中的所
有子线程都运行着相同的代码。当有新的任务到来时，主线程将通过某种方式选择线程池中的某一个子
线程来为之服务。相比与动态的创建子线程，选择一个已经存在的子线程的代价显然要小得多。至于主
线程选择哪个子线程来为新任务服务，则有多种方式：

* 主线程使用某种算法来主动选择子线程。最简单、最常用的算法是随机算法和 Round Robin（轮流
  选取）算法，但更优秀、更智能的算法将使任务在各个**工作线程中更均匀地分配**，从而减轻服务器
  的整体压力。
* 主线程和所有子线程通过一个共享的工作队列来同步，**子线程都睡眠在该工作队列上**。当有新的任
  务到来时，主线程将任务添加到工作队列中。这将唤醒正在等待任务的子线程，不过只有一个子线
  程将获得新任务的”接管权“，它可以从工作队列中取出任务并执行之，而其他子线程将继续睡眠在
  工作队列上

<img src="D:\MyTxt\typoraPhoto\image-20230307105723891.png" alt="image-20230307105723891" style="zoom:67%;" />

> 线程池中的线程数量最直接的限制因素是中央处理器(CPU)的处理器(processors/cores)的数量
> N ：如果你的CPU是4-cores的，对于CPU密集型的任务(如视频剪辑等消耗CPU计算资源的任务)来
> 说，那线程池中的线程数量最好也设置为4（或者+1防止其他因素造成的线程阻塞）；对于IO密集
> 型的任务，一般要多于CPU的核数，因为线程间竞争的不是CPU的计算资源而是IO，IO的处理一
> 般较慢，多于cores数的线程将为CPU争取更多的任务，不至在线程处理IO的过程造成CPU空闲导
> 致资源浪费。

* 设置线程池目的：

  空间换时间，浪费服务器的硬件资源，换取运行效率。

  1. 池是一组资源的集合，这组资源在服务器启动之初就被完全创建好并初始化，这称为静态资源。
  2. 当服务器进入正式运行阶段，开始处理客户请求的时候，如果它需要相关的资源，可以直接从池中
     获取，无需动态分配。
  3. 当服务器处理完一个客户连接后，可以把相关的资源放回池中，无需执行系统调用释放资源

#### EPOLLONESHOT事件

即使可以使用 ET 模式，一个socket 上的某个事件还是可能被触发多次。这在并发程序中就会引起一个
问题。比如一个线程在读取完某个 socket 上的数据后开始处理这些数据，而在数据的处理过程中该 
socket 上又有新数据可读（EPOLLIN 再次被触发），此时另外一个线程被唤醒来读取这些新的数据。于
是就出现了两个线程同时操作一个 socket 的局面。**一个socket连接在任一时刻都只被一个线程处理**，可
以使用 epoll 的 EPOLLONESHOT 事件实现。
对于注册了 EPOLLONESHOT 事件的文件描述符，操作系统最多触发其上注册的一个可读、可写或者异
常事件，且只触发一次，除非我们使用 epoll_ctl 函数重置该文件描述符上注册的 EPOLLONESHOT 事
件。这样，当一个线程在处理某个 socket 时，其他线程是不可能有机会操作该 socket 的。但反过来思
考，注册了 EPOLLONESHOT 事件的 socket 一旦被某个线程处理完毕， 该线程就应该立即重置这个 
socket 上的 EPOLLONESHOT 事件，以确保这个 socket 下一次可读时，其 EPOLLIN 事件能被触发，进
而让其他工作线程有机会继续处理这个 socket。

#### 有限状态机

逻辑单元内部的一种高效编程方法：有限状态机（finite state machine）。
有的应用层协议头部包含数据包类型字段，每种类型可以**映射为逻辑单元的一种执行状态**，服务器可以
根据它来编写相应的处理逻辑。如下是一种状态独立的有限状态机：

```c
STATE_MACHINE( Package _pack ) 
{
    PackageType _type = _pack.GetType();
    switch( _type )
    {
        case type_A:
            process_package_A( _pack );
            break;
        case type_B:
            process_package_B( _pack );
            break;
    } 
}
```

* 这是一个简单的有限状态机，只不过该状态机的每个状态都是相互独立的，即状态之间没有相互转移。
  状态之间的转移是需要状态机内部驱动，如下代码：

```c
STATE_MACHINE() 
{
    State cur_State = type_A;
        while( cur_State != type_C )
    {
        Package _pack = getNewPackage();
        switch( cur_State )
        {
            case type_A:
                process_package_state_A( _pack );
                cur_State = type_B;
                break;
            case type_B:
                process_package_state_B( _pack );
                cur_State = type_C;
                break;
        } 
    }
```

* 该状态机包含三种状态：type_A、type_B 和 type_C，其中 type_A 是状态机的开始状态，type_C 是状
  态机的结束状态。状态机的当前状态记录在 cur_State 变量中。在一趟循环过程中，状态机先通过 
  getNewPackage 方法获得一个新的数据包，然后根据 cur_State 变量的值判断如何处理该数据包。数据
  包处理完之后，状态机通过给 cur_State 变量传递目标状态值来实现状态转移。那么当状态机进入下一
  趟循环时，它将执行新的状态对应的逻辑。

#### 服务器压力测试

Webbench 是 Linux 上一款知名的、优秀的 web 性能压力测试工具。它是由Lionbridge公司开发

* 测试处在相同硬件上，不同服务的性能以及不同硬件上同一个服务的运行状况。
* 展示服务器的两项内容：每秒钟响应请求数和每秒钟传输数据量。

基本原理：Webbench 首先 fork 出多个子进程，每个子进程都循环做 web 访问测试。子进程把访问的
结果通过pipe 告诉父进程，父进程做最终的统计结果。

* 测试示例

```c
 ./webbench -c 1000  -t  30   http://192.168.31.128:18888/index.html
权限不够重新make一下
参数：
    -c 表示客户端数
    -t 表示时间
```



## 6. 项目记录

HTTP报文头

```http
读取到数据：GET / HTTP/1.1
Host: 192.168.31.128:10000
Connection: keep-alive
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/104.0.0.0 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
Accept-Encoding: gzip, deflate
Accept-Language: zh-CN,zh;q=0.9,ar;q=0.8
```



### 1. 面试题

1. read读取完成的标志？	

   ```c
   返回值1，返回值-1且errno=EAGAIN 或者EWOULDBLOCK
       进行一些非阻塞(non-blocking)操作(对文件或socket)的时候续做read或者EWOULDBLOCK操作而没有数据可读。此时程序不会阻塞起来等待数据准备就绪返回，read函数会返回一个错误EAGAIN，提示你的应用程序现在没有数据可读请稍后再试
       又例如，当一个系统调用(比如fork)因为没有足够的资源(比如虚拟内存)而执行失败，返回EAGAIN提示其再调用一次(也许下次就能成功)
   ```

2. epoll_wait得到的events中哪些是对方异常断开或者错误事件？

   ```c
   EPOLLRDHUP|EPOLLHUP|EPOLLERR
       EPOLLRDHUP 对端关闭连接或者shutdown写入半连接
   			 （程序里close()，shell下kill或ctr+c），触发EPOLLIN和EPOLLRDHUP
   			表示对端关闭了写端，即半关闭状态，可以视为一种错误事件。当发生EPOLLRDHUP事件时，可以认为对端已经完成了写操作并关闭了连接，此时应该关闭套接字并且清理相关资源。在使用epoll时，通常可以将EPOLLRDHUP事件和EPOLLIN事件一起进行处理。
       			
       
       EPOLLHUP 	表示读写都关闭。本端调用shutdown(SHUT_RDWR),应该是本端（server端）出错才触发的。
   				对应的连接被挂起，通常是对方关闭了连接
   				通常情况下EPOLLHUP表示的是本端挂断，造成这种事件出现的原因有很多，其中一种便是出现错误，更加细致的应该是和RST联系在一起
       
       EPOLLERR      服务器这边出错,只有采取动作时，才能知道是否对方异常。
       
     //****************************  
   EPOLLHUP和EPOLLRDHUP都是表示连接异常断开的事件，但是它们的含义略有不同。
   
   EPOLLRDHUP则表示对端关闭了连接，即对端调用了close()函数。它只会在ET模式下才会触发。
   EPOLLHUP表示对端连接断开，也可能是其他错误事件，比如一个本地socket调用了shutdown之后，再往这个socket上写数据，会返回一个RST。这种情况下也会触发EPOLLHUP事件。
   在实际使用中，一般先判断EPOLLRDHUP，如果有则说明连接已经被对端关闭，如果没有再判断EPOLLHUP。
       
       -------------------------
       对于 EPOLLERR和EPOLLHUP，不需要在epoll_event时针对fd作设置，一样也会触发
       EPOLLERR和EPOLLHUP，是因为之前收到了对端close时发送的FIN 包，此时再给对端发送数据，对端会返回RST包
       
       对EPOLLRDHUP的处理应该放在EPOLLIN和EPOLLOUT前面，处理方式应该 是close掉相应的fd后，作其他应用层的清理动作
   ```

   

3. 修改，定时清理超时的请求：

   1. 定时器类 和 请求数据类 ，定时器链表；
   2. ALARM定时处理：管道写端交给sig，定期写入定时信号，读端交给epoll，如果socket是fd[0]且多EPOLLIN，说明定时到了，判断是是不是ALARM，如果是最后处理（因为清理的优先级最小），最后处理的话就是调用回调函数去删除那个数据对象，删除socket在epoll上的绑定。

## Ubuntu一些小操作

### 配置类

### 1.VScode配置 SSH连接Ubuntu

* vscode扩展remote development下载

* 远程资源管理器 设置远程连接 ，默认配置文件修改为虚拟机IP地址 用户密码

* 生成宿主机SSH秘钥     ssh-keygen -t rsa 回车   生成位于C:\Users\klChen\.ssh的私钥和公钥

* 生成虚拟机SSH秘钥    ssh-keygen -t rsa 

* 配置秘钥 
  cd .ssh
  创建文件，复制宿主机秘钥vim authorized_keys

ctrl+L 清空终端屏

Ctrl+C中断了进程，返回到终端界面。

Ctrl+Z挂起了进程，返回到终端界面。

set nu在vim中设置行号

创建文件 touch 创建文件夹 mkdir

#### 文本文件上传

​	文本文件的换行符

Windows : \r\n

Linux : \n

可以在 Notepad ++ 里观察到此区别

视图 | 显示符号 | 显示行尾符

#### 修改格式

换行符的转换：

编辑 | 文档格式转换 | 转换为 UNIX格式

注意：只有在编辑 SHELL 脚本时，才需要转换

其他格式的文件一般都不需要转换，如*.xml, *.java

**演示**：Shell脚本的编辑 。。

1. 用 Notepad++打开编辑 mytest.sh

2. 转成 Unix格式 \n

3. 上传至Linux

4. chmod +x mytest.sh

5. 运行 ./mytest.sh

### 命令类

##### 1. 归档

tar , 即 tape archive 档案打包

创建档案包

tar  -cvf  example.tar  example

其中，

  c ,  表示 create 创建档案

  v , 表示 verbose 显示详情

  f ,  表示 file

也可以多个目录打包 tar -cvf xxx.tar file1 file2 file3 

还原档案包

tar  -xvf  example.tar

tar  -xvf  example.tar  -C  outdir

其中，-C 参数指定目标目录，默认解到当前目录下

##### 2. 压缩解压

先前的tar格式并没有压缩，体积较大

并档并压缩

tar  -zcvf  example.tar.gz  example

解压缩

tar  -zxvf  example.tar.gz

tar  -zxvf  example.tar.gz  -C  outdir

通常我们所见的，都是 *.tar.gz 这种格式

##### 7z

~~~shell
sudo apt install p7zip-full
#压缩
7z a -t7z -r filename.7z ./*
#解压
7z x filename.7z -r -o./* 
~~~

##### 环境变量

定义环境变量

export OUTDIR=/opt/

显示环境变量

echo ${OUTDIR}

查看所有环境变量

printenv

##### 查看当前进程

进程id：echo $$

终端设备：tty

![image-20230227110412165](D:\MyTxt\typoraPhoto\image-20230227110412165.png)

###### 找函数

man 2 xxx

或者 man xxx +tab键

##### 查看网络相关信息的命令

netstat 
 参数：-a 所有的socket
   -p 显示正在使用socket的程序的名称
   -n 直接使用IP地址，而不通过域名服务器

 netstat -anp|grep xxxx
查看端口号占用信息





### 小知识

#### 利用Core文件查看异常的信息

1. 用ulimit -a查看Core文件允许产生的大小，一般是0；然后改为一定值 ulimit -c 1024
2. 然后用-g调试编译.c文件
3. 然后调试改文件，输入core-file core就能看到
4. 这里系统版本不同，生成不出来Core文件

#### Linux下的时间设置

时钟时间 ＝ 阻塞时间 ＋ 就绪时间 ＋运行时间；

其中：运行时间=用户CPU时间（用户的进程获得了CPU资源以后，在用户态执行的时间。）+系统CPU时间（用户进程获得了CPU资源以后，在内核态的执行时间。）；

* 因为Linux是多任务操作系统，往往在执行一条命令时，系统还要处理其它任务

##### 前后台进程

默认前台进程，阻塞

加上&改为后台进程，改为非阻塞 ，但是注意要去查看，并且kill -9：`./sigprocmask&`

##### 段错误究竟是怎么发生的？段错误的复现为什么这么难？

段错误是个迷，有的人碰到过几次，有的人怎么也碰不到，这是由于神秘莫测的调度算法导致的。【潇潇_暮雨】小伙伴提出了，这是调用了不可重入的函数。《Linux/UNIX系统编程手册》第21.1.2节 对可重入函数进行了详细的解释，有兴趣的可以去翻一下。

可重入函数的意思是：函数由两条或多条线程调用时，即便是交叉执行，其效果也与各线程以未定义顺序依次调用时一致。通俗点讲，就是存在一个函数，A线程执行一半，B线程抢过CPU又来调用该函数，执行到1/4倍A线程抢回执行权。在这样不断来回执行中，不出问题的，就是可重入函数。多线程中每个线程都有自己的堆栈，所以如果函数中只用到局部变量肯定是可重入的，没问题的。但是更新了全局变量或静态数据结构的函数可能是不可重入的。假设某线程正在为一个链表结构添加一个新的链表项，而另外一个线程也视图更新同一链表。由于中间涉及多个指针，一旦另一线程中断这些步骤并修改了相同指针，结果就会产生混乱。但是并不是一定会出现，一定是A线程刚好在修改指针，另外一线程又去修改才会出现。这就是为什么该问题复现难度较高的原因。

作者在文中指出，将静态数据结构用于内部记账的函数也是不可重入的。其中最明显的例子就是stdio函数库成员（printf()、scanf()等），它们会为缓冲区I/O更新内部数据结构。所以，如果在捕捉信号处理函数中调用了printf()，而主程序又在调用printf()或其他stdio函数期间遭到了捕捉信号处理函数的中断，那么有时就会看到奇怪的输出，设置导致程序崩溃。虽然printf()不是异步信号安全函数，但却频频出现在各种示例中，是因为在展示对捕捉信号处理函数的调用，以及显示函数中相关变量的内容时，printf()都不失为一种简单而又便捷的方式。真正的应用程序应当避免使用该类函数。

printf函数会使用到一块缓冲区，这块缓冲区是使用malloc或类似函数分配的一块静态内存。所以它是不可重入函数

#### 虚拟地址空间层次划分

从操作系统层级上看，虚拟地址空间主要分为两个部分内核区和用户区。

**一、内核区**

1. 内核空间为内核保留，**不允许应用程序读写该区域的内容**或直接调用内核代码定义的函数
2. **内核总是驻留在内存中**，是操作系统的一部分。
3. 系统中**所有进程**对应的**虚拟地址空间的内核区**都会**映射到同一块物理内存**上（**系统内核只有一个**）

**二、用户区**

**每个进程的虚拟地址空间都是从 0 地址开始的**，我们在程序中打印的变量地址也其在虚拟地址空间中的地址，程序是无法直接访问物理内存的。虚拟地址空间中用户区地址范围是 0~3G（以 32 位系统的虚拟地址空间为例），里边分为多个区块。

各分区由低地址到高地址依次是：

1. **保留区:** 位于虚拟地址空间的最底部，**未赋予物理地址**。任何对它的引用都是非法的，程序中的**空指针（NULL）指向的就是这块内存地址**。
2. **.text段: 代码段也称正文段或文本段**，通常用于存放程序的**执行代码** (即 CPU 执行的**机器指令，二进制**)，代码段一般情况下是**只读**的，这是对执行代码的一种保护机制。
3. **.data段**: **数据段**通常用于存放程序中**已初始化且初值不为 0 的全局变量和静态变量**。数据段属于**静态内存分配 (静态存储区)**，可读可写。
4. **.bss段: 未初始化以及初始为 0 的全局变量和静态变量**，操作系统会将这些**未初始化变量初始化为 0**；
5. **堆(heap)**：用于存放进程运行时**动态分配的内存**。
   - 堆中内容是匿名的，不能按名字直接访问，只能通过指针间接访问。
   - 堆向高地址扩展 (即 “向上生长”)，是**不连续**的内存区域。这是由于系统用链表来存储空闲内存地址，自然不连续，而**链表从低地址向高地址遍历**。
6. **内存映射区(mmap)：\**作为内存映射区\**加载磁盘文件**，或者**加载程序运作过程中需要调用的动态库**。
7. **栈(stack):** 存储**函数内部声明的非静态局部变量，函数参数，函数返回地址等信息**，栈内存由**编译器自动分配释放**。**栈**和堆相反地址 **“向下生长”**，分配的**内存是连续**的。
8. **命令行参数**：存储进程执行的时候**传递给 main() 函数的参数，argc，argv []**
9. **环境变量**: 存储和进程相关的环境变量，比如：**工作路径**，**进程所有者**等信息



## C函数记录

~~~c
int fprintf(FILE *stream, const char *format, ...)；

第一个参数：（buffer）
这个参数就是接收字符串的字符数组。其大小必须要大于所接收的字符串的大小，否则的话会有空间不够从而导致内存溢出的风险。（这里比较大小时还要考虑到字符串最后的 ‘\0’）

第二个参数：（format）
这个参数就是要传的字符串了。

其余参数：
剩下的参数其实算是对第二个参数format的补充，可有可无，视情况而定
~~~

~~~c
#include <stdio.h>
perror("xxx")
作用：调用系统或者库函数有错的时候，发出错误信息
~~~

 	sizeof 和strlen 有本质上的区别。sizeof 是C 语言的一种单目运算符，如++、–等，并不是函数，sizeof 的优先级为2 级，比/、% 等3 级运算符优先级高，sizeof以字节的形式给出操作数的存储空间的大小。而 strlen 是一个函数，是由 C 语言的标准库提供的。strlen 计算的 是字符串的长度。
```c
sprintf 
    int sprintf( char *buffer, const char *format [, argument] … );
    跟 printf 在用法上几乎一样，只是打印的目的地不同而已，前者打印到字符串中，后者则直接在命令行上输出;
1. 可以用sprintf来将其他类型转换字符串类想
2. 可以生成字符串，拼接 格式等
     sprintf(buf,"hello,%d\n",i);

    
```

##### 可变参数

输入一串格式化的字符串，经过处理后可以将 %s %f %d等占位符替换为对应的数据；

```c
… 表示函数的参数个数可变，典型的如printf()

第一个参数是一个格式化字符串，后面是与格式化字符串中的代码相对应的不同类型的多个参数。

char* func(const char* format, ...)  // 重载了func函数，不重载也行
{
    va_list ap;
    char *res = NULL; 
    va_start(ap, format);
    res = func(format, ap);  
    va_end(ap);
    return res ;  
} 

1.        int vsprintf(char *str, const char *format, va_list ap);函数与sprintf()函数对应，只是在函数调用时，把上面的...对应的一个个变量用va_list调用所替代。在函数调用前ap要通过va_start()宏来动态获取。
2. 结构体va_list用来存参数列表，
```

```c
struct iovec  //I/O vector，与readv和wirtev操作相关的结构体
#include <sys/uio.h>
/* Structure for scatter/gather I/O. */
struct iovec{
     void *iov_base; /* Pointer to data. */
     size_t iov_len; /* Length of data. */
};
成员iov_base指向一个缓冲区，这个缓冲区是存放readv所接收的数据或是writev将要发送的数据。
成员iov_len确定了接收的最大长度以及实际写入的长度。   
readv和writev函数用于在一次函数调用中读、写多个非连续缓冲区。有时也将这两个函数称为散布读（scatter read）和聚集写（gather write）
    
HTTP响应的时候配合writev(),写入响应头和体
    writev(m_sockfd,m_iv,m_iv_count);
```



## 虚拟机设置

IPv4 192.168.31.128

##### 配置文件

* 配置文件名以 *.cnf 为后缀,参考 windows下mysql的配置 : my.ini

* 配置文件都在/etc下，在 /etc/init.d 下，是各个系统服务的启动脚本2 软件包搜索

apt list | grep ssh

apt search ssh --names-only

3 删除软件包

apt remove xxx 卸载软件包

apt purge xxx 卸载软件包、并清除配置文件

~~~shell
echo命令的功能是在显示器上显示一段文字，一般起到一个提示的作用。此外，也可以直接在文件中写入要写的内容。也可以用于脚本编程时显示某一个变量的值，或者直接输出指定的字符串。
1. -n : 表示输出之后不换行，直接显示新行的提示符
2. -e : 表示对于转义字符按对应的方式进行处理。
3. echo “想要的内容”> 文件名：
将想要的内容覆盖到对应的文件当中去，文件当中之前的内容不复存在了，实际上是修改了原文件的内容。
4. echo “想要的内容”>> 文件名
将想要的内容追加到文件后，对文件之前的内容不修改，只进行增添，也叫追加重定
~~~

~~~c
void *memcpy(void *dest, const void *src, size_t n);
内存拷贝
~~~

## 小练习

```c
#ifndef THREAD_POOL_H
#define THREAD_POLL_H

#include "locker.h"
#include <list>
#include <pthread.h>
#include <exception>
#include <cstdio>
//定义线程池模板类，方便复用
template<typename T>
class threadpool{
public:
    threadpool(int thread_number=8,int max_requests=10000);
    ~threadpool();
    bool append(T* request);    //添加到请求队列中
    
private:
    //只能是静态函数
    static void* work(void*arg);
    void run();
    
private:
    int m_thread_number; //线程数量
    pthread_t* m_threads;//线程数组
    int m_max_requests;//允许等待的最大量
    std::list<T*> m_workqueue; //请求队列
    locker m_queuelocker;//互斥锁
    sem m_queuestat;// 信号量用来判断是否有任务需要处理
    bool m_stop;//是否结束线程
};

template<typename T>
threadpool<T>::threadpool(int thread_number,int max_requests):
            m_thread_number(thread_number),m_max_requests(m_max_requests),
            m_stop(false),m_threads(NULL) {
        if((thread_number <= 0) || (max_requests <= 0)){
            throw std::exception();
        }
        m_threads = new pthread_t[m_thread_number];
        if(!m_threads){
            throw std::exception();
        }

        //创建thread_number个线程，线程分离
        for(int i=0;i!=thread_number;i++){
            printf("create the %d thread\n",i);
            //“值得一提的是，在c++程序中使用pthread_creat时，该函数的第3个参数必须指向一个静态函数”
            //第四个参数 -向work传参this 用于调用非静态成员
            if(pthread_create(m_threads+i,NULL,work,this) != 0){
                delete[] m_threads;
                throw std::exception();
            }

            if(pthread_detach(m_threads[i]) != 0){
                delete[] m_threads;
                throw std::exception();
            }
        }
}

template<typename T>
threadpool<T>::~threadpool(){
    delete[] m_threads;
    m_stop = true;
}

template<typename T>
bool threadpool<T>::append(T* request){
    m_queuelocker.lock();
    if(m_workqueue.size() > m_max_requests){
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template<typename T>
void* threadpool<T>::work(void*arg){
    //静态函数不能访问类内非静态成员,可以传入this指针
    threadpool *pool = (threadpool*) arg;
    pool->run();
    return pool;
}

template<typename T>
void threadpool<T>::run(){
    while(!m_stop){
        //P
        m_queuestat.wait();
        //上锁
        m_queuelocker.lock();
        //其实这里wait会阻塞
        if(m_workqueue.empty()){
            m_queuelocker.unlock();
            continue;
        }

        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        //解锁
        m_queuelocker.unlock();

        if(!request){
            continue;
        }

        request->process();
    }
    
}

#endif

```

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include "locker.h"
#include "thread_pool.h"
#include "http_conn.h"

#define MAX_FD 65535 //最大文件描述符
#define MAX_EVENTS_NUMBER 10000 //监听的最大的事件数量

//将文件描述符添加到epoll中
extern void addfd(int epollfd,int fd,bool one_shoot);
//将文件描述符从epoll中删除
extern void removefd(int epollfd,int fd);
//修改文件描述符
extern void modfd(int epollfd,int fd,int ev);

//添加信号捕捉 
void addsig(int sig,void(handler)(int)){
    struct sigaction sa;
    //清空
    memset(&sa,'\0',sizeof(sa));
    //设置回调函数
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);
    //注册信号捕捉
    sigaction(sig,&sa,NULL);
}


int main( int argc, char* argv[] ) {
    
    if( argc <= 1 ) {
        printf( "usage: %s port_number\n", basename(argv[0]));
        return 1;
    }

    //获取端口号
    int port =atoi(argv[1]);

    //对SIGPIE信号进行处理
    addsig(SIGPIPE,SIG_IGN);

    //初始化线程池
    threadpool<http_conn> *pool = NULL;
    try{
        pool = new threadpool<http_conn>;
    }catch(...){
        exit(-1);
    }

    //创建一个数组，用于保存所有的客户端信息
    http_conn *users = new http_conn[MAX_FD];
    
    //生成监听的socket
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    
    //设置端口复用
    int reuse = 1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));

    //绑定
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(listenfd,(struct sockaddr*)&address,sizeof(address));

    //监听
    listen(listenfd,5); 

    //创建epoll对象,事件数组，之后添加
    epoll_event events[MAX_EVENTS_NUMBER];
    int epollfd = epoll_create(100);

    //监听文件描述符添加到epoll实例当中
    addfd(epollfd,listenfd,false);
    http_conn::m_epollfd = epollfd;

    while(1){
        int num = epoll_wait(epollfd,events,MAX_EVENTS_NUMBER,-1);
        if((num < 0) && (errno != EINTR)){
            printf("epoll failure\n");
            break;
        }

        //循环遍历事件数组
        for(int i = 0; i < num; i++){

            int sockfd = events[i].data.fd;

            if(sockfd == listenfd){
                //有客户端连接进来
                printf("new client\n");
                struct sockaddr_in client_address;
                socklen_t client_addlen = sizeof(client_address);

                int connfd = accept(listenfd,(struct sockaddr*)&client_address,&client_addlen);
                if(connfd == -1){
                    printf("error is: %d\n",errno);
                    continue;
                }
                if(http_conn::m_user_count >=MAX_FD){
                    //目前连接数满了
                    close(connfd);
                    continue;
                }
                //将新的客户端初始化，放到数组中
                users[connfd].init(connfd,client_address); 

            }else if(events[i].events & (EPOLLRDHUP|EPOLLHUP|EPOLLERR)){
                //对方异常断开或者错误等事件
                users[sockfd].close_conn();
                printf("EPOLLRDHUP|EPOLLHUP|EPOLLERR while epoll\n");

            }else if(events[i].events & EPOLLIN){
                //读事件发生
                if(users[sockfd].read()){
                    //一次性把数据读完
                    pool->append(users+sockfd);
                    printf("read ok\n");
                }else{
                    users[sockfd].close_conn();
                }
            }else if(events[i].events & EPOLLOUT){
                if(!users[sockfd].write()){
                    //把respond写入客户端端fd后，把该客户端fd从epoll中删除
                    users[sockfd].close_conn();
                }
                printf("write response ok\n");
                printf("-----------------------\n");
            }
        }



    }
    close(epollfd);
    close(listenfd);
    delete[] users;
    delete pool;

    return 0;
}
```

```c
//使用#ifndef可以避免以下错误:如果在.h文件中定义了全局变量,一个C文件包含了.h文件多次,如果不加#ifndef宏定义,会出现变量重复定义的错误;如果加了#ifndef则不会出现这种错误
#ifndef LOCKER_H
#define LOCKER_H

#include <pthread.h>
#include <exception>
#include <semaphore.h>
//线程同步封装类

//互斥锁类
class locker{
public:
    locker(){
        if(pthread_mutex_init(&m_mutex,NULL) != 0){
            throw std::exception();
        }
    }
    ~locker(){
        pthread_mutex_destroy(&m_mutex);
    }
    inline bool lock(){
        return pthread_mutex_lock(&m_mutex) == 0; 
    }
    inline bool unlock(){
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
    pthread_mutex_t& get(){
        return m_mutex;
    } 

private:
    pthread_mutex_t m_mutex;
};

//条件变量类
class cond{
public:
    cond(){
        if(pthread_cond_init(&m_cond,NULL) != 0){
            throw std::exception();
        }
    }
    ~cond(){
        pthread_cond_destroy(&m_cond);
    }

    inline bool wait(pthread_mutex_t* mutex){
        return pthread_cond_wait(&m_cond,mutex)==0;
    }

    inline bool timewait(pthread_mutex_t* mutex,struct timespec t){
        return pthread_cond_timedwait(&m_cond,mutex,&t)==0;
    }

    inline bool signal() {
        return pthread_cond_signal(&m_cond)==0;
    }

    inline bool broadcast(pthread_mutex_t mutex){
        return pthread_cond_broadcast(&m_cond)==0;
    }

private:
    pthread_cond_t m_cond;
};

//信号量类
class sem{
public:
    sem()=default;
    sem(int num){
        if(sem_init(&m_sem,0,num) != 0){
            throw std::exception();
        }
    }
    ~sem(){
        sem_destroy(&m_sem);
    }
    //加锁P
    bool wait(){
        return sem_wait(&m_sem)==0;
    }
    //减锁V
    bool post(){
        return sem_post(&m_sem)==0;
    }
private:
    sem_t m_sem;
};

#endif
```

```c
#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/uio.h>
#include <string.h>
#include "locker.h"


class http_conn{
public:

    static int m_epollfd;    //所有socket事件都被注册到同一个epollfd上
    static int m_user_count; //统计用户的数量
    static const int FILENAME_LEN = 200;         //文件名的最大长度
    static const int READ_BUFFER_SIZE = 2048;    //读缓冲区的大小
    static const int WRITE_BUFFER_SIZE =1024;    //写缓冲区大小 

    // HTTP请求方法，这里只支持GET
    enum METHOD {GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT};
    
    /*
        解析客户端请求时，主状态机的状态
        CHECK_STATE_REQUESTLINE:当前正在分析请求行
        CHECK_STATE_HEADER:当前正在分析头部字段
        CHECK_STATE_CONTENT:当前正在解析请求体
    */
    enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
    
    /*
        服务器处理HTTP请求的可能结果，报文解析的结果
        NO_REQUEST          :   请求不完整，需要继续读取客户数据
        GET_REQUEST         :   表示获得了一个完成的客户请求
        BAD_REQUEST         :   表示客户请求语法错误
        NO_RESOURCE         :   表示服务器没有资源
        FORBIDDEN_REQUEST   :   表示客户对资源没有足够的访问权限
        FILE_REQUEST        :   文件请求,获取文件成功
        INTERNAL_ERROR      :   表示服务器内部错误
        CLOSED_CONNECTION   :   表示客户端已经关闭连接了
    */
    enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };
    
    // 从状态机的三种可能状态，即行的读取状态，分别表示
    // 1.读取到一个完整的行 2.行出错 3.行数据尚且不完整
    enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };

    http_conn(){}
    ~http_conn(){}

    void init(int sockfd,const sockaddr_in &add);   //初始化新收到的客户端的连接
    void close_conn();
    bool read();    //非阻塞的读
    bool write();   //非阻塞的写

    void process();     //处理用户请求
  
private:

    void init();    //初始化连接其余的数据
    HTTP_CODE process_read();                  //解析HTTP请求
    bool process_write(HTTP_CODE ret);         //写入HTTP响应
    
    
    //这组函数被process_read调用以分析HTTP请求
    HTTP_CODE parse_request_line(char *text);       //解析HTTP请求首行
    HTTP_CODE parse_headers(char *text);            //解析HTTP头部
    HTTP_CODE parse_content(char *text);            //解析HTTP内容
    LINE_STATUS parse_line();                       //解析行  
    inline char* get_line() {return m_read_buf + m_start_line;}
    HTTP_CODE do_request();

 
    //这组函数被process_read调用以填充HTTP应答
    void unmap();
    bool add_response(const char* format,...);
    bool add_content(const char* content,...);
    bool add_content_type();
    bool add_status_line(int status,const char*title);
    bool add_headers(int content_length);
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();



private:
    int m_sockfd;   //该HTTP连接的socket
    sockaddr_in m_address;  //通信的socket地址

    char m_read_buf[READ_BUFFER_SIZE];      //读缓冲区
    int m_read_idx;                         //标识读缓冲区中已经写入的数据的最后一个字节的下一个
    
    char m_real_file[FILENAME_LEN];         // 客户请求的目标文件的完整路径，其内容等于 doc_root + m_url, doc_root是网站根目录
    int m_check_index;                      //当前正在分析的字符在缓冲区的位置
    int m_start_line;                       //当前正在解析的行的起始位置  
    char *m_url;                            //目标文件名
    char *m_version;                        //协议版本，只支持HTTP1.1  
    char *m_host;                            //主机名
    bool m_linger;                          //HTTP是否保持连接 keep alive
    int m_content_length;                   //HTTP请求的消息总长度

    METHOD m_method;                         //请求方法
    CHECK_STATE m_check_state;               //主状态机当前所处的状态

    char m_write_buf[WRITE_BUFFER_SIZE];      //写缓冲区
    int m_write_idx;                          //写缓冲区的待处理字节
    char* m_file_address;                     //客户请求的目标地址
    struct stat m_file_stat;                  //目标文件的状态
    struct iovec m_iv[2];                      //采用writev来执行写操作，所以定义下面两个成员，其中m_iv_count表示被写内存块的数量。 
    int m_iv_count;

    int bytes_to_send;                      //将要发送的数据的字节数
    int bytes_have_send;                    //已经发送的字节数

};

#endif

```

```c

#include "http_conn.h"

// 定义HTTP响应的一些状态信息
const char* ok_200_title = "OK";
const char* error_400_title = "Bad Request";
const char* error_400_form = "Your request has bad syntax or is inherently impossible to satisfy.\n";
const char* error_403_title = "Forbidden";
const char* error_403_form = "You do not have permission to get file from this server.\n";
const char* error_404_title = "Not Found";
const char* error_404_form = "The requested file was not found on this server.\n";
const char* error_500_title = "Internal Error";
const char* error_500_form = "There was an unusual problem serving the requested file.\n";

// 网站的根目录
const char* doc_root = "/home/klchen/WinToUbuntu/WebServer/resources";

// 类内静态变量初始化
int http_conn::m_user_count = 0;
int http_conn::m_epollfd = -1;

//设置fd非阻塞
int setnonblocking(int fd){
    int old_flag = fcntl(fd,F_GETFL);
    int new_flag = old_flag | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_flag);
    return new_flag;
}

//将需要监听的文件描述符添加到epoll中
void addfd(int epollfd,int fd,bool one_shoot){
    struct epoll_event event;
    event.data.fd = fd;
    //event.events = EPOLLIN ||EPOLLRDHUP;
    event.events = EPOLLIN |EPOLLRDHUP;
    //设置一个Socket连接同时只能触发一次，防止读取的时候更新又开一个线程处理
    if(one_shoot){
        event.events|=EPOLLONESHOT;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);

    //设置文件描述符非阻塞
    setnonblocking(fd);
}

//将监听文件描述符从epoll中删除
void removefd(int epollfd,int fd){
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
    close(fd);
}

//修改文件描述符 ，修改socket上的ONESHOOT事件，以确保下一次可读的时候，EPOLL时间只触发一次
void modfd(int epollfd,int fd,int ev){
    epoll_event event;
    event.data.fd = fd;
    //为什么要添加 EPOLLET|EPOLLRDHUP？？？
    event.events = ev|EPOLLONESHOT|EPOLLET|EPOLLRDHUP;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);
}

//初始化连接
void http_conn::init(int sockfd, const sockaddr_in &add){
    m_sockfd = sockfd;
    m_address = add;

    //端口复用
    int reuse = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
    
    //新的客户端fd添加到epoll对象中
    addfd(m_epollfd,sockfd,true);
    m_user_count++;

    //其他部分的初始化
    init();
}

//其他部分的初始化
void http_conn::init()
{
    m_check_state = CHECK_STATE_REQUESTLINE;    //初始化状态为解析请求首行
    m_check_index = 0;
    m_start_line = 0;
    m_read_idx = 0;
    m_url = 0;    
    m_version = 0;     
    m_method = GET;
    bzero(m_read_buf,READ_BUFFER_SIZE);
    m_host = 0;
    m_linger = false;
    bytes_to_send = 0;
    bytes_have_send = 0;
    m_content_length = 0;
    m_read_idx = 0;
    m_write_idx = 0;

    bzero(m_read_buf,READ_BUFFER_SIZE);
    bzero(m_write_buf,WRITE_BUFFER_SIZE);
    bzero(m_read_buf,FILENAME_LEN);
}

//关闭连接
void http_conn::close_conn(){
    if(m_epollfd != -1){
        removefd(m_epollfd,m_sockfd);
        m_sockfd = -1;
        m_user_count--;//关闭一个连接
    }

}

bool http_conn::read()
{
    //printf("一次性读完数据\n");
    if(m_read_idx >= READ_BUFFER_SIZE){
        return false;
    }

    int bytes_read = 0;
    while(1){
        //第四个参数设置0，表示阻塞读取
        bytes_read = recv(m_sockfd,m_read_buf+m_read_idx,READ_BUFFER_SIZE-m_read_idx,0);
        if(bytes_read == -1){
            if((errno == EAGAIN)||(errno ==EWOULDBLOCK)){
                //非阻塞情况下，没数据可读了
                break;
            }
            return false;
        }else if(bytes_read == 0){
            //对方关闭
            return false;
        }else if(bytes_read > 0){
            m_read_idx += bytes_read;
        }
    }
    printf("%s\n",m_read_buf);
    return true;
}


/* 当得到一个完整、正确的HTTP请求时，我们就分析目标文件的属性，
 如果目标文件存在、对所有用户可读，且不是目录，则使用mmap将其
 映射到内存地址m_file_address处，并告诉调用者获取文件成功
*/
http_conn::HTTP_CODE http_conn::do_request()
{   
    //或得路径+文件名
    strcpy(m_real_file,doc_root);
    int len = strlen(doc_root);
    strncpy(m_real_file+len,m_url,FILENAME_LEN-1);

    //获取文件权限
    if(stat(m_real_file,&m_file_stat) < 0){
        return  NO_RESOURCE;
    }
    
    if(!(m_file_stat.st_mode & S_IROTH)){
        return FORBIDDEN_REQUEST;
    }

    //判断是否是目录
    if(S_ISDIR(m_file_stat.st_mode)){
        return BAD_REQUEST;
    }

    //只读方式打开文件
    int fd = open(m_real_file,O_RDONLY);

    //创建内存映射，写入
    m_file_address = (char *)mmap(NULL,m_file_stat.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    close(fd);
    return FILE_REQUEST;
}

// 主状态机 解析请求
http_conn::HTTP_CODE http_conn::process_read()
{
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;

    char *text = 0;
    while(((m_check_state == CHECK_STATE_CONTENT) &&(line_status == LINE_OK))
        ||((line_status = parse_line()) == LINE_OK)){
    //解析到了一行完整的数据，或者解析到了一行完整的请求体
        //获取一行数据
        text = get_line();
        m_start_line = m_check_index;
        printf("got 1 http line: %s\n",text);

        switch (m_check_state)
        {
            case CHECK_STATE_REQUESTLINE:{
                ret = parse_request_line(text);
                if(ret == BAD_REQUEST){
                    return BAD_REQUEST;
                }
                break;
            }
            case CHECK_STATE_HEADER:{
                ret = parse_headers(text);
                if(ret == BAD_REQUEST){
                    return BAD_REQUEST;
                }else if(ret == GET_REQUEST){
                    //头部解析完了，得到一个完整的客户请求
                    return do_request();
                }
                break;
            }
            case CHECK_STATE_CONTENT:{
                ret = parse_content(text);
                if(ret == GET_REQUEST){
                    return do_request();
                }
                line_status = LINE_OPEN;            
                break;
            }
            default:{
                return INTERNAL_ERROR;
            }
        }
    }
    return NO_REQUEST;
}

//解析HTTP请求行，获得请求方法，目标URL，HTTP版本
http_conn::HTTP_CODE http_conn::parse_request_line(char *text)
{   //读取到数据：GET /index.html HTTP/1.1
        //strpbrk是在源字符串（s1）中找出最先含有搜索字符串（s2）中任一字符的位置并返回，若找不到则返回空指针
    m_url = strpbrk(text," \t");    //text中空格和\t哪个先得到
    if(!m_url){
        return BAD_REQUEST;
    }
        //strcasecmp用来比较参数s1和s2字符串前n个字符，比较时会自动忽略大小写的差异,strncasecmp有n指定比较最长位置    
    *m_url++ = '\0';///index.html HTTP/1.1
    char *method = text;//GET /index.html HTTP/1.1

    if(strcasecmp(method,"GET") == 0){
        m_method = GET;
    }else{
        return BAD_REQUEST;
    }  
    
    m_version = strpbrk(m_url," \t");
    if(!m_version){
        return BAD_REQUEST;
    }
    *m_version++ = '\0';//HTTP/1.1
    
    if(strcasecmp(m_version,"HTTP/1.1")!= 0 ){
        return BAD_REQUEST;
    }
    //http://192.168.31.128:10000/index.html
    if(strncasecmp(m_url,"http://",7) == 0){
        m_url+=7;// 192.168.31.128:10000/index.html
        m_url=strchr(m_url,'/'); // /index.html
    }

    if(!m_url || (m_url[0] !='/')){
        return BAD_REQUEST;
    }

    m_check_state = CHECK_STATE_HEADER; //主状态机检查状态变为请求头

    return NO_REQUEST;
}

http_conn::HTTP_CODE http_conn::parse_headers(char *text)
{
    //遇到空行，表示头部字段解析完成
    if(text[0] == '\0'){
        //如果HTTP请求有消息体，还需要读取m_content_length字节的消息体
        //状态机转移到CHECK_STATE_CONTENT状态
        if(m_content_length != 0){
            m_check_state = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        //否则，说明已经得到完整的HTTP请求,继续获取请求
        return GET_REQUEST;
    }else if(strncasecmp(text,"Connection:",11)==0){
        //处理Connection:头部字段 Connection: keep-alive
        text += 11;
        text += strspn(text," \t");
        if( strcasecmp(text,"keep-alive")==0){
            m_linger = true;
        }
    } else if ( strncasecmp( text, "Content-Length:", 15 ) == 0 ) {
        // 处理Content-Length头部字段
        text += 15;
        text += strspn( text, " \t" );
        m_content_length = atol(text);
    } else if ( strncasecmp( text, "Host:", 5 ) == 0 ) {
        // 处理Host头部字段
        text += 5;
        text += strspn( text, " \t" );
        m_host = text;
    } else {
        printf( "oop! unknow header %s\n", text );
    }
    return NO_REQUEST;
}

//本项目没有真正解析HTTP请求的消息体，只是判断它是否被完整的读入了
http_conn::HTTP_CODE http_conn::parse_content(char *text)
{
    if( m_read_idx >=(m_content_length + m_check_index)){
        text[m_content_length] = '\0';
        return GET_REQUEST;
    }
    return HTTP_CODE();
}

// 次状态机，解析行
http_conn::LINE_STATUS http_conn::parse_line()
{
    char temp;
    for(;m_check_index < m_read_idx;++m_check_index){
        temp = m_read_buf[m_check_index];
        //考虑Linux的换行符是\r\n
        if(temp == '\r'){
            //读到换行符的清空
            if((m_check_index + 1)== m_read_idx){
                //到缓存最后了
                return LINE_OPEN;
            }else if(m_read_buf[m_check_index+1]=='\n'){
                //完整解析一行了
                m_read_buf[m_check_index++] = '\0';
                m_read_buf[m_check_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }else if(temp == '\n'){
            //读了一半换行符
            if((m_check_index > 1) && (m_read_buf[m_check_index -1] == '\r')){
                m_read_buf[m_check_index -1] ='\0';
                m_read_buf[m_check_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;   
        }

    }

    return LINE_OK;
}

//向缓冲区写入待发送的数据
bool http_conn::add_response(const char *format, ...)
{   
    if(m_write_idx >= WRITE_BUFFER_SIZE){
        return false;
    }
    va_list arg_list;
    va_start(arg_list,format);
    int len = vsnprintf(m_write_buf+m_write_idx,WRITE_BUFFER_SIZE-1-m_write_idx,format,arg_list);
    if(len >= (WRITE_BUFFER_SIZE-1-m_write_idx)){
        return false;
    }
    m_write_idx += len;
    va_end(arg_list);
    return true;
}

bool http_conn::add_content(const char *content, ...)
{
    return add_response( "%s", content );
}

bool http_conn::add_content_type()
{
    return add_response("Content-Type:%s\r\n", "text/html");
}

bool http_conn::add_status_line(int status, const char *title)
{
    return add_response("%s %d %s\r\n","HTTP/1.1",status,title);
}

bool http_conn::add_headers(int content_length)
{
    
    return (
        add_content_length(content_length) &&
        add_content_type()&&
        add_linger()&&
        add_blank_line() 
    );
}

bool http_conn::add_content_length(int content_length)
{
    return add_response("Content-Length: %d\r\n",content_length);
}

void http_conn::unmap()
{
    if(m_file_address){
        munmap(m_file_address,m_file_stat.st_size);
        m_file_address = 0;
    }
}

//写HTTP响应
bool http_conn::write()
{
    int temp =0;
    //写完了
    if(bytes_to_send  == 0){
        modfd(m_epollfd,m_sockfd,EPOLLIN);
        init();
        return true;
    }

    while(1){
        //分散写，有两个内存块，响应头+体
        temp = writev(m_sockfd,m_iv,m_iv_count);
        if(temp < -1){
             // 如果TCP写缓冲没有空间，则等待下一轮EPOLLOUT事件，虽然在此期间，
            // 服务器无法立即接收到同一客户的下一个请求，但可以保证连接的完整性。
            if(errno == EAGAIN){
                modfd(m_epollfd,m_sockfd,EPOLLOUT);
            }
            //释放内存映射
            unmap();
            return false;
        }

        bytes_have_send += temp;//bytes_have_send不计较不同内存块的
        bytes_to_send -= temp;
        
        if(bytes_have_send >= m_iv[0].iov_len){
            m_iv[0].iov_len = 0;
            m_iv[1].iov_base = m_file_address+(bytes_have_send-m_write_idx);
            m_iv[1].iov_len = bytes_to_send;
        }else{
            m_iv[0].iov_base = m_write_buf + bytes_have_send;
            m_iv[0].iov_len = m_iv[0].iov_len-temp;
        }

        if(bytes_to_send <= 0){
            //没有数据可以发了
            unmap();
            modfd(m_epollfd,m_sockfd,EPOLLIN);

            if(m_linger){
                init();
                return true;
            }else{
                return false;
            }
        }
    }
}

bool http_conn::add_linger()
{
    return add_response( "Connection: %s\r\n", ( m_linger == true ) ? "keep-alive" : "close" );
}

bool http_conn::add_blank_line()
{
    return add_response( "%s", "\r\n" );
}

//根据服务器处理HTTP请求的结果，决定返回给客户端的内容
bool http_conn::process_write(HTTP_CODE ret)
{    
    switch (ret)
    {
    case INTERNAL_ERROR:
        add_status_line(500,error_500_title);
        add_headers(strlen(error_500_form));
        if( ! add_content(error_500_form)){
            return false;
        }
        break;
    case BAD_REQUEST:
        add_status_line( 400, error_400_title );
        add_headers( strlen( error_400_form ) );
        if ( ! add_content( error_400_form ) ) {
            return false;
        }
        break;
    case NO_RESOURCE:
        add_status_line( 404, error_404_title );
        add_headers( strlen( error_404_form ) );
        if ( ! add_content( error_404_form ) ) {
            return false;
        }
        break;
    case FORBIDDEN_REQUEST:
        add_status_line( 403, error_403_title );
        add_headers(strlen( error_403_form));
        if ( ! add_content( error_403_form ) ) {
            return false;
        }
        break;
    case FILE_REQUEST:
        add_status_line(200,ok_200_title);
        add_headers(m_file_stat.st_size);
        //存储用于分写的内存位置
        m_iv[0].iov_base =  m_write_buf;
        m_iv[0].iov_len = m_write_idx;
        m_iv[1].iov_base = m_file_address;
        m_iv[1].iov_len = m_file_stat.st_size;
        m_iv_count = 2;

        bytes_to_send = m_write_idx + m_file_stat.st_size;

        if(m_write_buf){
            printf("%s",m_write_buf); 
        }else{
            printf("m_write_buf is empty!\n"); 
        }          
            return true;
    default:
        return false;
    }

    m_iv[0].iov_base = m_write_buf;
    m_iv[0].iov_len = m_write_idx;
    m_iv_count = 1;
    bytes_have_send = m_write_idx;

    if(m_write_buf){
        printf("%s",m_write_buf); 
    }else{
        printf("m_write_buf is empty!\n"); 
    }      
    
    return true;
}

//有线程池中的工作线程调用，处理HTTP请求的入口函数
void http_conn::process()
{   
    //解析HTTP请求
    HTTP_CODE read_ret = process_read();
    if(read_ret == NO_REQUEST){
        //请求不完整，重新修改请求
        modfd(m_epollfd,m_sockfd,EPOLLIN);
        return;
    }
    //生成响应      
    bool write_ret = process_write(read_ret);
    if(!write_ret){
        close_conn();
    }
    printf("Generate response sucessfully\n");
    modfd(m_epollfd,m_sockfd,EPOLLOUT);
}
```











