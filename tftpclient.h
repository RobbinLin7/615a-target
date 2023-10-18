#ifndef _TFTP_CLIENT_H
#define _TFTP_CLIENT_H
#include <string>
#include <queue>
#include "tftp.h"
#include "locker.h"
#include "job.h"
extern std::queue<Job> jobs;
extern cond gotNewJob;
class TftpClient: public Tftp{
public:
    TftpClient(){}
    void run();
    void sendFile(const std::string& fileName, const sockaddr_in* targetAddr) override;
    void receiveFile(const std::string& fileName, const sockaddr_in* targetAddr) override;
private:
};
#endif