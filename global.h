#pragma once
#include "tftpclient.h"
#include<queue>
#include<map>
extern size_t max_tftpClient_cnt;
//extern TftpClient tftpClients[max_tftpClient_cnt];  //tftpClient��
extern cond gotNewJob;
extern std::queue<Job> jobs;                       //�������
extern std::map<sockaddr_in, cond> conds;
