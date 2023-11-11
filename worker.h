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
    Worker():id(++i){}
    void run();
};
#endif