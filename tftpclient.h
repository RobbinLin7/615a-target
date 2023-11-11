#ifndef _TFTP_CLIENT_H
#define _TFTP_CLIENT_H
#include <string>
#include <queue>
#include "tftp.h"
#include "locker.h"
#include "job.h"
class TftpClient;
#include "global.h"
class TftpClient: public Tftp{
public:
    TftpClient(){}
    void run();
    void sendFile(const std::string& fileName, const sockaddr_in* targetAddr);
    void receiveFile(const std::string& fileName, const sockaddr_in* targetAddr);
private:
};
#endif