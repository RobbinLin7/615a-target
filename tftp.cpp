#include "tftp.h"

Tftp::tftpType Tftp::parseTftpPacket(std::string &tftpPacket, std::string &fileName, std::string &mode, block_t &num){
    tftpType type;
    switch (tftpPacket[1]){
    case '\1':
        /* code */
        type = tftpReadRequest;
        fileName = tftpPacket.substr(2, tftpPacket.find_first_of('\0', 2) - 2);
        //std::cout << "filename " << fileName << std::endl;
        mode = tftpPacket.substr(2 + fileName.size() + 1, tftpPacket.size() - 4 - fileName.size());
        //std::cout << "mode " << mode << std::endl;
        break;
    case '\2':
        type = tftpWriteRequest;
        fileName = tftpPacket.substr(2);
        mode = tftpPacket.substr(2 + fileName.size() + 1);
        break;
    case '\3':
        type = tftpData;
        num = (tftpPacket[2] << 8) + tftpPacket[3];
        break; 
    case '\4':
        type = tftpAck;
        num = (tftpPacket[2] << 8) + tftpPacket[3];
        break; 
    case '\5':
        type = tftpError;
        break;
    default:
        type = undefined;
        break;
    }
    return type;
}

void Tftp::makeTftpDataPacket(std::string &tftpDataPacket, std::string &data, block_t blockNo)
{
    //1.opcode
    tftpDataPacket.push_back('\0');
    tftpDataPacket.push_back('\3');
    //2.blockNo
    tftpDataPacket.push_back(static_cast<char>(blockNo >> 8));
    tftpDataPacket.push_back(static_cast<char>(blockNo));
    //3.data
    tftpDataPacket.append(data);
}

void Tftp::makeTftpReadReuqestPacket(std::string &tftpReadRequestPacket, std::string fileName, std::string mode)
{
    //1.opcode
    tftpReadRequestPacket.push_back('\0');
    tftpReadRequestPacket.push_back('\1');
    //2.fileName
    tftpReadRequestPacket.append(fileName.c_str());
    //3.mode
    tftpReadRequestPacket.append(mode.c_str());
}

void Tftp::makeTftpWriteRequestPacket(std::string &tftpWriteRequestPacket, std::string fileName, std::string mode)
{
    //1.opcode
    tftpWriteRequestPacket.push_back('\0');
    tftpWriteRequestPacket.push_back('\2');
    //2.fileName
    tftpWriteRequestPacket.append(fileName.c_str());
    tftpWriteRequestPacket.push_back('\0');
    //3.mode
    tftpWriteRequestPacket.append(mode.c_str());
    //tftpWriteRequestPacket.push_back('\0');
}

void Tftp::makeTftpAckPacket(std::string &ack, block_t ackNo)
{
    //1.opcode
    ack.push_back('\0');
    ack.push_back(static_cast<char>(4));
    //2.ackNo
    ack.push_back(static_cast<char>(ackNo >> 8));
    ack.push_back(static_cast<char>(ackNo));
}
