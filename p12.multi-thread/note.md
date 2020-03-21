# 多线程
## 线程概念
"线程是进程内部的一个控制序列".

`POSIX 1003.1c` 规范将线程标准化. 1996年 Linux 就获得了 `LinuxThread` 库提供的线程支持, 其与 POSIX 规范的差别并不大, 明显的区别是信号处理部分, 是受当时的系统内核限制导致的.

优点:
- 简化程序设计复杂度
- 作业拆分多线程可以避免单线程作业的时候阻塞而无法进行其他作业.
- 可能提高在多核CPU中的效率
- 一般而言线程切换的代价比进程切换的代价小.

缺点:
- 编码复杂度加大, 尤其在多线程之间同步, 访问共享数据等问题上.
- 调试困难.
- 运行环境可能导致处理效率不如单线程, 例如进程仅分配到一个处理核心.

## 使用线程库
线程库中绝大多数函数名都以 `pthread_` 开头. 为了使用线程库, 需要做如下准备:
- 定义宏 `_REENTRANT`.
- 在程序中包含头文件 `pthread.h`.
- 编译时链接库文件 `-lpthread`.

### 可重入化
举个简单例子, 一个函数计算过程使用了外部变量来存储中间值或结果值. 在多线程的情况下, 两个线程同时调用这个函数, 假设两者执行时间有重叠部分, 那么有可能前者在计算时先修改了外部变量, 后者立即修改了外部变量为新的计算值, 这就容易导致前者调用出现计算错误等问题, 破坏程序流程.

可重入化, 即使得这个函数在两个线程同时调用时结果不会互相影响, 最简单的办法既是将依赖的外部内存改为每次调用都使用单独的内存副本.

宏 `_REENTRANT` 即是设置在编译时使用可重入化的程序库, 必须定义于所有 `include` 语句之前(或者使用编译命令 `-D_REENTRANT`). 它会产生以下影响:
- 将某些函数调整为可安全重入的版本, 这些函数名不会发生改变, 但会添加 `_r` 后缀. 例如 `gethostbyname` 会变为 `gethostbyname_r`.
- `stdio.h` 原来以宏形式实现的一些函数会编程可安全重入函数.
- `errno.h` 定义的变量 `errno` 会变成一个函数调用, 用于获取调用线程的 `errno` 值.

### 创建线程
```C
#include <pthread.h>

int pthread_create (pthread_t *thread, pthread_attr_t *attr, void *(*start_routin)(void *), void *arg);

void pthread_exit (void *retval);

int pthread_join (pthread_t th, void **thread_return);
```

`pthread_create` 根据 `attr`, `start_routin` 和 `arg` 提供的值创建一个线程, 将线程的id值写入 `thread` 指针的位置. 参数 `start_routin` 是一个函数指针, 指向 参数和返回值为 `void *` 的函数, `arg` 将作为 `start_routin` 的调用参数传入. 函数调用成功返回0, 失败时返回相关错误代码.

`pthread_exit` 用于线程终止执行, 并可以返回一个指针作为线程的"返回值". 注意, 线程退出后函数执行栈将被清理, 所以局部变量无法有效保存, 导致返回的指针变为野指针. 使用一个野指针科会引发各种程序错误.

`pthread_join` 用于挂起当前线程, 等待指定的线程运行结束. 并且可以通过双重指针 `thread_return` 获取线程的返回值. 函数调用成功返回0, 失败时返回相关错误代码.

示例代码, 见文件 [thread_test_1.c](./code/thread_test_1.c).

## 同步
同步用于控制线程间的协作. 可以使用 `信号量` 和 `互斥量` 来控制.

### 信号量
系统提供了两组接口用于实现信号量. 一组是 `POSIX` 的实时扩展, 用于线程. 另一组是 `SystemV` 信号量, 常用于进程间同步(见14章).

```C
#include <semaphone.h>

int sem_init (sem_t *sem, int pshared, unsigned int value);
int sem_destroy (sem_t *sem);
```

`sem_init` 初始化 `sem` 指向的信号量对象, `pshared` 设定信号量的共享范围, `value` 则设定信号量的初始值.

信号量在初始化时根据 `pshared` 的值决定共享范围, 如果 `pshared` 为0, 则在进程的线程间共享. 如果 `pshare` 非0, 那么线程则在进程间共享, 需要将信号量存储在共享内存中(详见14章).

```C
int sem_wait (sem_t *sem);
int sem_post (sem_t *sem);
```
以上对信号量的操作函数 wait 和 post 都是原子操作. 这意味着多个 wait 和多个 post 的操作是单一有序的, 不会同时发生. 避免了并发修改数据时出现的覆盖问题.

`sem_post` 对信号量的 value 执行加1操作. 而 `sem_wait` 会对信号量的 value 执行减1操作, 如果信号量的值小于1, 调用将被阻塞, 直到 value 的值被 post 调用增加. 由于 wait 是原子操作, 所以当多个线程正在调用 `sem_wait` 阻塞中时, 信号量被 post一次, 那么仅会有一个线程会成功执行完 wait 操作, 继续执行.

示例代码 见 [thread_semaphone_test.c](./code/thread_semaphone_test.c).

### 互斥量
```C
#include <pthread.c>

int pthread_mutex_init (pthread_mutex_t *mutex, const pthread_mutexattr_t *mutex_attr);

int pthread_mutex_destroy (pthread_mutext_t *mutex);

int pthread_mutex_lock (pthread_mutex_t *mutex);

int pthread_mutex_unlock (pthread_mutex_t *mutex);
```

这些函数成功时返回0, 失败时返回错误代码, 所以需要进行返回值检查.

互斥量使用方式与信号量类似. 要注意的点是, 互斥量默认的类型为 fast, 这一设置会导致尝试对一个已经锁定的互斥量调用 `pthread_mutex_lock` 时阻塞. 而如果一个线程对互斥量锁定成功后再次进行 lock, 那这将会导致死锁. 这可以通过修改互斥量属性来避免, 由于书中并不讨论属性, 所以需要另行查阅资料.

示例代码 见 [thread_mutext_test.c](./code/thread_mutex_test.c)

## 线程的属性
在 `pthread_create` 的参数中, 有一项是 `pthread_attr_t *attr`, 它用于设置线程的个高级属性.

之前的代码示例中, 如果 main 线程运行结束而不执行 `pthread_join`, 进程就会终止, 子线程会直接随进程终止. 假设我们需要一个线程在 main 函数结束后继续运行而不需要调用 `pthread_join`, 那么就需要创建脱离线程 `detached thread`. 创建脱离线程就需要对创建时的属性进行设置. 

`pthread_attr_t` 属于复合类型, 同样有一系列函数对它进行创建修改操作:
```C
#include <pthread.h>

int pthread_attr_init (pthread_attr_t *attr);
int pthread_attr_destroy (pthread_attr_t *attr);
```
这些函数成功时返回0, 失败时返回非0值. destroy 负责对对象进行清理回收, 一旦被清理回收, 这个对象需要重新初始化才可使用.

以下是用于修改属性值的操作函数:
```C
#include <pthread.h>

int pthread_attr_setdetachstate (pthread_attr_t *attr, int detachstate);
int pthread_attr_getdetachstate (pthread_attr_t *attr, int *detachstate);

int pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy);
int pthread_attr_getschedpolicy (pthread_attr_t *attr, int *policy);

int pthread_attr_setschedparam (pthread_attr_t *attr, const struct sched_param *param);
int pthread_attr_getschedparam (pthread_attr_t *attr, const struct sched_param *param);

int pthread_attr_setinheritsched (pthread_attr_t *attr, int inherit);
int pthread_attr_getinheritsched (pthread_attr_t *attr, int *inherit);

int pthread_attr_setscope (pthread_attr_t *attr, int scope);
int pthread_attr_getscope (pthread_attr_t *attr, int *scope);

int pthread_attr_setstacksize (pthread_attr_t *attr, int stacksize);
int pthread_attr_getstacksize (pthread_attr_t *attr, int *stacksize);
```

根据这些函数,可以看到至少有以下属性:
|    fields    | description                                                                                             |
| :----------: | :------------------------------------------------------------------------------------------------------ |
| detachstate  | 线程的脱离状态, 脱离线程会在main线程结束后继续维持进程运行. 根据脱离状态也会导致等待线程的方式发生变化. |
| schedpolicy  | 线程调度策略.                                                                                           |
|  schedparam  | 线程调度策略相关的参数.                                                                                 |
| inheritsched | 参数继承选项. 选择是否继承创建者线程使用的线程属性设置.                                                 |
|    scope     | 线程调度竞争的范围. 可以选择与系统所有线程竞争调度或与进城内线程竞争调度.                               |
|  stacksize   | 线程申请的最小内存值(byte), 不可小于 `PTHREAD_STACK_MIN`.                                               |

以上只是这些参数的简单介绍, 详细使用以及更多的参数设置需参考文档.

## 取消线程
一个线程可以要求另一线程终止. 但线程终止并不是被要求后就强制执行, 在被要求终止方的线程控制权力更大.

```C
#include <pthread.h>

int pthread_cancel (pthread_t th);
```

发起申请的线程调用 `pthread_cancel`, 对指定线程发出终止请求. 如果符合被请求方线程的设置, 那么线程将会终止.

```C
#include <pthread.h>

int pthread_setcancelstate (int state, int *oldstate);

int pthread_setcanceltype (int type, int *oldtype);
```

在被请求终止方的线程, 首先要根据 `cancelstate` 决定是否可以被终止, 如果当前线程设置为可以被终止, 那么再根据 `canceltype` 的值在适当的时候停止线程. 详情见文档.

根据 `canceltype` 决定的点来停止线程可能会让线程的行为脱离掌控. 如果需要手动检查, 可以使用 `pthread_testcancel`, 这个函数会检查是否接收到取消请求, 如果有, 则直接终止线程.

如果对一个被取消的线程调用 `pthread_join`, 根据文档, `*retval` 的值会是 `PTHREAD_CANCELED`.