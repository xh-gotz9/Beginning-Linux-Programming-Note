# 进程间通信:管道
当一个进程连接数据流到另一个进程时, 我们使用管道 (`pipe`). 通常是把一个进程的输出连接到另一个进程的输入. 这在 shell 中很常见, 用管道字符 `|` 即可完成, 例如 `ps aux | grep init`. Linux 可以通过程序实现同样的效果.

## popen调用
```C
#include <stdio.h>

FILE *popen (const char *command, const char *opem_mode);
int pclose (FILE *stream_to_close);
```
`popen` 可以将一个程序作为新进程启动, 并且可以通过返回的 `FILE` 对象向新进程读或写数据, 在新进程中作为程序的标准输入或标准输出. 调用成功时返回管道对象的 `FILE` 指针, 失败时返回 `NULL`.

`open_mode` 决定打开的管道是用于读数据还是写数据. 如果 `open_mode` 值为 `r`, 那么这个管道可以用于读取被调用程序的标准输出. 如果 `open_mode` 值为 `w`, 那么这个管道可以用于向被调用程序的标准输入写数据.

由于开启的管道是 `stdio.h` 提供的 `FILE` 对象, 所以用库中提供的其他相关函数即可进行操作.

>从函数库文件可以看出, 管道属于 C 标准IO库, 即可以跨平台使用 (虽然实现不同).

[读入数据例程](./code/pipe_read.c), [写入数据例程](./code/pipe_write.c).

## 性能讨论
从 `popen` 为了启动进程, 首先会启动 `shell` 来解析命令, 然后启动程序.

使用 `shell` 可以让 `popen` 执行复杂命令, 无需手动完成参数扩展. 但是, 每次 `popen` 调用都会启动一个进程并且还要启动 `shell`. 这样, 一次调用就需要启动两个进程, 开销成本略高, 并且依赖 `shell` 可能会导致程序运行效果难以控制, 例如系统 `PATH` 变量被修改等问题.

## pipe系统调用
了解过 `write`, `read` 系统调用都会知道, 系统底层是以 fd 表示文件的, 而 `popen` 使用的却是 `FILE`, 显然它是被封装的高级调用. `popen` 作为高级调用, 必然有系统调用予以支撑, 这就是 `pipe` 系统调用.
```C
#include <unistd.h>

int pipe (int file_descriptor[2]);
```
`pipe` 调用的参数是两个文件描述符组成的数组的指针, **`pipe` 调用成功后会将管道fd写入数组**. 调用成功后函数返回0, 失败则返回-1并设置 `errno` 的值. 此处设置 `errno` 常见可能的值有 `EMFILE`(文件描述符过多), `ENFILE`(系统文件表已满), `EFAULT`(文件描述符无效).

`fd[1]` 用于写数据, 从 `fd[0]` 可以读取写入到 `fd[1]` 的数据, 数据流向遵循 FIFO(先进先出) 原则.

示例代码见文件 [sys_pipe_test.c](./code/sys_pipe_test.c)

### 管道的应用
管道的应用思路要点包括以下几点:
1. `fork` 调用后会复制 fd.
2. `exec` 调用后只是改变进程运行的程序, 不清理其他环境设置. 
3. fd的分配总是从最小可用的值开始分配, 如果将 fd[0] 关闭并打开新的fd, 那么新打开的fd值为0, 即新打开的文件将作为标准输入使用, fd[1] , fd[2]同理.

关于第2点的实验代码见 [progress_fd_test.c](./code/progress_fd_test.c). 
