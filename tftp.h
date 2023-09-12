#ifndef _TFTP_H_
#define _TFTP_H_
#include <string>
#include "socket.h"
#include <fstream>
typedef uint16_t ack_t;
typedef uint16_t block_t;
const size_t max_tftp_data_len = 512;
class tftp{
public:
    static void makeTftpDataPacket(std::string& tftpDataPacket, std::string& data, block_t blockNo);
    static void makeTftpReadReuqestPacket(std::string& tftpReadRequestPacket, std::string fileName, std::string mode = "OCTET");
    static void makeTftpWriteRequestPacket(std::string& tftpWriteRequestPacket, std::string fileName, std::string mode = "OCTET");
    static void makeTftpAck(std::string& ack, ack_t ackNo);
    static void handleTftpReadReuqest();
    static void handleTftpWriteRequest();
    static void sendFile(std::string& fileName, const sockaddr_in* targetAddr);
    static void receiveFile(std::string& fileName); 
};
#endif