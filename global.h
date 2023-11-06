#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "tftpclient.h"
#include<queue>
#include<map>
extern const size_t max_tftpClient_cnt;
extern TftpClient tftpClients[];  //tftpClient池
extern cond gotNewJob;
extern std::queue<Job> jobs;                       //任务队列
extern std::map<sockaddr_in, cond*, bool (*)(const sockaddr_in&, const sockaddr_in&)> conds;
#endif 
// _GLOBAL_H_
