#ifndef _THREAD_H_
#define _THREAD_H_

#define UNIX
#ifdef UNIX
#include<pthread.h>
#include<iostream>
class thread
{

private:
    /* data */
    pthread_t m_pthread;
    void* (*func)(void* args);
public:
    thread(void* (*func)(void* args)){
        this->func = func;
        pthread_create(&m_pthread, NULL, func, NULL);
    }
    ~thread(){
        pthread_join(m_pthread, NULL);
        std::cout << "in ~thread" << std::endl;
    }
};
#endif


#ifdef WINDOWS
//todo
#endif


#endif