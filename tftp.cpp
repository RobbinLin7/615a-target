#include "tftp.h"
void tftp::makeTftpAckPacket(std::string& ack, ack_t ackNo){
    //1.opcode
    ack.push_back('\0');
    ack.push_back(static_cast<char>(4));
    //2.ackNo
    ack.push_back(static_cast<char>(ackNo >> 8));
    ack.push_back(static_cast<char>(ackNo));
}

void tftp::sendFile(std::string &fileName, const sockaddr_in* targetAddr){
    udp_socket socket;
    block_t blockNo = 0;
    char buf[max_tftp_data_len];
    std::ifstream fin;
    try{
        fin.open(fileName);
        size_t count = 0;
        do{
            fin.read(buf, max_tftp_data_len);
            count = fin.gcount();
            std::string data(buf, count);
            std::string tftpDataPacket;
            makeTftpDataPacket(tftpDataPacket, data, blockNo);
            //这里使用static_cast会报错
            socket.sendto(count, 0, reinterpret_cast<const sockaddr*>(targetAddr), sizeof(sockaddr));
            socket.recvfrom(0);
        }while(fin && count == max_tftp_data_len);
        fin.read(buf, max_tftp_data_len);

    }catch(std::exception e){

    }
}

void tftp::receiveFile(std::string &fileName){

}

ack_t tftp::parseTftpAckPacket(std::string &ack)
{   
    ack_t ackNo;
    if(ack[0] != '\0' || ack[1] != '\4'){
        throw std::exception();
    }
    return (ack[2] << 8) + ack[3];
}

void tftp::makeTftpDataPacket(std::string &tftpDataPacket, std::string &data, block_t blockNo){
    //1.opcode
    tftpDataPacket.push_back('\0');
    tftpDataPacket.push_back('\3');
    //2.blockNo
    tftpDataPacket.push_back(static_cast<char>(blockNo >> 8));
    tftpDataPacket.push_back(static_cast<char>(blockNo));
    //3.data
    tftpDataPacket.append(data);
}

void tftp::makeTftpReadReuqestPacket(std::string &tftpReadRequestPacket, std::string fileName, std::string mode = "OCTET")
{
    //1.opcode
    tftpReadRequestPacket.push_back('\0');
    tftpReadRequestPacket.push_back('\1');
    //2.fileName
    tftpReadRequestPacket.append(fileName.c_str());
    //3.mode
    tftpReadRequestPacket.append(mode.c_str());
}

void tftp::makeTftpWriteRequestPacket(std::string& tftpWriteRequestPacket, std::string fileName, std::string mode = "OCTET"){
    //1.opcode
    tftpWriteRequestPacket.push_back('\0');
    tftpWriteRequestPacket.push_back('\2');
    //2.fileName
    tftpWriteRequestPacket.append(fileName.c_str());
    //3.mode
    tftpWriteRequestPacket.append(mode.c_str());
}

