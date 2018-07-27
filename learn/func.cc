#include<cstudio>
void byteorder(){
    union{
        short value;
        char union_bytes[ sizeof(short) ];
    } test;
    test.value = 0x0102;
    if( (test.union_bytes[0]==1)&&(test.union_bytes[1]==2)){
        printf("big endian\n");
    }
    else if ( (test.union_bytes[0]==2)&&(test.union_bytes[1]==1)){
        printf("little endian\n");
    }else{
        printf("unknown\n");
    }
}//判断字节序是大端还是小端

#include<netinet/in.h>
unsigned long int htonl( unsigned long int hostlong);
//host to network long
unsigned short int htons( unsigned short int hostshort);
//host to network short
unsigned long int ntohl( unsigned long int netlong);
//network to host long
unsigned short int ntohs( unsigned short int netshort);
//network to host short
//转换字节序

//socket地址结构体
#include<bits/socket.h>
struct sockaddr{
    sa_family_t sa_family;//地址簇
    char sa_data[4];
};
//还有另外几种更先进的，查书
//  协议簇·········地址簇·············描述··········地址簇值含义及其长度
//  PF_UNIX      AF_UNIX        UNIX本地域协议簇      文件的路径名，长度可达108字节
//  PF_INET      AF_INET        TCP/IPv4协议簇       16bit端口号和32bit IPv4地址，共6字节
//  PF_INET6     AF_INET6       TCP/IPv6协议簇       16bit端口号，32bit流标识，128bit IPv6地址,32bit范围ID,共26字节

//Linux定义新的结构体
struct sockaddr_storage{
    sa_family_t sa_family;//地址簇
    unsigned long int __ss_align;
    char __ss_padding[128-sizeof(__ss_align)];
};
//TCP/IP专用地址结构体，分别适用于IPv4和IPv6
struct sockadddr_in{
    sa_family_t sin_family;//地址簇
    u_int16_t sin_port;//端口号
    struct in_addr sin_addr;//IPv4地址结构体
};
struct in_addr{
    u_int32_t s_addr;//IPv4地址，以网络字节序表示
};

struct sockadddr_in6{
    sa_family_t sin6_family;
    u_int16_t sin6_port;
    u_int32_t sin6_flowinfo;//流信息
    struct in6_addr sin6_addr;
    u_int32_t sin6_scope_id;//scope ID
}; 
struct in6_addr{
    unsigned char sa_addr[16];
};

//IP地址转换函数
#include<arpa/inet.h>
in_addr_t inet_addr(const char* strptr);
//将用点分十进制字符串表示的IPv4地址转化为网络字节序整数表示的IPv4地址。失败返回INADDR_NONE
int inet_aton(const char* cp, struct in_addr* inp);
//效果一样，但将结果传给inp，失败返回0，成功返回1
char* inet_ntoa(struct in_addr in);
//将网络字节序整数表示的IPv4地址转化为用点分十进制字符串表示的IPv4地址。传出的地址为静态内存，不能更改

#include<sys/types.h>
#include<sys/socket.h>
//创建socket
int socket(int domain,int type,int protocol);
//domain调用哪个协议栈
//type 服务类型:分SOCK_STREAM流服务（TCP/IP）、SOCK_UGRAM数据报服务（UDP）
//protocol:选择一个具体的协议，一般为0（默认协议）
//调用成功返回socket文件描述符,失败则返回-1
int bind(int sockfd, const struct sockaddr* my_addr,socklen_t addrlen);
//将my_addr所指的socket地址分配给未命名的sockfd文件描述符,addrlen指出该地址的长度,成功返回0，失败返回-1
int listen(int sockfd,int backlog);
//创建一个监听队列以存放待处理的客户连接
//backlog表示内核监听队列的最大长度
int connect(int sockfd, const struct sockaddr* my_addr,socklen_t addrlen);
//成功则返回0，之后可以通过sockfd来与服务器通信
int accept(int sockfd, const struct sockaddr* my_addr,socklen_t addrlen);
//成功返回一个新的socket，失败返回-1
ssize_t recv(int sockfd,void* buf,size_t len,int flags);
//接收数据，成功返回数据大小（可能比len少，说明内部还没接受完或者数据就这么长）,flags有多个取值，一般为0
ssize_t send(int sockfd,const void* buf,size_t len,int flags);
//发送数据，成功返回实际写数据大小
int getsockname(int sockfd,struct sockaddr* address,socklen_t* address_len);
//获取sockfd对应的本端socket地址，成功返回0，失败为-1
int getpeername(int sockfd,struct sockaddr* address,socklen_t* address_len);
//获取sockfd对应的远端socket地址，成功返回0，失败为-1

#include<unistd.h>
int close(int fd);
//将引用计数减1，全部执行完才会关闭连接

#include<sys/socket.h>
int shutdown(int sockfd,int howto);
//终止连接

#include<sys/epoll.h>
int epoll_create(int size);
//size告诉系统创建一个多大的epoll，成功返回epoll的文件标识符
int epoll_ctl(int epfd,int op,int fd, struct epoll_event *event);
//epfd为epoll的文件标识符，fd为操作的文件标识符，event指定参数事件
//op有三种取值：EPOLL_CTL_ADD 往事件表中注册fd上的事件
//            EPOLL_CTL_MOD 修改fd上的注册事件
//            EPOLL_CTL_DEL 删除fd上的注册事件
//成功返回0，失败返回-1并设置errno
struct epoll_event{
    __uint32_t events;//epoll事件
    epoll_data_t data;//用户数据
};
typedef union epoll_data{
    void* ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data__t;
int epoll_wait(int epfd,struct epoll_event* events,.int maxevents,int timeout);
//执行时返回就绪的文件描述符的个数，失败返回-1并置errno