# 套接字

## 套接字属性
1. 套接字的域

    套接字的域指定通信中使用的网络介质. 最常用的是 `AF_INET`, 即 Ineternet 网络. 其底层的网际(IP)协议只有一个地址族, 即IP地址, 用于指定网络中的计算机.
    
    书中P516例程使用的是 `AF_UNIX` 域, 其底层协议就是文件I/O, 地址则是文件名. 其他域见详细文档.

2. 套接字类型
   
   一个套接字域可能有不同的通信方式, 而每种通信方式有各自的特性.

   `AF_UNIX` 域套接字提供了可靠的双向通信. 在网络域中, 就要注意传输介质的特性.

   因特网协议提供了两种通讯机制: 流(`stream`)和数据报(`datagram`), 它们有着截然服务方式.

   - 流套接字
    
        流套接字提供有序可靠双向字节流的连接, 发送的数据保证不会丢失或错乱. 由 `SOCK_STREAM` 指定, 在 `AF_INET` 中通过 `TCP/IP` 协议实现.
   
   - 数据报套接字
        
        数据报套接字由 `SOCK_DGRAM` 指定, 由 `UDP/IP` 协议实现. 它对发送数据的长度有限制, 且不保证数据发送后的可达性, 可能出现丢失, 复制或乱序到达的问题. 使用开销相对流套接字更小(毕竟可靠性都不保证了).

3. 套接字协议

     如果底层传输机制允许不止一个协议来提供指定套接字类型的服务, 那就可以通过协议属性指定使用特定的协议实现.

## 创建套接字
```C
#include <sys/types.h>
#include <sys/socket.h>

int socket (int domain, int type, int protocol);
```

`socket` 创建的套接字是套接字通讯的一端, `domain` 指定域, `type` 指定套接字类型, `protocol` 指定套接字使用的协议.

|    domain    | description                       |
| :----------: | :-------------------------------- |
|   AF_UNIX    | UNIX 域协议 (文件系统套接字)      |
|   AF_INET    | ARPA 因特网协议 (UNIX 网络套接字) |
|    AF_ISO    | ISO 标准协议                      |
|    AF_NS     | 施乐(Xerox)网络系统协议           |
|    AF_IPX    | Novell IPX协议                    |
| AF_APPLETALK | Appletalk DDS                     |

`AF_UNIX` 常用于实现本地套接字通信, `AF_INET` 常用于实现网络套接字通讯.

## 使用套接字
### 套接字地址
#### AF_UNIX
   
对于 `AF_UNIX` 域, 地址由 `sockaddr_un` 结构来描述, 定义在 `sys/un.h` 头文件中, 至少包含以下属性:
```C
struct sockaddr_un
{
     sa_family_t sun_family; /* AF_UNIX */
     char sun_path[]; /* pathname */
}
```

`sun_family` 属性的值总是 `AF_UNIX`. `sun_path` 是套接字文件的地址, 目前文档中限定长度为 108 字节.

#### AF_INET
对于 `AF_INET` 域, 地址结构由 `sockaddr_in` 指定, 定义在头文件 `netinet/in.h` 中, 至少包含以下属性:
```C
struct sockaddr_in
{
     short int sin_family; /* AF_INET */
     unsigned short int sin_port; /* Port number */
     struct in_addr; /* Internet address */
}

struct in_addr
{
     unsigned long int s_addr;
}
```

IPv4 地址是由四个字节组成一个32位的值, 所以使用了 `unsigned long int`.

### 命名套接字
```C
#include <sys/socket.h>

int bind (int socket, const strcut sockaddr *address, size_t address_len);
```

`bind` 系统调用把参数 address 中的地址分配给与文件描述符 socket 关联的未命名套接字. 地址结构的长度由参数 `address_len` 传递.

地址长度和格式取决于地址族. `bind` 调用需要将一个地址结构指针转换为指向通用地址类型 `struct sockaddr *`.

`bind` 调用成功时返回0, 失败时返回-1, 并且设置errno为下列值:

|         errno         | description                                          |
| :-------------------: | :--------------------------------------------------- |
|         EBADF         | 文件描述符无效                                       |
|       ENOTSOCK        | 文件描述符对应的不是一个套接字                       |
|         EINAL         | 文件描述符对应的是一个已命名套接字                   |
|     EADDRNOTAVAIL     | 地址不可用                                           |
|      EADDRINUSE       | 地址已经绑定了一个套接字                             |
|        EACCESS        | 因为权限不足, 不能创建文件系统中的路径名 (`AF_UNIX`) |
| ENOTDIR, ENAMETOOLING | 表明选择的文件名不符合要求 (`AF_UNIX`)               |

### 创建套接字队列
为了能够在套接字上接受进入的连接, 服务器程序必须创建一个队列来保存未处理的请求. 它用 `listen` 系统调用来完成这一工作.
```C
#include <sys/socket.h>

int listen (int socket, int backlog);
```

`backlog` 对套接字队列长度进行限制, 超过数量的连接将被拒绝. `listen` 函数成功时返回0, 失败时返回-1, 错误代码包括 `EBADF`, `EINVAL` 和 `ENOTSOCK`.

### 接受连接
一旦服务器创并命名套接字后, 就可以通过 `accept` 系统调用来等待客户建立对该套接字的连接.
```C
#include <sys/socket.h>

int accept (int socket, struct sockaddr *address. size_t *address_len);
```

`accept` 系统调用只有当有客户程序试图连接到由 socket 参数指定的套接字上时才返回. 

### 请求连接
```C
#include <sys/socket.h>

int connect (int socket, const struct sockaddr *address, size_t address_len);
```
参数 `socket` 指定的套接字将连接到参数 `address` 指定的服务器套接字, `address` 指向的结构的长度由参数 `address_len` 指定.

#### 读写数据
```C
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recv (int sockfd, void *buf, size_t len, int flags);
ssize_t send (int sockfd, const void *buf, size_t len, int flags);
```

### 设置非阻塞模式
```C
int flag = fcntl (socket, F_GETFL, 0);
fcntl (socket F_SETFL, O_NONBLOCK | flag);
```
设置非阻模式后, 原本会发生阻塞的fd操作会变为非阻塞模式.

## 套接字选项
可以使用 `setsockopt` 为套接字设置各种属性来改变它们的行为.
```C
#include <sys/socket.h>

int setsockopt (int socket, int level, int option_name, const void *option_value, size_t option_len);
```

`level` 的参数的选项参考 [GUN文档](https://www.gnu.org/software/libc/manual/html_node/Socket_002dLevel-Options.html)

重点常用 `level`
|    level     | description                                                                                                   |
| :----------: | :------------------------------------------------------------------------------------------------------------ |
| SO_KEEPALIVE | 下层协议是否需要定期向连接发送消息, 如果消息发送失败, 则连接中断. 选项值是 `int` 类型, 一个非0值表示开启选项. |
| SO_BROADCAST | socket 的**数据报**是否允许被广播. 选项值是 `int` 类型, 非0值表示允许.                                        |
|  SO_RCVBUF   | socket 缓冲区的大小. 选项值是 `size_t`.                                                                       |


## 多客户的处理方式 select
`select` 调用可以对多个文件描述符进行管理.

```C
#include <sys/types.h>
#include <sys/time.h>

int select (int nfds, fd_set, *readfds, fd_set *write_fds, fd_set *errorfds, struct timeval *timeout);
```
关于 `nfds`, 文档中提到, `nfds` 必须是 `readfds`, `write_fds`, `errorfds` 中最大的fd值加1. 但是 glibc 中 `fd_set` 是固定长度的数据结构, `FD_SETSIZE` 被设定为 1024. `fd_set` 的操作受其限制, 最大的可监视fd值为1023. 如果想要监视超过 1023 以上的fd, 可以用 `poll` 调用替代.

`fd_set` 是一个用于存储一个或多个 fd 的数据结构类型, 有配套的设置函数:
```C
#include <sys/types.h>
#include <sys/time.h>

void FD_ZERO (fd_set *fdset);
void FD_CLR (int fd, fd_set *fdset); // 从 fdset 中删除 fd
void FD_SET (int fd, fd_set *fdset);
void FD_ISSET (int fd, fd_set *fdset);
```

`select` 可以设置一个定时器, 根据给出的时间 `timeout`, 在超时后返回, 避免阻塞过久. `timeout` 的结构体 `timeval` 如下:
```C
struct timeval {
     time_t tv_sec; /* seconds */
     long tv_usec; /* microseconds */
}
```

`select` 调用会在 `readfds` 中有fd可读, `writefds` 中有fd可写, `errorfds` 中有fd发生错误时返回, 或者在阻塞超过 `timeout` 设定的时间后返回. 如果没有提供 `timeval` 参数, 那么 `select` 调用会一直阻塞.

### select 的使用流程
具体实现见代码 [inet_server_select](./code/inet_server_select.c).

1. 创建 `fd_set`. 可以创建多个 `fd_set` 用于侦听读/写/错误三种问题.
2. 调用 `select` 等待发生事件.
3. 发生事件后返回, 轮询检查 fd_set 中的fd是否有相应事件发生, 进行处理.

可以看出, 第3步需要做非常多的额外工作, 而且加上 `fd_set` 的限制, `select` 看起来并不那么好用.

## poll
在 `select` 文档中提到, 如果需要监视的fd大于1023, 需要使用 `poll`. 

```C
#include <poll.h>

struct pollfd {
     int fd;
     short events;
     short revents;
}

int poll (struct pollfd *fds, nfds_t nfds, int timeout);
```

`poll` 需要传入 `pollfd` 的数组, 由 `fds` 指向数组头部, `nfds` 指定了数组长度. `poll` 相比 `select` 提供了更简单的超时设置方式, 直接指定超时上限为 `timeout` 毫秒.

`pollfd` 结构体中, `fd` 是要监视的fd值, `events` 作为输入值, `revents` 对监视结果进行返回. 如果 `fd` 值为负数, `events` 将被忽略, `revents` 将返回 0. 

`events` 是一个 bit mask 值, `revents` 可能的可返回值包含 `events` 的值以及 `POLLHUP`, `POLLER`, `POLLNVAL`. `events` 的值定义在 `poll.h` 中:

|   events   | description                                                                         |
| :--------: | :---------------------------------------------------------------------------------- |
|   POLLIN   | 有数据可读.                                                                         |
|  POLLPRI   | 可能有带外数据.                                                                     |
|  POLLOUT   | 可写数据, 虽然如果写入比 socket 缓冲区大的数据仍会造成阻塞. (除非设置了非阻塞模式). |
| POLLRDHUP  | 流 socket 对端关闭了连接或者停止了写入连接.                                         |
| POLLRDNORM | 等效于 `POLLIN`                                                                     |
| POLLRDBAND | 有优先数据可读                                                                      |
| POLLWRNORM | 等效于 `POLLWR`                                                                     |
| POLLWRBAND | 可写入优先数据                                                                      |
|  POLLMSG   | 虽然Linux定义了这一 event, 但不要使用                                               |
|  POLLERR   | 发生了错误条件, 这个 event 仅写入 `revents`. 也可表示一个fd的读取端已经关闭.        |
|  POLLHUP   | 挂起, 仅写入 `revents`. 表示关闭了连接的一端, 后续的读取在读完已到达数据后会返回零. |
|  POLLNVAL  | 无效请求, 仅写入 `revents`. fd 没有打开.                                            |

### poll 的工作流程
1. 为 fd 初始化 `pollfd` 结构体, 设置 fd 感兴趣的事件.
2. 创建 `pollfd` 数组将 `pollfd` 结构体填入.
3. 循环调用 `poll`, 等待事件发生.
4. 发生事件后, 根据 `poll` 的返回值确定产生事件的fd数量, 遍历 `pollfd` 依次检查.
5. 对有事件发生的 fd 执行相应的操作.