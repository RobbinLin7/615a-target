#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "worker.h"
#include "job.h"
#include "locker.h"
#include<queue>
#include<map>
extern const size_t max_worker_cnt;
extern Worker workers[];  //tftpClient��
extern cond gotNewJob;
extern std::queue<Job> jobs;                       //�������
extern std::map<sockaddr_in, cond*, bool (*)(const sockaddr_in&, const sockaddr_in&)> conds;
#endif 
// _GLOBAL_H_
