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

int signal(int signum, void (*handler)(int));
```
