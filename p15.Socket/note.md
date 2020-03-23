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

#### 命名套接字
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

#### 创建套接字队列
为了能够在套接字上接受进入的连接, 服务器程序必须创建一个队列来保存未处理的请求. 它用 `listen` 系统调用来完成这一工作.
```C
#include <sys/socket.h>

int listen (int socket, int backlog);
```

`backlog` 对套接字队列长度进行限制, 超过数量的连接将被拒绝. `listen` 函数成功时返回0, 失败时返回-1, 错误代码包括 `EBADF`, `EINVAL` 和 `ENOTSOCK`.

#### 接受连接
一旦服务器创并命名套接字后, 就可以通过 `accept` 系统调用来等待客户建立对该套接字的连接.
```C
#include <sys/socket.h>

int accept (int socket, struct sockaddr *address. size_t *address_len);
```

`accept` 系统调用只有当有客户程序试图连接到由 socket 参数指定的套接字上时才返回. 

#### 请求连接

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
