#ifndef _TFTP_H_
#define _TFTP_H_
#include <stdint.h>
#include <string>
#include <fstream>
#include "socket.h"
class Worker;
typedef uint16_t block_t;
const size_t max_tftp_data_len = 512;
class Tftp{
public:
    friend class Worker;
    enum tftpType{tftpReadRequest, tftpWriteRequest, tftpData, tftpAck, tftpError, undefined};
    static tftpType parseTftpPacket(std::string& tftpPacket, std::string& fileName, std::string& mode, block_t& num);
    static void makeTftpDataPacket(std::string& tftpDataPacket, std::string& data, block_t blockNo);
    static void makeTftpReadRequestPacket(std::string& tftpReadRequestPacket, std::string fileName, std::string mode = "OCTET");
    static void makeTftpWriteRequestPacket(std::string& tftpWriteRequestPacket, std::string fileName, std::string mode = "OCTET");
    static void makeTftpAckPacket(std::string& ack, block_t ackNo);
    void sendFileAsClient(const std::string& fileName, const sockaddr_in* targetAddr);
    void sendFileAsServer(const std::string& fileName, const sockaddr_in* targetAddr);
    void receiveFileAsClient(const std::string& fileName, const sockaddr_in* targetAddr);
    void receiveFileAsServer(const std::string& fileName, const sockaddr_in* targetAddr);
protected:
    udp_socket socket;
    std::string tftpPacket;
    std::string fileName, mode;
    std::string targetTftpPacket;
    block_t num;
};
#endif