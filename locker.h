#pragma once
#include <exception>
#include <iostream>
#include <condition_variable>
#ifdef linux
#include<pthread.h>
#include<semaphore.h>
#elif _WIN32
#define  WIN32_LEAN_AND_MEAN  
#include <Windows.h>

typedef HANDLE sem_t;
typedef CRITICAL_SECTION pthread_mutex_t;
typedef CONDITION_VARIABLE pthread_cond_t;
#endif
class sem {
public:
    sem() {
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


    ~sem() {
#ifdef linux
        sem_destroy(&m_sem);
#elif _WIN32
        CloseHandle(m_sem);
#endif
    }

    bool wait() {
#ifdef _WIN32
        return WaitForSingleObject(m_sem, INFINITE) == WAIT_OBJECT_0;
#elif linux
        return sem_wait(&m_sem) == 0;
#endif
    }

    bool post() {
#ifdef _WIN32
        return ReleaseSemaphore(m_sem, 1, NULL) != 0;
#elif linux
        return sem_post(&m_sem) == 0;
#endif
    }
private:
    sem_t m_sem;
};


class locker {
    friend class cond;
public:
    locker() {
    }
    ~locker() {
    }
    bool lock() {
        mtx.lock();
        //std::unique_lock<std::mutex> lock(mtx);
    }
    bool unlock() {
        mtx.unlock();
    }
private:
    std::mutex mtx;
};

class cond {
public:
    cond() {
    }
    ~cond() {
    }

    bool wait() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock);
        return true;
    }

    bool signal() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.notify_one();
        return true;
    }
private:
    std::condition_variable cv;
    std::mutex mtx;
};
