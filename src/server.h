#ifndef SERVER_H
#define SERVER_H

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<bits/socket.h>
#include<thread>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/un.h>
#include<ctype.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#include<iostream>
#include<assert.h>
#include<fcntl.h>
#include<pthread.h>

#include "thread_pool.h"

int setnonblocking(const int& fd);//将fd对应的文件描述符设定为不阻塞
int addfd(const int& sockfd,const int& epoll_fd);//将文件加进epoll的内核事件表，模式设为LT(水平触发)模式,一旦就绪立刻处理
void removefd(const int& epoll,const int& fd);//删除fd在epoll中的所有注册事件并关闭fd的文件


void send_msg(int client,const int& model,const std::string& file_name="");
int file_is_exist(const std::string &s);
void* server_deal_request(void* arg);


short start_server(int *sockpd,uint16_t *port,const int &epoll_fd);
short server(uint16_t *port);



#endif