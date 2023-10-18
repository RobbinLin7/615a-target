#ifndef _ARINC615A_H_
#define _ARINC615A_H_
//#include<sys/socket.h>
//#include<netinet/in.h>
//#include<arpa/inet.h>
#include "socket.h"
#include "locker.h"
#include "job.h"
#include <queue>
extern std::queue<Job> jobs;

struct information_para {
    sockaddr_in& addr;
    cond& m_cond; 
    Arinc615a::informationStatus& status;
};

class Arinc615a{
public:
    Arinc615a() = delete;
    enum informationStatus{SEND_LCI, SEND_LCL};
    enum uploadStatus{};
    enum mediaDownloadStatus{};
    enum operatorDownloadStatus{};
    
    static void information(void* arg);
    
    bool upload()const;
    bool mediaDownload()const;
    bool operatorDownload()const;
};
#endif