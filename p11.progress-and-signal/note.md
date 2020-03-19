# 进程

# 信号
信号是系统响应某些条件而产生的时间. 在文档中用术语 `raise` 表示生成事件, `catch` 表示捕获事件.

信号的产生原因通常是一些错误条件, 例如内存段冲突, 浮点数处理器错误或非法指令. 它们由shell和终端处理器生成来引起中断, 同时也可以用于向进程传递消息.

信号定义见 `signal.h` 头文件. 常见的信号有 `SIGINT`, 可以通过终端的 `ctrl+C` 向当前进程发送 `SIGINT` 信号. `SIGUSR1` 和 `SIGUSR2` 则是用于用户自定义消息.

## 生成信号
系统的 `kill` 程序即是一个信号生成程序, 可以向指定进程发送指定信号, 可以通过 `man 1 kill` 查看详细介绍. `kill` 程序还有个变体 `killall` 可以指定程序名来给执行这个程序的进程发送信号.

编程中系统提供了同名函数 `kill` 用于向进程发送信号.
```C
#include <sys/types.h>
#include <signal.h>

int kill (pid_t pid, int sig);
```
`kill` 调用将指定信号 `sig` 发送到指定进程 `pid`. 发送信号需要有相应的权限, 即目标进程与发送进程属于同一个用户. root用户例外, 他可以向所有进程发送信号.

`kill` 调用在成功时返回0, 在失败时返回 -1, 并且用 `errno` 变量记录错误码. 失败原因通常是给定信号无效(`EINVAL`), 发送进程权限不够(`EPERM`), 目标进程不存在(`ESRSH`).

## 定时信号
信号实现提供的定时器, 在设置定时超过后会为进程发送信号 `SIGALRM`. 由于系统调度等原因, 这个定时信号可能比设置的时间晚.

设置定时器的函数 `alarm`:
```C
#include <unistd.h>

unsigned int alarm (unsigned int seconds);
```

`alarm` 调用提供设置定时器的秒数, 返回上一次定时剩余的时间, 如果定时器已经触发则返回0. `alarm` 定时器每个进程只有一个, 如果前一个定时器没有触发就又进行了 `alarm` 调用那么定时器时间会被更新并重新计时.

## 处理信号
### signal
```C
#include <signal.h>

void *signal(int signum, void (*handler)(int));
```

`signal` 为信号 `signum` 设置捕获程序 `handler`, 设置成功后返回原有的捕获程序. `signal.h` 为 handler 参数提供了两个处理函数的宏, `SIG_DFL` 可以把信号的捕获程序恢复成原有设定, `SIG_IGN` 可以忽略信号不进行处理.

### sigaction
```C
#include <signal.h>

struct sigaction
{
    void (*)(int) sa_handler,
    sigset_t sa_mask,
    int sa_flags
}

int sigaction (int signum, struct sigaction *act, struct sigaction *oact);
```

`sigaction` 的函数调用提供了更细粒度的控制以及更友好的错误处理方式.

`sigaction` 在设置成功时返回0, 失败时返回-1. 如果给出的信号无效或对不可捕获/忽略信号进行设置, 错误变量 `errno` 会被设置为 `EINVAL`. 参数方面使用了 `oact` 参数提供的指针来保存原有的设置. `act` 参数的 `sa_handler` 属性来设置捕获程序.

`struct sigaction` 的 `sa_mask` 属性是一个信号集, 这个信号集用于指定在信号处理时应当阻塞的信号. 如果不进行信号阻塞, [捕获程序可能会被新的信号打断执行](./code/gsigint_test.v3.c), 也有可能造成信号丢失等问题.

## 信号集
### 基本操作
```C
#include <signal.h>

int sigaddset(sigset_t *set, int signo);
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigdelset(sigset_t *set, int signo);

int sigismember(sigset_t *set, int signo);
```

以上都是一些简单函数用于信号集相关的操作. 他们成功时返回0, 失败时返回-1并设置 `errno`. 当给定信号无效时, `errno` 被设置为 `EINVAL`. `sigismember` 略有不同, 它用于判断信号是否信号集中, 如果是则返回1, 不是就返回0, 当给定信号无效会返回-1并设置 `errno` 为 `EINVAL`.

### 直接设置屏蔽信号
```C
#include <signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
```

`sigprocmask` 可以直接为程序设置屏蔽信号集, 用户可以选择保留原信号集. 如果操作成功, 返回0. 如果 `how` 参数无效, 返回-1并设置 `errno` 为 `EINVAL`. 它根据 how 参数提供多种模式:

|  arg `how`  | description                               |
| :---------: | :---------------------------------------- |
|  SIG_BLOCK  | 将 `set` 中的信号添加到信号屏蔽集中       |
| SIG_SETMASK | 将被屏蔽的信号设置为 `set` 信号集中的信号 |
| SIG_UNBLOCK | 将 `set` 信号集中的信号设置为不再阻塞     |

>问: 当一个信号解除屏蔽后, 之前发送的信号是否还存在? 是否会被后续处理?
>答: 存在, 但多个相同信号会被合为一个. [见代码](./code/sigint_test.v4.c)

### 其他信号集操作
```C
#include <signal.h>

int sigpending(sigset_t *set);
int sigsuspend(const sigset_t *mask);
```

`sigpending` 用于将处于被阻塞状态的一组信号写到参数 `set` 中. 成功时返回0, 否则返回-1并设置 `errno`.

>`sigpending` 只是检查, 并不会将阻塞的信号移出队列. [示例代码](./code/sigint_test.v5.c)

`sigsuspend` 可以临时将进程当前的屏蔽信号集替换为参数 `mask` 提供的信号集, 并挂起等待一个信号. 如果接收到的信号导致进程终止, 则函数不会返回, 否则返回-1, 并设置 `errno` 为 `EINTR`.
