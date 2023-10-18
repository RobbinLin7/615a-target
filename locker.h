#ifndef LOCKER_H
#define LOCKER_H
#include<exception>
#ifdef linux
#include<pthread.h>
#include<semaphore.h>
#elif _WIN32
#define  WIN32_LEAN_AND_MEAN  
#include <Windows.h>
#include <condition_variable>
typedef HANDLE sem_t;
typedef CRITICAL_SECTION pthread_mutex_t;
typedef CONDITION_VARIABLE pthread_cond_t;
#endif
class sem{
    public:
    sem(){
#ifdef _WIN32
        m_sem = CreateSemaphore(NULL, 0, 1, NULL);
        if (m_sem == NULL) {
            throw std::exception();
        }
#elif linux
        if (sem_init(&m_sem, 0, 0) != 0) {
            throw std::exception();
        }
#endif
    }


    ~sem(){
#ifdef linux
        sem_destroy(&m_sem);
#elif _WIN32
        CloseHandle(m_sem);
#endif
    }

    bool wait(){
#ifdef _WIN32
        return WaitForSingleObject(m_sem, INFINITE) == WAIT_OBJECT_0;
#elif linux
        return sem_wait(&m_sem) == 0;
#endif
    }

    bool post(){
#ifdef _WIN32
        return ReleaseSemaphore(m_sem, 1, NULL) != 0;
#elif linux
        return sem_post(&m_sem) == 0;
#endif
    }
private:
    sem_t m_sem;
};


class locker{
    friend class cond;
    public:
    locker(){
#ifdef _WIN32
        //m_mutex = CreateMutex(NULL, FALSE, NULL);
        //if (m_mutex == NULL) {
            //throw std::exception();
        //}
        InitializeCriticalSection(&m_mutex);
#elif linux
        if (pthread_mutex_init(&m_mutex, NULL) != 0) {
            throw std::exception();
        }
#endif
    }
    ~locker(){
#ifdef _WIN32
        //CloseHandle(m_mutex);
        DeleteCriticalSection(&m_mutex);
#elif linux
        pthread_mutex_destroy(&m_mutex);
#endif
        
    }
    bool lock(){
#ifdef _WIN32
        EnterCriticalSection(&m_mutex);
        return true;
#elif linux
        return pthread_mutex_lock(&m_mutex) == 0;
#endif
    }
    bool unlock(){
#ifdef _WIN32
        //return ReleaseMutex(m_mutex) != 0;
        LeaveCriticalSection(&m_mutex);
        return true;
#elif linux
        return pthread_mutex_unlock(&m_mutex) == 0;
#endif
    }
    private:
    pthread_mutex_t m_mutex;
};

class cond{
    public:
    cond(){
#ifdef _WIN32

#elif linux
        if(pthread_mutex_init(&m_mutex, NULL) != 0){
            throw std::exception();
        }
        if(pthread_cond_init(&m_cond, NULL) != 0){
            pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
#endif
    }
    ~cond(){

#ifdef _WIN32

#elif linux
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
#endif
    }

    bool wait(){
#ifdef _WIN32
        m_mutex.lock();
        SleepConditionVariableCS(&m_cond, &m_mutex.m_mutex, INFINITE);
        m_mutex.unlock();
        return true;
#elif linux
        int ret = 0;
        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, &m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
#endif
    }

    bool signal(){
#ifdef _WIN32
        WakeConditionVariable(&m_cond);
        return true;
#elif linux
        return pthread_cond_signal(&m_cond) == 0;
#endif
    }

    private:
        //bool flag = false;
        locker m_mutex;
        pthread_cond_t m_cond;
#endif
};



