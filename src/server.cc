
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


void send_msg(int client,const int& model,const std::string& file_name=""){
    std::string s="";
    const std::string server_string="Server: myhttpsever/1.0.0\r\nContent-type: text/html;\r\n";
    switch(model){
        case 404:
            s+="HTTP/1.1 404 NOT FOUND\r\n";
            s+=server_string;
            s+="Content-Length: 149\r\n";
            s+="\r\n";//2
            s+="<html>\r\n";//8
            s+="<head>\r\n";//8 18
            s+="<title>404 NOT FOUND</title>\r\n";//30 48
            s+="</head>\r\n";//9
            s+="<body>\r\n";//8 65
            s+="<p>The file you requested does not exist.Please check your url</p>\r\n";//68 133
            s+="</body>\r\n";//9
            s+="</html>\r\n";//9 151-2
        break;
        case 505:
            s+="HTTP/1.1 505 HTTP VERSION NOT SUPPORTED\r\n";
            s+=server_string;
            s+="Content-Length: 139\r\n";
            s+="\r\n";//2
            s+="<html>\r\n";//8
            s+="<head>\r\n";//8 18
            s+="<title>505 HTTP VERSION NOT SUPPORTED</title>\r\n";//47 65
            s+="</head>\r\n";//9 74
            s+="<body>\r\n";//8 82
            s+="<p>The version of server is too low</p>\r\n";//41 123
            s+="</body>\r\n";//9 
            s+="</html>\r\n";//9 141-2
        break;
        case 200:
            s+="HTTP/1.1 200 OK\r\n";
            s+=server_string;
            s+="Content-Length: ";
            if(file_name!=""){
                FILE *fp=fopen(file_name.c_str(),"r");
                if(!fp) return;
                fseek(fp,0L,SEEK_END);
                int size=ftell(fp);
                fclose(fp);
                s+=std::to_string(size);
            }
            s+="\r\n";
            //s+="Content-Language:zh-cn\r\n";
            s+="\r\n";//4
        break;
        case 400:
            s+="HTTP/1.1 400 BAD REQUEST\r\n";
            s+=server_string;
            s+="Content-Length: 150\r\n";
            s+="\r\n";//2
            s+="<html>\r\n";//8
            s+="<head>\r\n";//8 18
            s+="<title>400 BAD REQUEST</title>\r\n";//32 50
            s+="</head>\r\n";//9 
            s+="<body>\r\n";//8 67
            s+="<p>The request can't be understood. Please check the request.</p>\r\n";//67 134
            s+="</body>\r\n";//9 
            s+="</html>\r\n";//9 152-2
        break;
        default:
        break;
    }
    if(model==200){
        FILE *fp=fopen(file_name.c_str(),"r");
        if(!fp) return;
        
        char c[1024];
        fgets(c, sizeof(c), fp);
        //循环读
        while (!feof(fp))
        {
            s += c;
            fgets(c, sizeof(c), fp);
        }
        s+= c;
        s+="\r\n";
        

        fclose(fp);
    }
    for(int i=0;i<s.length();i+=1024){
        std::string s_sub = s.substr(i,1024);
        send(client,s_sub.c_str(), s_sub.length(), 0);
    }
    std::cout<<s;
    // if(model==200){
    //     FILE *fp=fopen(file_name.c_str(),"r");
    //     if(!fp) return;
        
    //     char c[1024];
    //     fgets(c, sizeof(c), fp);
    //     //循环读
    //     while (!feof(fp))
    //     {
    //         send(client, c, sizeof(c), 0);
    //         std::string s1 = c;
    //         std::cout<<s1;
    //         fgets(c, sizeof(c), fp);
    //     }
    //     std::string s1 = c;
    //     s1+="\r\n";
    //     std::cout<<s1;
    //     send(client, s1.c_str(), sizeof(c), 0);
        

    //     fclose(fp);
    // }
}
int file_is_exist(const std::string &s)
{
    if (s.empty())
    {
        return -1;
    }
    else
    {
        return access(s.c_str(), 04);
        //这里也可以用s.data()，但这里是const char* ，如果要变成char*要强制类型转换
    }
}
void* server_deal_request(void* arg){
    int client = (intptr_t)arg;
    std::string s;
    char c[1024];
    const int c_length = 1024;
    int msg_length;
    int sock_fd = (int)(intptr_t)arg;
    msg_length = recv(sock_fd,(void*)c,c_length,0);
    
    c[msg_length]='\0';
    s+=c;
    while(msg_length>=c_length){
        msg_length = recv(sock_fd,(void*)c,c_length,0);
        
        if(msg_length==-1){
            perror("消息处理");
            close(client);
            return nullptr;
        }
        s+=c;
    }

    std::cout<<s<<std::endl;
    
    int index = 0;
    int index_prev;
    while(s[index]!=' '&&index<s.length()&&s[index]!='\r'&&s[index]!='\n'){
        index++;
    }
    if(index==s.length()){
        send_msg(client,400);
        close(client);
        return nullptr;
    }
    std::string model= s.substr(0,index);

    //std::cout<<"model:"<<model<<std::endl;
    while((s[index]==' '||s[index]=='\r'||s[index]=='\n')&&index<s.length()){
        index++;
    }
    index_prev = index;
    
    if(strcasecmp(model.c_str(),"GET")&&strcasecmp(model.c_str(),"POST")){
        send_msg(client,400);
        close(client);
        return nullptr;
    }

    while(s[index]!=' '&&index<s.length()&&s[index]!='\r'&&s[index]!='\n'){
        index++;
    }

    std::string file_name = s.substr(index_prev,index-index_prev);

    //std::cout<<"file_name:"<<file_name<<std::endl;

    if(file_name[0]=='/'){
        if(file_name.length()==1){
            file_name+="index.html";
        }else if(file_name.length()<=5){
            file_name+=".html";
        }else{
            std::string s1 = file_name.substr(file_name.length()-5,5);
            if(strcasecmp(s1.c_str(),".html")){
                file_name+=".html";
            }
        }
    }else{
        send_msg(client,400);
        close(client);
        return nullptr;
    }

    std::cout<<"file_name:"<<file_name<<std::endl;

    while((s[index]==' '||s[index]=='\r'||s[index]=='\n')&&index<s.length()){
        index++;
    }
    index_prev = index;

    while(s[index]!=' '&&index<s.length()&&s[index]!='\r'&&s[index]!='\n'){
        index++;
    }

    std::string version = s.substr(index_prev,index-index_prev);

    //std::cout<<"version:"<<version<<std::endl;

    if(strcasecmp(version.c_str(),"HTTP/1.1")){
        send_msg(client,505);
        close(client);
        return nullptr;
    }

    file_name = "htdocs" + file_name;

    int flag = file_is_exist(file_name);
    if(flag==-1){
        send_msg(client,404);
        close(client);
        return nullptr;
    }else if(flag==0){
        send_msg(client,200,file_name);
    }

    close(client);
    return nullptr;
}

int setnonblocking(const int& fd){
    int old_option = fcntl(fd,F_GETFL);
    if(old_option==-1){
        return old_option;
    }
    int new_option = old_option|O_NONBLOCK;
    new_option = fcntl(fd,F_SETFL,new_option);
    return new_option;
}//将fd对应的文件描述符设定为不阻塞

int addfd(const int& sockfd,const int& epoll_fd){
    epoll_event event;
    event.data.fd = sockfd;
    event.events = EPOLLIN;
    //event.events|=EPOLLET;//设模式为RT（边沿触发）
    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,sockfd,&event)==-1){
        return -1;
    }
    return setnonblocking(sockfd);
}//将文件加进epoll的内核事件表，模式设为LT(水平触发)模式,一旦就绪立刻处理

short start_server(int *sockpd,uint16_t *port,const int &epoll_fd){
    struct sockaddr_in sock;//IPv4专用地址结构体
    int flag;//执行函数后状态
    //初始化
    memset(&sock,0,sizeof(sock));
    sock.sin_family = AF_INET;
    sock.sin_port=htons(*port);
    sock.sin_addr.s_addr = htonl(INADDR_ANY);

    //创建socket
    *sockpd = socket(PF_INET,SOCK_STREAM,0);
    if(*sockpd == -1){
        perror("socket创建");
        return 0;
    }

    //绑定socket
    flag = bind(*sockpd,(struct sockaddr*)&sock,sizeof(sock));
    if(flag==-1){
        perror("绑定socket");
        return 0;
    }

    //将socket转为监听口
    flag = listen(*sockpd,4);
    if(flag == -1){
        perror("转换为监听口");
        return 0;
    }

    flag = addfd(*sockpd,epoll_fd);
    if(flag < 0){
        perror("加入epoll内核事件表");
        return -1;
    }

    return 1;
}
#define epoll_size 10
#define MAX_EPOLL_NUM 1024
short server(uint16_t *port){
    struct sockaddr_in sock;//接受的socket地址
    socklen_t sock_len;//接受的socket地址长度
    pthread_t new_thread;//线程id，每次线程创建时将线程id赋给这个变量
    int ac_fd;//接收的socket文件标识符
    int sockpd;//监听的socket地址
    int epoll_fd;//epoll的标识符
    epoll_event events[MAX_EPOLL_NUM];//epoll事件的数组
    
    epoll_fd = epoll_create(epoll_size);
    if(epoll_fd<0){
        perror("epoll创建");
        return -1;
    }
    

    short flag = start_server(&sockpd,port,epoll_fd);
    if(!flag){
        return flag;
    }
    printf("running on port %d\n", *port);

    while(1){
        int epoll_num = epoll_wait(epoll_fd,events,MAX_EPOLL_NUM,-1);
        if(epoll_num<0){
            perror("epoll创建");
            return -1;
        }
        
        std::cout<<"内核事件数："<<epoll_num<<std::endl;

        for(int i = 0;i<epoll_num;i++){
            int sock_fd = events[i].data.fd;

            if(sock_fd==sockpd){
                std::cout<<"接收连接"<<std::endl;
                ac_fd = accept(sockpd,(struct sockaddr*)(&sock),&sock_len);
                //接收信息，得到接受的socket地址

                if(ac_fd == -1){
                    perror("消息接收");
                    return 0;
                }

                flag = addfd(ac_fd,epoll_fd);
                if(flag < 0){
                    perror("加入epoll内核事件表");
                    return -1;
                }
            }else{
                if(events[i].events& EPOLLIN){
                    std::cout<<"第"<<i<<"个socket的标识符"<<sock_fd<<std::endl;
                    if(pthread_create(&new_thread, NULL, server_deal_request, (void *)(intptr_t)sock_fd)){
                        perror("pthread_create");
                        return -1;
                    }
                }else{
                    std::cout<<"处理epoll内核事件出错:事件不可读"<<std::endl;
                }
            }
        }
        

        //if (pthread_create(&new_thread, NULL, server_deal_request, (void *)(intptr_t)ac_fd))

        //server_deal_request((void*)(intptr_t)ac_fd);
        //std::thread t(server_deal_request,(void *)(intptr_t)ac_fd);
        //t.detach();
    }
    close(sockpd);
    return 1;
}

int main(){
    uint16_t port;
    printf("write the port:");
    std::cin>>port;
    short flag = server(&port);
    while(!flag){
        std::cout<<"the port has been used.please write the next port:";
        std::cin>>port;
        flag = server(&port);
    }
    if(flag == -1){
        std::cout<<"it happens some errors in epoll"<<std::endl;
    }
}