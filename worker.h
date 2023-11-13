#ifndef _WORKER_H_
#define _WORKER_H_
#include "tftp.h"
class Worker
{
private:
    /* data */
    Tftp tftp;
public:
    static int i;
    int id;
    Worker():id(++i){
        tftp.socket.setRecvTimeout(3, 0);
    }
    void run();
};
#endif