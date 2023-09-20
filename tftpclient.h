#ifndef _TFTP_CLIENT_H
#define _TFTP_CLIENT_H
#include <string>
#include "tftp.h"

class TftpClient: public Tftp{
public:
    TftpClient(){}
    void sendFile(const std::string& fileName, const sockaddr_in* targetAddr) override;
    void receiveFile(const std::string& fileName, const sockaddr_in* targetAddr) override;
private:

};
#endif