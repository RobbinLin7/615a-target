#ifndef _THREAD_H_
#define _THREAD_H_
#include<iostream>
#ifdef UNIX
#include<pthread.h>
typedef pthread_t thread_t;
typedef void*(*MyThreadFunction)(void*); 
#else
#include<Windows.h>
typedef DWORD thread_t;
typedef DWORD (*MyThreadFunction)(LPVOID);
#endif
class thread
{

private:
    /* data */
    thread_t threadId;
    void* (*func)(void* args);
#ifdef _WIN32
    HANDLE hThread;
#endif // _WIN32

public:
#ifdef UNIX
    thread(void* (*func)(void* args)) {
        this->func = func;
        pthread_create(&m_pthread, NULL, func, NULL);
}
    ~thread() {
        pthread_join(m_pthread, NULL);
        std::cout << "in ~thread" << std::endl;
    }
#else
    thread(MyThreadFunction func) {
        hThread = CreateThread(NULL, 0, func, 0, 0, &threadId);
        if (hThread == NULL) {
            throw std::exception();
        }
        ResumeThread(hThread);
    }
    HANDLE getHandle() {
        return hThread;
    }
    ~thread() {
        CloseHandle(hThread);
    }
#endif

};
#endif
