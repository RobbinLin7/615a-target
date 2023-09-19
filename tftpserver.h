#ifndef _TFTP_SERVER_H_
#define _TFTP_SERVER_H_
#include <string>
#include "socket.h"
#include <fstream>
#include <regex>
typedef uint16_t block_t;
const size_t max_tftp_data_len = 512;
const uint16_t tftp_server_port = 69;
class TftpServer{
public:
    void startTftpServer();
    static TftpServer* getTFTPServerInstance();
    enum tftpType{tftpReadRequest, tftpWriteRequest, tftpData, tftpAck, tftpError, undefined};
    static void makeTftpDataPacket(std::string& tftpDataPacket, std::string& data, block_t blockNo);
    static void makeTftpReadReuqestPacket(std::string& tftpReadRequestPacket, std::string fileName, std::string mode = "OCTET");
    static void makeTftpWriteRequestPacket(std::string& tftpWriteRequestPacket, std::string fileName, std::string mode = "OCTET");
    static void makeTftpAckPacket(std::string& ack, block_t ackNo);
    static void handleTftpReadReuqest();
    static void handleTftpWriteRequest();
    void sendFile(std::string& fileName, const sockaddr_in* targetAddr);
    void receiveFile(std::string& fileName);
    static tftpType parseTftpPacket(std::string& tftpPacket, std::string& fileName, std::string& mode, block_t& num); 

private:
    TftpServer();
    udp_socket socket;
    std::string tftpPacket;
    std::string fileName, mode;
    block_t num;
};
#endif