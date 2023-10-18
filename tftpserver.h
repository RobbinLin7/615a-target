#ifndef _TFTP_SERVER_H_
#define _TFTP_SERVER_H_
#include <string>
#include "socket.h"
#include "tftp.h"
#include "global.h"
#include <regex>
const uint16_t tftp_server_port = 69;
class TftpServer: public Tftp{
public:
    void startTftpServer();
    static TftpServer* getTFTPServerInstance();
    static void handleTftpReadReuqest();
    static void handleTftpWriteRequest();
    void sendFile(const std::string& fileName, const sockaddr_in* targetAddr) override;
    void receiveFile(const std::string& fileName, const sockaddr_in* targetAddr) override;
private:
    TftpServer();
    static TftpServer* tftpServerInstance;
};
#endif