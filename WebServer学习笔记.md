# WebServer知识



## C++ 11之后标准

### Using typename

```c++
using return_type=typename std::result_of<F(Args...)>:: type;
//这里的typename是为了说明后面是个类型,因为后面是个嵌套从属类型
```

### std::optional——C++17

std::optional<具体类型> 是一个包含0或1个元素的容器。作为返回值可以是返回**std::nullopt**，也可以是返回**具体类型**。

主要好处：省去了运行状态的 bool 值的声明，直接判断res == std::nullopt让代码更简洁；
其他好处：更注重返回值本身的语意，不用担心额外的动态内存分配。

### std::tuple

​		std::tuple是类似pair的模板。每个pair的成员类型都不相同，但每个pair都恰好有两个成员。不同std::tuple类型的成员类型也不相同，但**一个std::tuple可以有任意数量的成员**。每个确定的std::tuple类型的成员数目是固定的，但一个std::tuple类型的成员数目可以与另一个std::tuple类型不同。
但我们希望将一些数据组合成单一对象，但又不想麻烦地定义一个新数据结构来表示这些数据时，std::tuple是非常有用的。我们可以将std::tuple看作一个”快速而随意”的数据结构

###  std::thread

相比于 `thread_create`，`std::thread` 更加易用和安全，具有以下优点：

1. `std::thread` 是 C++ 标准库中的线程库，可以跨平台使用，并且不需要在代码中包含特定的头文件。
2. `std::thread` 允许用户以函数或可调用对象的形式传递参数，不需要手动打包成一个结构体。
3. `std::thread` 对于线程的管理更加简单和安全。当一个 `std::thread` 对象销毁时，它会自动等待线程结束并回收资源。
4. `std::thread` 支持更加丰富的线程操作，如线程间通信、线程同步、线程池等。

### std::condition_variable

 C++ 标准库提供的用于线程同步的条件变量。它可以与一个互斥量（`std::mutex`）一起使用，实现线程等待某个条件成立的机制。

常用的成员函数有：

- `wait(lock, pred)`：等待一个条件变量，直到被通知或超时。其中，`lock` 是一个已经被锁住的互斥量，用于保护等待的操作，`pred` 是一个可调用对象，用于判断是否满足等待的条件。
- `notify_one()`：通知等待在条件变量上的某一个线程，使其从等待状态中退出。
- `notify_all()`：通知等待在条件变量上的所有线程，使它们从等待状态中退出。
- `wait_for(lock, timeout, pred)`：等待一个条件变量，直到被通知或者超时，或者被打断。其中，`timeout` 是一个表示等待超时时间的时间段，`pred` 是一个可调用对象，用于判断是否满足等待的条件。
- `wait_until(lock, timeout_time, pred)`：等待一个条件变量，直到被通知或者超时，或者被打断。其中，`timeout_time` 是一个表示等待超时时间点的时间点类型，`pred` 是一个可调用对象，用于判断是否满足等待的条件。

`std::condition_variable` 可以配合 `std::unique_lock`、`std::lock_guard` 等 RAII 类使用，以自动在加锁和解锁时调用相应的函数，简化了代码的编写。

### std::unique_lock\<std::mutex>

**unique_lock对象的声明周期内，它所管理的锁对象会一直保持上锁状态；而unique_lock的生命周期结束之后，它所管理的锁对象会被解锁**

锁对象 `lock` 会自动锁定 `queue_mutex`，即对该锁进行上锁。同时，当 `lock` 对象销毁时（例如函数返回），它会自动释放 `queue_mutex`，即对该锁进行解锁。

在线程池的代码中，`lock` 对象的作用是锁定任务队列，以便向任务队列中添加任务，而在任务添加完成后，`lock` 对象会被销毁，从而自动解锁任务队列。

>  在线程池运行过程中，对任务分发上锁，用{}区别作用域，lock对象生命周期在{}之中，{}之外lock对象被析构，自动解锁

注意：互斥锁**本身类型**是**std::mutex**！！！但是管理类型是std::unique_lock\<std::mutex>

### std::result_of

用于在编译的时候推导出一个可调用对象（函数,std::funciton或者重载了operator()操作的对象等）的返回值类型.主要用于模板编写中.

### std::future

​		我们想要从线程中返回异步任务结果，一般需要依靠**全局变量**；从安全角度看，有些不妥；为此C++11提供了std::future类模板，**future对象提供访问异步操作结果的机制**，很轻松解决从异步任务中返回结果。

`std::packaged_task` 是一个模板类，可以将任何可调用对象（函数指针、函数对象、Lambda表达式等）封装成一个异步操作（`std::future`），并允许对其进行延迟调用，等待其结果

> 异步线程指的是不需要等待当前线程执行完毕，就可以在另外一个线程中执行的线程。异步线程通常用于处理一些耗时的操作，如网络请求、文件读写、图片处理等。在主线程中，我们可以开启异步线程执行这些操作，以避免主线程被长时间阻塞。通常我们使用线程池来管理异步线程，以避免线程的频繁创建和销毁，提高程序的性能和效率。

### std::packaged_task

​		简单来说std::packaged_task<F>是对可调对象(如函数、lambda表达式等)进行了包装起来，并将这一可调对象的返回结果传递给关联的std::future对象。**实现了将一个可调用对象和一个future对象绑定在一起，从而将异步调用和同步调用结合起来**

1. 创建一个`std::packaged_task`对象，将一个可调用对象封装在其中。
2. 调用`std::packaged_task`对象的`get_future()`方法，获取一个`std::future`对象，该对象用于获取异步操作的结果。
3. 将`std::packaged_task`对象传递给一个线程或者其他的异步执行机制，执行异步操作。
4. 调用`std::future`对象的`get()`方法，等待异步操作完成并获取其结果。

根据传递的参数类型（左值或右值）选择是将其转发为左值引用还是右值引用。当使用 `std::forward` 转发参数时，编译器会根据实际参数类型自动确定该使用左值引用还是右值引用。这种技术可用于将函数的参数按照原始类型转发给其他函数或模板，从而避免不必要的值拷贝和提高代码效率。

设置异步对象的这几句：

```c++
auto task = std::make_shared<std::packaged_task<ReturnType()>> 
                (std::bind(std::forward<F>(f),std::forward<Args>(args)...));
std::future<ReturnType> res = task->get_future();
```

### std::bind

```c++
auto newCallable = bind(callable,arg_list);
```

​		arg_list中的参数可能包含形如\_n的名字，其中n是一个整数，这些参数是“占位符”，表示**newCallable的参数**，它们占据了传递给newCallable的参数的“位置”。数值n表示生成的可调用对象中参数的位置：\_1为newCallable的第一个参数，_2为第二个参数，以此类推。

​		在WEBSERVER项目写线程池时，主要目的就是**把很多类型不同的函数都包装成 void name () 类型，用于模板编程，提高复用性。**

```c++
std::bind(std::forward<F>(f),std::forward<Args>(args)...)
    //第一个参数是可调用对象，第二个参数是参数列表，返回值是一个可以异步执行的函数对象
```

### std::forward

一个模板函数，用于实现**完美转发**（perfect forwarding），即保持被转发参数的值类别（value category）不变，使其可以正确地传递给下一个函数。完美转发的实现方式是使用模板推导来确定被转发参数的类型，并根据该类型将其转发给下一个函数。如果 `t` 是左值引用类型，那么返回的值也是左值引用类型；如果 `t` 是右值引用类型，那么返回的值也是右值引用类型。

### 虚假唤醒

​		假设是在等待消费队列，一个线程A被nodify，但是还没有获得锁时，另一个线程B获得了锁，并消费掉了队列中的数据。B退出或wait后，A获得了锁，而这时条件已不满足。

> 本项目用条件变量控制，可以拿到锁且条件满足才能继续运行，所以没有虚假唤醒

### std::chrono

`std::chrono` 是 C++11 中引入的一个时间库，提供了一种类型安全且不依赖于平台的方式来处理时间和时间间隔

用到的在时间轮上定时器 std::chrono::seconds记录秒数

### std::call_once

在多线程中，有一种场景是某个任务只需要执行一次，可以用C++11中的std::call_once函数配合std::once_flag来实现。多个线程同时调用某个函数，std::call_once可以保证多个线程对该函数只调用一次

传入的函数必须是无参无返回值的，也就是 `void()` 类型的。如果需要传入参数，可以使用 `std::bind` 或 lambda 表达式等方式将参数封装到无参函数中。

### volatile 和 std::atomic

告诉编译器，用它声明的类型变量可以被某些编译器未知的因素更改，所以你要每次都读该变量的内存读取，不要优化。

```c
#include <stdio.h>
 
void main()
{
    int i = 10;//这里如果声明i是volatile类型，那么之后就不会优化
    int a = i;
 
    printf("i = %d", a);
 
    // 下面汇编语句的作用就是改变内存中 i 的值
    // 但是又不让编译器知道
    __asm {
        mov dword ptr [ebp-4], 20h
    }
 
    int b = i;
    printf("i = %d", b);	//这时候i = 10，因为没有volatile
}
```

其实不只是内嵌汇编操纵栈"这种方式属于编译无法识别的变量改变，另外更多的可能是**多线程并发访问共享变量**时，一个线程改变了变量的值，怎样让改变后的值对其它线程 visible。一般说来，volatile用在如下的几个地方：

- 1) 中断服务程序中修改的供其它程序检测的变量需要加 volatile；
- 2) 多任务环境下各任务间共享的标志应该加 volatile；
- 3) 存储器映射的硬件寄存器通常也要加 volatile 说明，因为每次对它的读写都可能由不同意义；

有些变量是用 volatile 关键字声明的。当两个线程都要用到某一个变量且该变量的值会被改变时，应该用 volatile 声明，该关键字的作用是防止优化编译器把变量从内存装入 CPU 寄存器中。如果变量被装入寄存器，那么两个线程有可能一个使用内存中的变量，一个使用寄存器中的变量，这会造成程序的错误执行。volatile 的意思是让编译器每次操作该变量时一定要从内存中真正取出，而不是使用已经存在寄存器中的值

#### std::atomic

在使用std::atomic时，编译器会根据目标平台的架构选择相应的硬件指令来实现原子操作，从而避免了锁和互斥量等同步机制的开销。

下面是std::atomic的用法：

1. 定义std::atomic变量：

```cpp
std::atomic<int> my_atomic_var;
```

1. 对std::atomic变量进行赋值：

```cpp
my_atomic_var = 42;
```

1. 对std::atomic变量进行原子操作：

```cpp
my_atomic_var.fetch_add(1);
my_atomic_var.fetch_sub(1);
```

1. 检查std::atomic变量的值是否满足特定的条件：

```cpp
c++Copy codeif (my_atomic_var.load() > 0) {
    // ...
}
```

1. 使用std::atomic_flag实现原子锁：

```cpp
std::atomic_flag lock = ATOMIC_FLAG_INIT;

void foo() {
    while (lock.test_and_set(std::memory_order_acquire)) { // test_and_set 原子地设置标志为 true 并获得其先前值
        // 等待
    }
    // 临界区代码
    lock.clear(std::memory_order_release);
}
```

需要注意的是，在使用std::atomic进行原子操作时，需要指定内存序（memory order）。内存序是指控制内存操作在不同线程之间的可见性和排序关系的机制。C++标准库提供了几种不同的内存序，用于控制原子操作的行为。具体来说，std::memory_order_relaxed表示松散的内存序，不保证多线程之间的同步；std::memory_order_acquire表示获取内存序，用于确保读取操作的可见性；std::memory_order_release表示释放内存序，用于确保写入操作的可见性；std::memory_order_seq_cst表示顺序一致的内存序，用于确保原子操作的顺序性。

总之，std::atomic是C++11中用于实现原子操作的模板类和操作函数，它提供了一种轻量级的同步机制，可以避免并发问题。使用std::atomic时，需要注意指定适当的内存序，以确保原子操作的正确性。

### Regex库

`std::regex reg(R"(^(POST|GET|HEAD)\s(\S*)\s(HTTP)\/\d\.\d)")`

* std::regex：正则表达式库，用于处理字符串的匹配、替换等操作

可以使用类似 `std::regex re("pattern")` 的方式创建一个正则表达式对象，其中 `"pattern"` 是一个正则表达式字符串。

注意格式：格式1：R“( )” 这种形式里面'\'只用写一次 \* 格式2：如果直接写“ ”，转义斜杠要用两次。

* std::regex_match：C++11 标准库 `<regex>` 中的函数，用于判断一个字符串是否完全匹配某个正则表达式，

  匹配成功返回 `true`，否则返回 `false`

* std::smatch   表示正则表达式的一个匹配结果。

在正则表达式中设置括号，可以分组，这样得到的smatch也是分组的，可以用 res[1] res[2]等取得匹配的分组结果

#### R()

: 在正则表达式中，R 表示该正则表达式使用的是原始字符串，即不会对字符串中的特殊字符进行转义处理。在 C++ 语言中，使用原始字符串可以通过在字符串前加上 R"()" 来表示。例如，"hello\nworld" 中的 "\n" 会被解释成换行符，而 R"(hello\nworld)" 中的 "\n" 则表示字符串中的字面字符 "\n"。

这边HTTP报文的正则匹配，应该不需要转义，就是匹配字符

### sizeof 关键字

在 C++ 中，sizeof 是一个关键字，用于计算一个对象或类型所占的字节数。在使用 sizeof 关键字时，可以加括号，也可以不加括号。在实际使用时，不加括号更为常见，因为 sizeof 运算符是一个关键字而不是函数调用，加括号可能会增加代码的混淆度

### 类型转换

1. 静态转换（static_cast）：可以进行基本数据类型之间的转换、void指针和目标类型指针之间的转换、父子类之间指针或引用的转换，但在进行父子类之间的转换时需要注意，只有当子类指针或引用真正指向一个父类对象时才是安全的，否则可能会导致未定义行为。

2. 动态转换（dynamic_cast）：只能在父子类指针或引用之间进行转换，因为它在运行时检查转换的安全性，所以在进行转换时需要保证父类指针或引用指向的是一个子类对象，否则转换会失败并返回一个空指针或引用。此外，只有在使用虚函数或继承关系时才能使用dynamic_cast。

3. const_cast：用于去除const和volatile属性，可以将const或volatile类型转换为非const或非volatile类型。使用const_cast需要非常小心，因为它可能会导致未定义行为。

4. 重新解释转换（reinterpret_cast）：可以在不同的指针、引用、整数类型之间进行转换，但并不会改变底层数据的类型，而是将它们的二进制位重新解释为其他类型的值。使用reinterpret_cast需要非常小心，因为它可能会导致未定义行为。
   一般用于指针转换，能够在不改变所指的数据类型的情况下，重新解释数据的方式。它可以把一个指针类型转换为另一个指针类型，也可以将一个整型转换为指针类型，反之亦然。

### emplace_back和push_back的区别

​		push_back：在引入右值引用，转移构造函数，转移复制运算符之前，通常使用push_back()向容器中加入一个右值元素（临时对象）的时候，首先会调用构造函数**构造这个临时对象**，然后需要调用拷贝构造函数将这个临时对象放入容器中。原来的**临时变量释放**。这样造成的问题是临时变量申请的**资源就浪费**。push_back括号里面是对象，以该对象拷贝构造。

​		emplace_back：在容器尾部添加一个元素，这个元素**原地构造**，不需要触发拷贝构造和转移构造。而且调用形式更加简洁，直接根据参数初始化临时对象的成员。emplace_back括号里面是参数，以该参数原地构造。

## Linux一些功能函数

### getopt() 

获取命令行参数的函数，貌似每次获取一个选项的参数

选项 : 
	gcc helloworld.c -o helloworld.out; 这条指令中的-o就是命令行的选项，而后面的helloworld.out就是-o选项所携带的参数

选项字符串 : 
	"`a:b:cd::e`"，这就是一个选项字符串。对应到命令行就是-a ,-b ,-c ,-d, -e 
冒号表示参数，一个冒号就表示这个选项后面必须带有参数（没有带参数会报错哦），但是这个参数可以和选项连在一起写，也可以用空格隔开，比如-a123 和-a  123（中间有空格） 都表示123是-a的参数；两个冒号的就表示这个选项的参数是可选的，即可以有参数，也可以没有参数，但要注意有参数时，参数与选项之间不能有空格（有空格会报错的哦），这一点和一个冒号时是有区别的。

搭配参数：

extern char* optarg;  	保存选项的参数的

extern int optind; 		  记录下一个检索位置

extern int opterr;			是否将错误信息输出到stderr，为0时表示不输出

extern int optopt;			不在选项字符串optstring中的选项

### 信号处理方式

Linux多线程中使用信号机制和进程中使用不同。

进程中使用是异步的：先注册信号处理函数，当信号异步发生时，调用处理函数来处理信号。而且有限制，比如有一些函数不能在信号处理函数中调用；再比如一些函数read、recv等调用时会被异步的信号给中断(interrupt)，因此我们必须对在这些函数在调用时因为信号而中断的情况进行处理（判断函数返回时 enno 是否等于 EINTR）。

多线程中，把对信号的异步处理，转换成同步处理，也就是说**用一个线程专门的来“同步等待”信号的到来，而其它的线程可以完全不被该信号中断/打断(interrupt)**这样就在相当程度上简化了在多线程环境中对信号的处理。而且可以保证其它的线程不受信号的影响。这样我们对信号就可以完全预测，因为它不再是异步的，而是同步的（我们完全知道信号会在哪个线程中的哪个执行点到来而被处理！）。而同步的编程模式总是比异步的编程模式简单。其实多线程相比于多进程的其中一个优点就是：多线程可以将进程中异步的东西转换成同步的来处理。

​		sigwait是**同步**的等待信号的到来，而不是像进程中那样是异步的等待信号的到来。sigwait函数使用一个信号集作为他的参数，并且在集合中的任一个信号发生时返回该信号值，**解除阻塞**，然后可以针对该信号进行一些相应的处理。

​		调用sigwait同步等待的信号必须在调用线程中被屏蔽，并且通常应该在所有的线程中被屏蔽（这样可以保证信号绝不会被送到除了调用sigwait的任何其它线程），这是通过利用信号掩码的继承关系来达到的。

​		本项目中，如果事先不屏蔽这些信号，会导致**subReactor的epoll_wait()函数被信号中断**，`SIGALRM`信号被传递给正在调用`epoll_wait()`的线程或进程，**则`epoll_wait()`将返回-1**，并设置errno为`EINTR`。

#### 设置信号屏蔽

SIGPIPE、SIGALRM 和 SIGTERM 都是 Unix/Linux 系统中的信号（signal），它们分别代表不同的含义和用途。

1. SIGPIPE：

SIGPIPE 是指在进程向一个已经接收到 RST（复位） 或者 FIN（结束） 段的 socket 发送数据时，会触发 SIGPIPE 信号，通常情况下这个信号会使程序退出。这个信号主要的作用是防止程序继续向已经关闭的 socket 写数据，从而避免因此产生不必要的错误。

        ① 初始时，C、S连接建立，若某一时刻，C端进程关机或者被KILL而终止（终止的C端进程将会关闭打开的文件描述符，即向S端发送FIN段），S端收到FIN后，响应ACK
        ② 假设此时，S端仍然向C端发送数据：当第一次写数据后，S端将会收到RST分节； 当收到RST分节后，第二次写数据后，S端将收到SIGPIPE信号（S端进程被终止）     

2. SIGALRM：

SIGALRM 是指在一个定时器超时时发送给进程的信号，通常可以用来实现定时任务。当定时器超时时，操作系统会向进程发送 SIGALRM 信号，进程可以通过注册一个信号处理函数来处理这个信号。在信号处理函数中，通常会执行需要定时的任务，或者更新定时器的超时时间等。

3. SIGTERM：

SIGTERM 是指在进程被请求终止时发送给进程的信号，通常情况下这个信号会使进程正常退出。当一个进程接收到 SIGTERM 信号时，它可以进行一些清理操作，然后退出。这个信号可以通过 `kill` 命令或者其他方式向进程发送。

总的来说，这三个信号分别用于**防止程序向已经关闭的 socket 写数据、实现定时任务和请求进程终止**。在程序开发中，需要根据实际情况选择合适的信号进行处理，以保证程序的正确性和稳定性。

4.SIGINT

 通常是由用户在终端中使用 Ctrl+C 触发，用于请求程序中断执行。当程序收到SIGINT信号时，操作系统会终止程序执行并返回到命令行终端。SIGINT是一种可被捕获的信号，可以使用signal函数或者sigaction函数注册一个信号处理函数来处理SIGINT信号。

#### pthread_sigmask

在Linux当中，一个进程可以有多个（上千）线程，`signal`是一个比`pthread`早出现的概念，在`signal`被发明的年代，多进程是主流。推荐的做法是：如果现在主线程启动的时候设置了pthread_sigmask信号屏蔽，那之后创建的子线程会继承信号掩码，这样所有的线程都不会响应被屏蔽的信号，以确保线程的信号处理行为一致。然后单独拉出一个线程，解除阻塞信号，处理事件。

注意：SIGKILL 信号和 SIGSTOP 信号不能被阻塞。(设置信号集时会被内核剔除)（避免出现神仙进程）

参数和`sigprocmask`函数的参数类似，只是它们作用的对象是线程，而不是进程。

`在Linux当中，进程的所有线程共享信号`。线程可以通过设置信号掩码来屏蔽掉某些信号。然而，在这里子线程是通过线程池创建的，不太好添加信号掩码，况且在每个线程中单独 设置信号掩码也很容易导致逻辑错误。因此，最好的方法是专门定义一个线程去处理所有 信号。首先需要在所有子线程创建之前，在主线程中设置好信号掩码。随后创建的子线程会自动继承这个信号掩码。这样做了之后，所有线程都不会响应被屏蔽的信号。因此，需 要再单独创建一个线程，并通过调用sigwait函数来等待信号并处理,专门处理信号。



### epoll_create1()

`epoll_create1` 是 Linux 2.6.27 以后提供的系统调用，它用来创建一个 epoll 实例。与旧的 `epoll_create` 不同的是，`epoll_create1` 可以指定 `flags` 参数来设置 epoll 实例的属性。

其中，`EPOLL_CLOEXEC` 表示在执行 `exec` 系统调用时，关闭这个 epoll 实例。如果不指定 `EPOLL_CLOEXEC`，则在执行 `exec` 系统调用时，epoll 实例会被继承到子进程中，这可能会导致安全问题。因此，一般情况下我们都应该使用 `epoll_create1(EPOLL_CLOEXEC)`。



### 读写文件和网络数据

send和recv函数与socket相关联，适用于在网络中传输数据，而read和write函数适用于文件IO，可以用于读写文件或标准输入/输出



### 非阻塞的socket EAGAIN

**发送时**send() `tcp数据包太大了怎么办？`

　当客户通过Socket提供的send函数发送大的数据包时，就可能返回一个EAGAIN的错误。该错误产生的原因是由于`send 函数中的size变量大小超过了tcp_sendspace的值`。tcp_sendspace定义了应`用在调用send之前能够在kernel中缓存的数据量`。当应用程序在socket中设置了O_NDELAY或者O_NONBLOCK属性后，如果发送缓存被占满，send就会返回EAGAIN的错误。 
　　为了消除该错误，有三种方法可以选择： 
1.调大tcp_sendspace，使之大于send中的size参数 
　　---no -p -o tcp_sendspace=65536 　　

2.在调用send前，在setsockopt函数中为SNDBUF设置更大的值 

3.使用write替代send，因为write没有设置O_NDELAY或者O_NONBLOCK

计算出要发多少个字节，然后维护一个变量表示剩下多少字节需要发送，然后while循环里面套send函数（ET模式），send函数的返回值是发送成功的字节数，返回成功后剩下的字节数就减少，直到剩余字节为0。

http2.0还支持数据的压缩。

> 本项目在发送socket数据包的时候，接收到errno == EAGAIN || errno == EWOULDBLOCK时，视为send在内核中的缓冲区已满，直接截断，等待下次重新发送。 也就是分包发送

**接收时**recv()

接收数据时常遇到Resource temporarily unavailable的提示，errno代码为11(EAGAIN)。这表明你在非阻塞模式下调用了阻塞操作，在该操作没有完成就返回这个错误，这个错误不会破坏socket的同步，不用管它，下次循环接着recv就可以。对非阻塞socket而言，EAGAIN不是一种错误。在VxWorks和Windows上，EAGAIN的名字叫做EWOULDBLOCK。其实这算不上错误，只是一种异常而已。

另外，如果出现EINTR即errno为4，错误描述Interrupted system call，操作也应该继续。

最后，如果recv的返回值为0，那表明对方已将连接断开，我们的接收操作也应该结束。

> 本项目在recv()接收到errno == EAGAIN || errno == EWOULDBLOCK时，视为已经读完，直接返回read_sum

### favicon.ico

`favicon.ico` 是一个网站图标文件，通常放置在网站根目录下，用于显示在浏览器标签页上、书签栏、收藏夹等位置。在访问网站时，浏览器会默认请求该文件，以便显示网站图标。

该文件通常使用 ICO 格式（Windows 图标格式）或 PNG 格式存储，大小为 16x16 像素或 32x32 像素。如果网站没有提供该文件，则浏览器会使用默认图标代替。

网站开发者可以使用专门的工具来创建自己的 `favicon.ico` 文件，如 Photoshop、GIMP、ICOFX 等，或者使用在线图标生成器，如 Favicon Generator、RealFaviconGenerator 等。通常，为了兼容性，网站开发者会同时提供 16x16 像素和 32x32 像素两个版本的 `favicon.ico` 文件。

```
GET /favicon.ico 
Host: 192.168.31.128:9899
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0
Accept: image/avif,image/webp,*/*
Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2
Accept-Encoding: gzip, deflate
Connection: keep-alive
Referer: http://192.168.31.128:9899/hello.html
```

### memset

​	 //bzero(&ev,sizeof ev); //bzero是linux特有的，推荐使用memset

### 前后台进程区分

在Linux中，进程分为前台进程和后台进程。

前台进程是指用户在命令行中启动的进程，通常会占用控制台，用户可以看到其输出并通过输入控制它的行为。前台进程在运行时会阻塞掉其他命令的执行，直到该进程运行结束或者被手动终止。后台进程是指在后台运行的进程，它们通常没有控制台，用户无法直接查看其输出，但是它们仍然在系统中运行。后台进程的执行不会阻塞掉其他命令的执行。在命令行中，可以通过在命令后加上“&”符号来将进程放入后台运行。例如，运行命令“./myprogram &”将使myprogram进程在后台运行。

还有一种特殊情况，即通过Ctrl+Z将前台进程挂起并放入后台，此时该进程不再是前台进程，但也不完全是后台进程。这时可以使用“fg”命令将其恢复到前台运行。

进程推出：和前台进程一样，后台进程也可以被杀死。可以使用kill命令或者其他类似的系统调用来杀死后台进程。当然，如果一个进程没有设置信号处理程序来处理某些信号，那么它可能会被杀死并退出。但是，如果一个进程在后台运行并且没有打开标准输入，标准输出和标准错误输出，那么它可能会被称为“守护进程”，因为它在后台运行而不会对终端用户产生任何输出或交互。这种进程通常是为了执行系统任务或长时间运行的任务而设计的，例如作为服务器进程。

## 项目结构设计

### 事件驱动环

事件驱动环（event loop）是一种常见的编程模型，主要应用于异步编程或非阻塞IO操作。事件驱动环中，程序会不断地从事件队列中取出事件并处理，如果没有新事件到来，则程序会一直等待直到有新事件到来。每个事件通常都与一个回调函数关联，当事件到来时会调用对应的回调函数进行处理

### 时间轮

时间轮其实可以用自己的接口驱动，这个项目目前还是用signal驱动，所以本质还是存储list<事件>的一个容器，只不过相比直接存链表，设计上有点考虑（不过链表和时间轮的槽内元素一样都是事件链表），考虑用链表会很长，而考虑用环状结构，桶会很大，但是总体长度固定。

//TODO 考虑时间轮+信号驱动的合理性，或者修改成时间轮的高效用法



### 项目流程

信号处理进程先启动

线程池初始化创建，主reactor初始化创建，主server初始化创建；

主reactorStart：设置监听chanel（包括event 连接func），设置连接的回调函数和错误的回调函数。创建subReactor的EventLoop（其中配套TimeWheel），把Eventloop作为task放到线程池中抢占，把所有subReactor时间轮的tick读端拿到。

利用信号，推动所有的时间轮，同时tick

mainReactor开始循环Listen_and_Call，这时他的chanelpool只有监听的listen_chanel\_；这时候开始接受socket连接请求，如果mainReactor收到了listen_chanel\_的EPLLOIN，也就是socket连接请求，调用main.CONNisComing()接收（先前设置了mainReactor的read_handle是Server的CONNisComing()），创建对应这个EPOLLIN事件（这个客户端）的chanel， 选取负载最小的子线程，创建HttpData对象，然后subReactor添加这个chanel（包括对应的event、HttpData）,添加头部解析的超时定时器（func是http408报文）。 // 目前为止chanel注册完毕

subReactor：这里叉开看子线程，线程池在争取的task是一个Eventloop的运行函数（不断的Listen_and_Call() ）,也就是线程池初始化一对一的获取Eventloop后就不在分配。

继续看subReactor注册了这个chanel之后，就能够监听这个chanel发过来的数据（这里注意区分listenfd监听的EPOLLIN是请求连接的EPOLLIN，之后subReactor监听到的EPOLLIN才是数据传输的）。线程原本就在Eventloop上不断的Listen_and_Call() ，当监听到已经注册的chanel的EPOLLIN等信号就能直接调用对应处理函数读写到内存中（对应函数就前面初始化HttpData的时候已经设置为相关recv和send类型的操作） //目前为止新客户端已经和一个subReactor连接上了

比如说 这时候先发过来，触发EPOLLIN，那么对应的subReactor就去recv读取http报文（ET模式，一下全读过来），然后调用状态机解析报文（ET模式，用while循环解析很多个http，也就是所说的管线化，当前项目好像还没实现 ），依次解析命令行、首部、内容，然后生成响应报文到内存中（需要回复文件的话用内存映射），接着直接调用Send回写响应报文。

### 类说明

- Channel: 底层事件类。可视作一个文件描述符、其需要监听的事件以及这些事件相应回调函数的封装。
- HttpData: http数据处理类。包括了连接socket所有事件的回调函数。
- EventLoop: 即reactor，负责向epoll_wait中添加、修改、删除事件以及调用epoll_wait并根据活跃事件调用其相应的回调函数。

### 连接的建立与分发

 这部分的功能由主线程，也即MainReactor完成。在本项目中，reactor的角色由类Eventloop扮演。在主线程中的Eventloop对象为MainReactor，子线程中的Eventloop对象即为SubReactor，并且采用了one loop per thread的模式。在MainReactor中只需要监听一个listen socket的可读（EPOLLIN）以及异常事件（EPOLLERR）。在可读事件的回调函数中，包括了ET模式下的accept函数调用以及连接socket的分发。其中连接socket会被分发给当前监听事件最少的SubReactor。连接socket的EPOLLIN、EPOLLOUT、EPOLLERR以及EPOLLRDHCP均由SubReactor负责监听和相应回调函数的调用。

### 请求报文的解析与响应报文的发送

 建立了tcp连接后，服务器会针对http请求报文的请求行和首部行数据的完整到达设置一个超时时间，若在该时间内，服务端没有接受到完整的http请求报文，那么会向客户端发送408 request time-out并关闭连接。同时，对于post报文中的实体数据，要求相邻两实体数据包到达的时间间隔不超过一个特定的时间，否则发送408并关闭连接。

### 如何关闭连接

 服务端关闭连接分为主动关闭和被动关闭两种情况。在连接socket发生EPOLLERR、从连接socket读取数据时出错、向连接socket写数据出错以及处理完短连接请求时，服务端会主动关闭连接。被动关闭连接发生在客户端首先先主动关闭了连接，此时服务端也对等关闭连接。



### 调用其他类

通常情况下，头文件会被多个源文件引用，而每个源文件都需要对其进行编译，这可能会导致编译时间增长。

为了避免重复编译头文件的问题，可以使用头文件的前置声明来代替直接包含头文件，从而加快编译速度。前置声明告诉编译器某个类、函数或变量的名称和类型，而不是提供其完整定义，这样编译器就能够知道如何处理这个类或函数，而无需真正的定义。在源文件中，只有当实际需要使用这个类或函数时，才需要包含相应的头文件来提供其定义。

使用头文件的前置声明可以减少编译时间，但也存在一些限制。例如，前置声明不能用于定义类的成员变量或函数，因为编译器需要知道这些变量或函数的具体大小和类型信息。因此，在这些情况下，必须包含相应的头文件。此外，前置声明还可能导致代码的可读性和可维护性降低，因为其他人可能不知道实际定义的内容。

总之，头文件的前置声明可以提高编译速度，但需要注意它的使用限制和代码可读性的影响。

> 在c++写类的时候，如果要用到其他头文件里的其他类作为本类的成员，那我先在我这个头文件前置声明这个类，然后在具体实现的cpp文件中如果用到了其他类的函数，再去导入其他类的头文件。为什么要这一套，可能是防止重复包含，或者为了加速，以减少头文件的依赖关系和编译时间

### 单例模式

保证一个类仅有一个实例，并提供一个访问它的全局访问点，该实例被所有程序模块共享。

定义一个单例类：

1. 私有化它的构造函数，以防止外界创建单例类的对象；
2. 使用类的私有静态指针变量指向类的唯一实例；
3. 使用一个公有的静态方法获取该实例。

#### 懒汉版（Lazy Singleton）：

单例实例在第一次被使用时才进行初始化，这叫做延迟初始化。

```cpp
//因为c++11标准要求局部静态变量初始化具有线程安全性
// version 1.3
class Singleton
{
private:
	Singleton();
	Singleton(const Singleton& other) = delete;
	Singleton& operator=(const Singleton& other) = delete;
public:
	static Singleton& getInstance() {
        static Singleton value;  //静态局部变量
		return instance;
	}
};

// initialize defaultly
Singleton& s1 = Singleton::getInstance();
```

C++11之前

```cpp
class Singleton
{
private:
	Singleton();
	Singleton(const Singleton& other) = delete;
	Singleton& operator=(const Singleton& other) = delete;
    
    Singleton* pInstance_{nullptr};
    std::mutex lock_;
public:
		
	Singleton* getInstance() {
    	if (pInstance == nullptr) {
        	std::unique_lock<std::mutex> locker(lock_);
        	if (pInstance_ == nullptr) {
            	pInstance_ = new Singleton;
        }
    }
    return pInstance_;
};
```



#### 饿汉版（Eager Singleton）：

指单例实例在程序运行时被立即执行初始化。

```cpp
//因为C++11要求编译器保证内部静态变量的线程安全
// version 1.3
class Singleton
{
private:
	static Singleton instance;
private:
	Singleton();
	~Singleton();
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
public:
	static Singleton& getInstance() {
		return instance;
	}
}

// initialize defaultly
Singleton Singleton::instance;
```

### Nagle算法

计网学过，是TCP提高传输效率的一种，还有指明分组发送的时机

> Nagle算法是一种用于减少网络拥塞的算法。它的主要思想是减少小数据包的数量，从而减少网络拥塞的可能性。
>
> 具体来说，Nagle算法会缓存应用程序发送的小数据包，并在缓存区`达到一定大小或一定时间间隔之后再将它们一起发送出去`。这样做的好处是可以减少网络传输的次数，降低网络拥塞的概率。
>
> Nagle算法的原理是基于TCP协议的。在TCP协议中，每次发送数据都需要建立一个连接，这会消耗网络资源并增加网络拥塞的可能性。Nagle算法通过减少小数据包的数量，可以降低连接建立的次数，从而减少网络拥塞的概率。
>
> 然而，Nagle算法可能会引入一定的延迟。由于数据包需要在缓存区达到一定大小或时间间隔后才会发送出去，因此会增加数据传输的延迟。因此，在某些情况下，禁用Nagle算法可能会更好，特别是对于需要实时传输数据的应用程序。

TCP/IP协议中针对TCP默认开启了Nagle算法。Nagle算法通过减少需要传输的数据包，来优化网络。在内核实现中，数据包的发送和接受会先做缓存，分别对应于写缓存和读缓存。 启动**TCP_NODELAY**，就意味着禁用了Nagle算法，允许小包的发送。对于延时敏感型，同时数据传输量比较小的应用，开启TCP_NODELAY选项无疑是一个正确的选择。 比如，对于SSH会话，用户在远程敲击键盘发出指令的速度相对于网络带宽能力来说，绝对不是在一个量级上的，所以数据传输非常少； 而又要求用户的输入能够及时获得返回，有较低的延时。如果开启了Nagle算法，就很可能出现频繁的延时，导致用户体验极差。

### HTTP报文解析

<img src="D:\MyTxt\typoraPhoto\image-20230316172604702.png" alt="image-20230316172604702" style="zoom:50%;" />

a、请求行

　　　　**请求行由请求方法字段、URL字段和HTTP协议版本字段，组成，它们用空格分隔，例如：GET /index.html  HTTP/1.1**
HTTP协议的请求方法有GET、POST、HEAD、PUT、DELETE、OPTIONS、TRACE、CONNECT。这里介绍最常用的GET和POST方法；

* GET：当client要从server中读取文档时，使用GET方法。GET方法要求服务器将URL定位的资源放在响应报文的**数据部分**，回送给client。使用GET方法时，请求参数和对应的值附加在URL后面，利用一个问号（"?"）代表URL的结尾与请求参数的开始，传递参数长度受限制，例如：  /index.jsp?id=100&op=bind

* POST:当client给服务器提供信息较多时， 使用POST方法。POST方法将请求参数封装在HTTP请求数据中，以key/value的形式出现，可以传递大量数据，可用来传递文件

b、消息头部

　　　　请求头部由key/value键值对组成，每行一对，key和value用冒号":"分隔，请求头部通知服务器有关于client端的请求信息，典型的请求头：

- User-Agent：产生请求的浏览器类型
- Accept：client端可识别的内容类型列表
- Host：请求的主机名，允许多个域名同处一个ip地址，即虚拟主机　　　　

c、空行

　　　　最后一个请求头之后是一个空行，发送回车符和换行符，通知服务器请求头结束。

　　　　对于一个完整的http请求来说空行是必须的，否则服务器会任务本次请求的数据尚未完全发送到server，处于等待状态

d、请求正文

　　　　请求数据不在GET方法中使用，而是在POST中使用。POST方法适用于需要client填写表单的场合，与请求数据相关的最常用的请求头是Content-Type 和Content-Length

### 获取实时时间

```cpp
std::string Gettime()
{
    time_t lt = time(NULL);
    struct tm* ptime = localtime(&lt);

    char timebuf[100];
    strftime(timebuf,100,"%a,%d %b %Y %H:%M:%S",ptime);
    return std::string(timebuf);
}
```

这段代码是一个函数，用于获取当前系统时间并将其格式化为字符串。

首先，调用 `time()` 函数获取当前时间的时间戳（秒数）。然后，调用 `localtime()` 函数将时间戳转换为本地时间，并保存在 `struct tm` 结构体中。结构体中包含了当前年、月、日、小时、分钟、秒等信息。

接下来，调用 `strftime()` 函数将 `struct tm` 结构体中的时间信息格式化成指定的字符串格式，并将结果保存在 `timebuf` 字符数组中。`strftime()` 函数的第二个参数 "%a,%d %b %Y %H:%M:%S" 是指定的时间格式，具体含义如下：

- %a：星期几的缩写（如 Mon、Tue 等）
- %d：月份中的第几天（01 到 31）
- %b：月份的缩写（如 Jan、Feb 等）
- %Y：年份（如 2023）
- %H：小时（00 到 23）
- %M：分钟（00 到 59）
- %S：秒（00 到 59）

最后，将字符数组转换为 `std::string` 对象并返回。这样就可以获取当前系统时间的字符串表示了。

### ET模式

在使用 epoll ET 模式时，可能会一次性读取多个 HTTP 请求。这是因为 ET 模式是边缘触发模式，在可读事件触发时只会通知一次，不会重复通知，直到下一次有新的数据到来才会再次触发可读事件。

当有多个 HTTP 请求到达时，内核会将这些请求合并成一个可读事件，然后通过 epoll_wait 函数返回。如果用户程序没有一次性将所有请求数据全部读取完毕，那么在下一次 epoll_wait 返回时还会继续读取之前未读取完毕的数据。

因此，在使用 ET 模式时，需要保证能够一次性



在使用 epoll ET 模式时，可以一次性写入多个 HTTP 响应。这是因为在 ET 模式下，当写入操作可行时，内核只通知一次可写事件，不会重复通知，直到下一次写入数据时才会再次触发可写事件。

因此，如果要一次性写入多个 HTTP 响应，只需要将多个响应数据合并成一个大的数据块，然后使用 write 函数一次性将这个数据块写入到套接字中即可。此时，内核只会触发一次可写事件，将整个数据块一次性写入到套接字中。

需要注意的是，一次性写入大量数据可能会导致网络拥塞或内存占用过高，因此需要根据具体应用场景和硬件条件，合理选择写入数据的大小。

### 事件处理模式

​		开发平台以及部署平台都是Linux，因而选择Reactor模式。这是因为Linux下的异步I/O是不完善的，aio系列函数是在用户空间出来的，不是由操作系统支持的，故部署在Linux上的高性能服务器大多选择Reactor模式。Reactor模式有三种常用方案：

- 单Reactor单进程/线程
- 单Reactor多线程
- 多Reactor多线程

​		单Reactor单进程/线程方案实现起来非常简单，不涉及任何进程或线程间的通信，但缺点也是十分明显的。一旦业务逻辑处理速度过慢，将导致严重的延迟响应。因此，单Reactor单进程/线程模式适合业务处理较为快速的场景。单Reactor多线程解决了上一个方案的问题，但由于只有一个Reactor负责所有事件的监听和响应，无法有效应对瞬时高并发的情形；多Reactor多线程不存在以上两个方案的问题，且线程间的通信也很简单，MainReactor只需将连接socket传递给SubReactor即可。数据的收发以及业务逻辑均在SubReactor中完成，任务划分十分明确。因此，本项目选择采用多Reactor多线程方案。



### core 调试

ulimit -c 命令检查系统是否支持生成 core 文件。如果 ulimit -c 命令输出 0，则表示系统未启用核心转储。

ulimit -c unlimited 来启用 core 文件的生成

运行程序，在程序崩溃时，会生成 core 文件。

使用以下命令来查看 core 文件  gdb /path/to/your/program /path/to/core/file

### 压力测试

```
 ./webbench -c 1000  -t  30   http://192.168.31.128:18888/index.html
 权限不够重新make一下 参数：   -c 表示客户端数    -t 表示时间
```

### GDB调试

```
set args -p 9899 -s 10 -l ./log/log.txt 
```

查找对应端口号进程     

```
sudo lsof -i :端口号    sudo kill -9 进程PID
```

* gdb基本操作

  ~~~shell
  #应该要保证可执行文件和源文件都在
  gdb 目标程序
  ~~~

  

* 断点操作

  **退出gdb后断点全部失效**

  

* 调试命令 

  

多线程gbd调试 有问题，还要看:

1. 使用 `bt` 命令打印当前调用栈，以查看程序在哪个函数中崩溃了。
2. 使用 `info registers` 命令查看当前程序执行时各寄存器的值，以查找程序中的潜在错误。
3. 使用 `info threads` 命令查看程序的各个线程状态，以确定哪个线程导致了段错误。
4. 使用 `x` 命令查看内存地址的内容，以确定程序在哪里访问了非法内存地址。
5. 使用 `watch` 命令在某个变量或内存地址发生更改时中断程序，以查找潜在的数据错误。
6. 如果发现内存泄漏等问题，可以使用 `valgrind` 工具来检查程序的内存使用情况

### bug纪录

1. bug httpdata的timer没注册（slot_[]全是空的）  但是主Reactor分发完成了（输出了任务+1），TimeWheel_Insert_Timer（）没进来，AddChanel failed subReactors_0 fd connfd38 ，
   bug已经修复 chanel变量问题

2. 定时器没用 可能是接受httpdata还有问题 
   void HttpData::Callback_IN()没进入，没有读，subreactor注册的EPOLIN事件有问题 不是这个原因 
   Reactor的events在哪里注册的 ？答：在注册chanel的时候，因为chanel自带events.
   bug 已经修复 状态机解析头问题

3. 调用modchanel出错 错误位置是状态机读完回写的时候，回写完重置Events的时候
   bug已经修复 重置chanel函数有问题

4. 长时间解析不出来 Analyse_Get_or_Head()函数解析有问题，但是返回的是success；没有http连接请求就进入状态机解析；
   bug已经修复，请求行解析有问题

5. keepalive到时间，释放错误；响应回写完之后长连接保持keepalive的定时器事件应该设置更改，不能还是408吧
   bug 先改为 keepalive的定时器事件为http的dis函数；
   目前还有double free的问题 
   如果点击刷新，应该是客户端主动发起一个断开的信号，接收到EPOLLIN，EPOLLOUT，EPOLLRDHUP事件，逻辑实现上是首先判断EPOLLRDHUP事件，来进行断开dis，但是Y有些系统未必触发EPOLLRDHUP事件，则在之后关心EPOLLIN事件，然后再recv中如果dis_conn == true 也进行断开第三。然后客户端重新发来请求，重新连接，EPOLLIN。

   先检查dis函数接触连接后删的干不干净后 看delchanel，到底在哪里删除一个客户端对应的所有对象需要考虑，先试试在delchanel中全部删掉，但是delchanel本身也是httpdata对象调用的。

   还是有bug，double free的bug，应该是keepalive的闹钟超时函数的问题。在响应完之后，reset函数设置闹钟超时函数是dis

   Server创建时基于mainReactor的，在mainReactor构造函数中创建了epoll实例，在Servernew中chanel，该chanel只是网络连接中接收新的客户端的socket事件，且只有一个。

   bug修复， 定时器重复删除，且槽链表头没有更新

6. 错误响应回写有问题，404打印，没有及时回写
   bug已经修复，状态码信息不能加\n

7. webbench压力测试  
   ./webbench -c 1000  -t  5   http://192.168.31.128:9899/index.html

   1. http超时解析的时候，删除fd出现很奇怪的fd，可能是把chanel对象删除了； 超时的tick函数找不到

   2. get an unregistered chanel，是不是没删干净，keep-alive超时之后需要删除EventLoop中chanelpool_对应的chanel，但是普通的一次读写完毕，重置的时候只需要删除httpdata相关的。fd56来连，fd56又来连，处理完之后，fd56断开，fd56又来连，未注册的chanel。这时候该事件对应的event却还在epoll中
      bug修改处：1. delchanel没有删除该chanel对应的EventLoop的events 2.先注册到chanel池，在添加到Eventloop中的events，因为已注册events就要开始读了（修复一些注册在epoll拿到之后的bug）；
      还是有bug ：fd56来连，fd56又来连，处理完之后，fd56断开，fd56又来连，未注册的chanel；这是单个线程做的事情，应该不存在线程同步的问题，epoll读取一次，去处理fd56，完了之后fd56断开，下一次fd56又来，重新注册吗？不是，主线程是通过什么知道是新的客户端的？设置监听到connfd的就分配，不管新不新

   3. 段错误 double free ： 定时器超时回调的时候找不到某个值；
      检查了定时器槽链表的是删除，定时器回调函数执行后，删除定时器对应的timer chanel httpdata，然后链表指向下一个节点，直到该槽清空，应该是没问题。
      修改write_buf\_初始化，没用 有默认初始化
      定时器没删干净 ；删除定时器是在Callback_Dis()函数
      tick中timer的超时的回调函数是Addchanel的时候注册的http的超时回调函数。这个问题是每次跑完才出现，我设置跑的长一点试试：结果还是tick回调直接断了，所以和跑完没关系，就是tick的回调函数有问题。不管是+=还是=都有问题，也就是string本身的问题；
      往前看函数调用，在TimeUpCall()的时候，fd已经是负数了，isConnect\_也是乱的,write_buf_乱七八糟。所以问题应该是http对应的有一个chanel和httpdata已经析构掉了 timer没删干净吗？

      如果是超时，不管状态机在解析什么东西（不对，一次一个event），直接先写入错误报文，然后Callback_OUT(),然后重置evnets，那万一这时候解析了一部分了，应该不可能，因为一个线程同时只处理一个event，处理完之后再去处理下一个，tick的EPOLLIN也是一样。

      分析timer的生命周期，

   4. 404的问题

8. 自定义的Getlog()输出正常的

9. bug eventloop接受到未注册的chanel 原因：闹钟chanel未注册

## 一些疑问

### 为什么监听socket一定要是非阻塞的

​	（前提是采用了IO复用函数）

​		当一个连接到来的时候，监听套接字可读，此时，我们稍微等一段时间之后再调用accept()。就在这段时间内，客户端**设置linger选项(l_onoff = 1, l_linger = 0)**，然后**调用了close()**，那么客户端将不经过四次挥手过程，通过发送**RST报文**断开连接。服务端接收到RST报文，系统会将排队的这个未完成连接直接删除，此时就相当于没有任何的连接请求到来， 而**接着调用的accept()将会被阻塞**（阻塞整个线程），直到另外的新连接到来时才会返回。这是与IO多路复用的思想相违背的**(系统不阻塞在某个具体的IO操作上，而是阻塞在select、poll、epoll这些IO复用上的)**。

​		上述这种情况下，**如果监听套接字为非阻塞的，accept()不会阻塞住，立即返回-1，同时errno = EWOULDBLOCK。**

#### 1.listenfd非阻塞

使用IO多路复用API epoll时，如果设置listenfd阻塞，如果不能建立连接，会在accept处阻塞，从而阻塞整个主线程使其不能执行下去。

#### 2.clientfd非阻塞

1.使用epoll的ET模式
2.epoll返回读写事件，但不一定真的可读写

### Epoll在使用ET模式时，为什么要把监听的套接字设为非阻塞模式？

​	如果多个连接同时到达，ET模式下就只会通知一次，为了处理剩余的连接数，必须要时刻accpet，直到出现errno为EAGAIN。

我们会循环从文件描述符读写数据，那么如果文件描述符是阻塞的，没有数据可读写时，`线程会阻塞在读写函数那里，程序就没办法继续往下执行`。所以，边缘触发模式一般和非阻塞 I/O 搭配使用，程序会一直执行 I/O 操作，直到系统调用（如 read 和 write）返回错误，错误类型为 EAGAIN 或 EWOULDBLOCK。



### socket客户端连接上服务端是在listen之后而非在accept之时

​		在listen后，客户端就可以与服务器进行连接（TCP三次握手），此时的连接结果放在队列中。服务端之后调用 accept() 函数从队列中获取一个已准备好的连接，函数返回一个新的 socket ,新的 socket 用于与客户端通信，listen 的 socket 只负责监听客户端的连接请求。



### 线程池任务分配用的轮询算法

好像只有初始匹配了一次，通过轮询算法获得EventLoop，之后就没有task了，因为task数量也就是eventloop数量，适合线程数量一样的

### Server的单例

程序拓展情况下， 其他对象可能也调用Get_the_service()函数获取实例，单例模式在设计资源分配和初始化等操作的时候，考虑线程安全问题，防止多线程环境下可能出现的race condition等问题，考虑使用线程安全的初始化策略。

### 管线化

http状态机解析的过程中多报文解析，一次性解析所有的内容，报文之间有\r\n隔离，并且批量返回响应。 

### SO_REUSEADDR选项

 *SO_REUSEADDR允许启动一个监听服务器并捆绑其众所周知的端口，即使以前建立的将该端口用作他们的本地端口的连接仍存在。

 SO_REUSEADDR允许在同一端口上启动同一服务器的多个实例，只要每个实例捆绑一个不同的本地IP地址即可

### TCP_NODELAY选项

 TCP/IP协议中针对TCP默认开启了Nagle算法。Nagle算法通过减少需要传输的数据包，来优化网络。在内核实现中，数据包的发送和接受会先做缓存，分别对应于写缓存和读缓存。启动TCP_NODELAY，就意味着禁用了Nagle算法，允许小包的发送。对于延时敏感型，同时数据传输量比较小的应用，开启TCP_NODELAY选项无疑是一个正确的选择。 比如，对于SSH会话，用户在远程敲击键盘发出指令的速度相对于网络带宽能力来说，绝对不是在一个量级上的，所以数据传输非常少； 而又要求用户的输入能够及时获得返回，有较低的延时。如果开启了Nagle算法，就很可能出现频繁的延时，导致用户体验极差。

### epoll中max_event事件的确定

 超过 maxevents 的就绪事件会被抛弃吗？-----不会，超过的仍然挂在epoll的就绪任务队列中，没有取出来。通常maxevents 必须要小于epoll_wait第二个参数数组的大小，其实它的主要目的是防止越界访问event数组。换句话说，max_event主要影响的是并行程度，即一次调用能处理多少事件，特别小会限制并行度，特别大通常没有什么问题。

### 报文分批到达的处理

 有没有可能客户端发送的http请求报文由于网络受限从而分多次到达，进而可能导致一次ReadData函数调用无法获取完整的请求报文，从而让服务器误以为请求报文的语法不正确而关闭连接？

-----不会，读到不完整数据直接返回，等待下次EPOLLIN事件。缓冲区获得完整请求报文后用状态机处理

### 写数据时缓冲区满的处理

 write函数和send函数可能因为客户端或者服务端的缓冲已满，而不能继续写数据，从而可能导致一次WriteData函数不能把响应报文写完，这种情况又该如何处理？

-----首先删除定时器，避免因超时而没写完数据。然后重新注册EPOLLOUT事件，在下轮epoll监听时重新试着写数据，

### 时间器时间更改的几个节点

 1、建立连接时，时间器定时为header_time，Epoll

 2、分析到post请求时，判断数据是否完整前，时间器更改为body_time

 3、完成发送后，重置时间：如果为长连接，重置时间为keep_alive_time。如果是短连接，则删除is_conn事件，并删除时间器。

### backlog参数

 从内核2.2版本之后，listen函数的backlog参数表示的是**全连接的数量上限**。所谓全连接，指的是完成了tcp三次握手处于establish状态的连接。在listen后，客户端就可以与服务器进行连接（TCP三次握手），此时的连接结果放在队列中。服务端调用 accept() 函数从队列中获取一个已准备好的连接，函数返回一个新的 socket ,新的 socket 用于与客户端通信。backlog指的就是accept队列的长度在任何时候都最多只能为backlog。在5.4版本之后backlog的默认最大值为4096(定义在/proc/sys/net/core/somaxconn)。显然，backlog与服务器的最大并发连接数量没有直接的关系（accept得够快（从队列中取得够快）），只会影响服务器允许同时发起连接的客户端的数量。

[Linux中，Tomcat 怎么承载高并发（深入Tcp参数 backlog） - 三国梦回 - 博客园 (cnblogs.com)](https://www.cnblogs.com/grey-wolf/p/10999342.html)

 全连接队列满了怎么办？ -----在 accept 队列满的情况下，收到了三次握手中的最后一次 ack 包， 它就直接无视这个包。（此时，客户端是全连接状态（收到了第二次握手的报文），服务端是半连接状态（无视了第三次握手的报文）） 一开始，看起来有点奇怪，但是记得， 服务端SYN RECEIVED 状态（半连接状态）下的 socket 有一个定时器。

 该定时器的机制： 如果 ack 包没收到（或者被无视，就像我们上面描述的这个情况）， tcp 协议栈 会重发 SYN/ACK （第二次握手）包。（重发次数由 /proc/sys/net/ipv4/tcp_synack_retries 指定）。

## 编译配置

* Make工具依据Makefile文件，来批量处理编译；
* CMake工具能够根据CMakeLists文件，输出各种各样的Makefile或者project文件

<img src="D:\MyTxt\typoraPhoto\image-20230310172619978.png" alt="image-20230310172619978" style="zoom: 67%;" />

### Cmake编译C/C++程序方法

#### cmake基本写法

运行环境是Jetson Nano Ubuntu 20.04。

CMakeLists.txt文件内容结构及其写法
①说明本项目最低cmake配置要求

```cmake
cmake_minimum_required(VERSION 3.0.0)
```

②说明本项目名称和版本信息

```cmake
project(CmakeTestProject VERSION 0.1.0)
```

这里，这个项目名就是CmakeTestProject。版本号为0.1.0

③配置编译标准，例如C++标准，C标准，编译模式debug还是release，等等。

```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
```

> 举个例子，就是这样。例如第四排表示 用C++11标准，第五排表示用debug模式。
>
> 关于set()这个指令方法内容量是比较大的，能实现很多功能的设置，例如，设置项目生成可执行文件的路径。这在此处就不做详解了，其他博文也有挺多，不过好像讲得不是那么通俗易懂。
>
> set还可以用做变量定义，例如，我要将 根目录下的gen 路径定义为GEN_DEST变量名
>
> set(GEN_DEST ${CMAKE_CURRENT_SOURCE_DIR}/gen/)
> 此处这个${CMAKE_CURRENT_SOURCE_DIR}表示项目的根目录。就是CmakeList.txt存放的这一级目录。

④项目中所用到的库文件引用。这部分只会涉及.c和.cpp文件

```cmake
file(GLOB_RECURSE 自定义变量名 路径)
```

例如：

```cmake
file(GLOB_RECURSE ca_gen_srcs_cpp ${GEN_DEST}/src/*.cpp)
file(GLOB_RECURSE ca_gen_srcs_c ${GEN_DEST}/src/*.c)
file(GLOB_RECURSE usr_srcs_cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
```

这里就是讲后面路径下的所有.cpp或.c文件包含进前面这个变量里。

⑤引用头文件。这部分就只涉及.h和.hpp文件

```cmake
include_directories(路径)
```

 例如：

```cmake
include_directories(${GEN_DEST}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
这里就可以不用具体指定文件了。
```

⑥为了方便阅读，集合一下前面所用的file内容。就是set一下

```cmake
set(ALL_COMPILE_SRC
    ${ca_gen_srcs_cpp}
    ${ca_gen_srcs_c}
    ${usr_srcs_cpp}
)
```

把第④步骤的内容集合一下，合为一个变量叫ALL_COMPILE_SRC（也可以用其他变量名）。

⑦链接动态库，如果没有需要引用的动态库，此步骤可以省略，不过笔者项目需要：

```cmake
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
也就是
link_directories(路径)
```

 ⑧将源代码添加到此项目的可执行文件

```cmake
add_executable (server1 "server.cpp" ${ALL_COMPILE_SRC})
也就是
 add_executable (可执行文件名 "main.cpp" 所有相关的.c和.cpp文件)
```

 ⑨将目标文件与库文件进行链接

```cmake
target_link_libraries(server1 -lddsrpcc  -lddsc -lpthread -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs -lssl -lcrypto)
也就是
target_link_libraries(可执行文件名 -相关的动态库)
```

完整的代码示
为了方便同学们理解，此处做个完整的示例

```cmake
#CMakeList.txt: Demo 的 CMake 项目，在此处包括源代码并定义
#项目特定的逻辑。

cmake_minimum_required (VERSION 3.8)

project ("Demo")
set(CMAKE_C_FLAGS "$ENV{CFLAGS} -O2 -Wall -pthread")
set(CMAKE_CXX_FLAGS "$ENV{CFLAGS} -O2 -Wall -pthread -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(GEN_DEST ${CMAKE_CURRENT_SOURCE_DIR}/gen/)
set(DDS_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/)

file(GLOB_RECURSE ca_gen_srcs_cpp ${GEN_DEST}/src/*.cpp)
file(GLOB_RECURSE ca_gen_srcs_c ${GEN_DEST}/src/*.c)
file(GLOB_RECURSE mqtt_c ${CMAKE_CURRENT_SOURCE_DIR}/tools/mqtt/*.cpp)
file(GLOB_RECURSE json_c ${CMAKE_CURRENT_SOURCE_DIR}/tools/JSON/*.c)
file(GLOB_RECURSE uart_cpp ${CMAKE_CURRENT_SOURCE_DIR}/tools/uart/*.cpp)
file(GLOB_RECURSE usr_srcs_cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


include_directories(${GEN_DEST}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tools)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tools/mqtt)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tools/JSON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tools/uart)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tools/spdlog)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(ALL_COMPILE_SRC
    ${source_c}
    ${ca_gen_srcs_cpp}
    ${ca_gen_srcs_c}
    ${uart_cpp}
    ${mqtt_c}
    ${json_c}
    ${usr_srcs_cpp}
)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

#将源代码添加到此项目的可执行文件。

add_executable (server1 "server.cpp" ${ALL_COMPILE_SRC})
add_executable (client1 "client.cpp" ${ALL_COMPILE_SRC})
add_executable (client2 "client1.cpp" ${ALL_COMPILE_SRC})

target_link_libraries(server1 -lddsrpcc  -lddsc -lpthread -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs -lssl -lcrypto)
target_link_libraries(client1 -lddsrpcc  -lddsc -lpthread -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs -lssl -lcrypto)
target_link_libraries(client2 -lddsrpcc  -lddsc -lpthread -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs -lssl -lcrypto)

#TODO: 如有需要，请添加测试并安装目标。
```

编译
接下来就开始见证编译时刻了。

```cmake
#在项目根目录下，创建个build文件（为啥是build呢，笔者认为应该是行业规范吧）

#命令行代码：
cd 该项目根目录下
mkdir build
cd build
#到该build路径后，开始预编译，再编译

cmake ..
make
#make完后，在build目录下就应该会有可执行文件了！
```

#### 外部导入外部库 `FetchContent`

> 功能：外部导入外部库。FetchContent 是 3.11.0 版本开始提供的功能，只需要一个 URL 或者 Git 仓库即可引入一个库。

* 使用方法

1. include(FetchContent) ：表示引入 FetchContent。
2. FetchContent_Declare(第三方库) ：获取第三方库，可以是一个 URL 或者一个 Git 仓库。
3. FetchContent_MakeAvailable(第三方库) ：将这个第三方库引入项目。
4. target_link_libraries(主项目 PRIVATE 子模块::子模块) ：链接这个第三方库。

* 实例

```cmake
cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
```

### SSH

#### Ubuntu链接github by ssh

下载shh 配置，生成shh秘钥，公有密钥添加到git账号中。

 ### Git

* 分布式是没有一个主版本号的，它们都是用id来做标志的，同时用master作为主仓库，其它的分支怎么迭代都不会影响到master

#### 基本指令

```shell
git init //初始化
ls -ah //查看配置文件

git add test.c  //提交到缓存
git add --all	
git commit -m "add new file \"test.c\""	//提交仓库
git commit --amend 	//重写上一次提交信息

git rm test	  //删除文件，也需要commit #不过好像rm后add --all后commit 更常用
git status	//提交之后查看文件是否做了改动
	A：未修改	AM：修改	Untracked：未提交	modified：新文件，但未提交
	
git log  			//查看日志
git log test.c		//查看单个文件可回滚版本
git reflog			//查看提交历史
git reset --hard 要回滚id	//回滚    #记得回滚之后 重新提交，使文件保持最新
git checkout -- test.c 	//恢复到上一次提交的状态 #git checkout的作用是检出，如果是文件的话，会放弃对文件的缓存区操作，但是要使用reset重置一下变更才行；如果是分支的话会切换过去。

git branch -a	//查看当前所有分支
git branch brachname	//创建分支但不切过去
git checkout -b branchname 	//创建分支并切换过去
git checkout master		//切换分支

git branch -m 分支名 新的分支名	//修改分支名称
#解决git不提交代码不能切换分支的问题
git stash	//保存当前工作状态
git stash list	//查看当前存储了多少工作状态
#git分支合并
git checkout master切换到master
git merge branchname将其合并

git branch -D branchname	//删除本地分支
git push origin --delete branchname	//删除远程分支

#远程连接git仓库 传文件
git init //初始化
git remote add origin git@github.com:forthdifferential/WebServer1.0.git		//关联
git add ****
git commit ***
git push -u origin master		//推送远程
## 相关问题
git remote -v  // 命令来确认你的远程仓库列表，并查看已经存在的远程仓库的URL
git remote set-url // 如果你确实需要修改远程仓库的URL，你可以使用 
git checkout -b master // 如果是因为本地没有master分支，你想要创建 master 分支，应该使用


git clone git@github.com:forthdifferential/WebServer1.0.git	//远程仓库关联到本地
git clone -b master git@github.com:forthdifferential/WebServer1.0.git	//按照分支拉取

```

#### GIt基本组成框架

1. Workspace：开发者工作区    ： 工作区就是你当前的工作目录；

2. Index / Stage：暂存区/缓存区   ： 这里存放了你使用git add命令提交的文件描述信息，它位于.git目录下的index下面，用来存放临时动作，比如我们做了git add或者git rm，都是把文件提交到缓存区，这是可以撤销的，然后在通过git commit将缓存区的内容提交到本地仓库

3. Repository：仓库区（或本地仓库）： git会保存好每一个历史版本，存放在仓库区，它可以是服务端的也可以是本地的，因为在分布式中，任何人都可以是主仓库。

4. Remote：远程仓库 ：只能是别的电脑上的仓库，即服务器仓库

#### git的分支

在团队开发中，master只有一个，合作开发里任何人都可以从master里拉取代码，拉取时master后创建分支，分支名改为你要做的操作，分支名必须简洁，和标题一样，提交的commit在简单描述一下就可以了，比如修改某某文件，修改什么什么bug，单词以下划线做分割，然后在提交一个版本。

> 如我们的master中有个bug，是内存泄漏
>
> 我们可以常见一个分支名为Memory_Leak,然后在commit里简单描述一下修复了哪个模块的内存泄漏，不要写修复了什么什么代码，什么什么问题导致的，只需要简单描述一下就可以了。
>
> 一般情况下，我们都是拉取master后，想要修改功能或者添加功能，都是创建分支，在分支里修改不影响master，如果修改错了代码或者误删之类的，在从master上拉取一份就可以了。

