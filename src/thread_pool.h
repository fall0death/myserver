#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<bits/socket.h>
#include<thread>
#include<stdlib.h>
#include<errno.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/un.h>
#include<ctype.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#include<iostream>
#include<assert.h>
#include<fcntl.h>
#include<pthread.h>
#include<queue>

#include"locker.h"

template<typename T>
class thread_pool{
public:
    
    ~thread_pool(){
        delete [] thread;
        thread = NULL;
        m_stop = true;
    }
    thread_pool(const int p_num = 10,const int Max_request_num = 1000);

    static void* worker(void* arg);
    void run();
    bool append_request(T *request);
private:
    thread_pool(thread_pool& p);
    
    int thread_num;//线程池中进程的数量
    int max_request_num;
    
    std::queue<T*> requests;

    sema sem;
    mutex_lock queue_lock;
    
    bool m_stop;

    pthread_t* thread;

};

template<typename T>
thread_pool<T>::thread_pool(const int p_num,const int Max_request_num){
    if(p_num<0||Max_request_num<0){
        throw std::exception();
    }
    thread_num = p_num;
    max_request_num = Max_request_num;
    thread = new pthread_t[thread_num];
    for(int i=0;i<thread_num;i++){
        if(pthread_create(thread+i,NULL,worker,this)){
            delete []thread;
            thread = NULL;
            throw std::exception();
        }
        if(pthread_detach(thread[i])){
            delete []thread;
            thread = NULL;
            throw std::exception();
        }
    }
}

template<typename T>
bool thread_pool<T>::append_request(T *request){
    queue_lock.lock();
    if(requests.size()>max_request_num){
        queue_lock.unlock();
        return false;
    }
    requests.push(request);
    queue_lock.unlock();
    sem.post();
    return true;
}
template<typename T>
void* thread_pool<T>::worker(void* arg){
    thread_pool* pool = (thread_pool*) arg;
    pool->run();
    return pool; 
}

template<typename T>
void thread_pool<T>::run(){
    while(!m_stop){
        sem.wait();
        queue_lock.lock();
        if(requests.empty()){
            queue_lock.unlock();
            continue;
        }
        T* request = requests.front();
        requests.pop();
        queue_lock.unlock();
        if(request!=NULL){
            request->process();
        }
        delete request;
    }
}

#endif