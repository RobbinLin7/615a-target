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
        fileName = tftpPacket.substr(2, tftpPacket.find_first_of('\0', 2) - 2);
        mode = tftpPacket.substr(2 + fileName.size() + 1, tftpPacket.size() - 4 - fileName.size());
        break;
    case '\3':{
        type = tftpData;
        unsigned short high = (unsigned char)tftpPacket.at(2);
        unsigned short low = (unsigned char)tftpPacket.at(3);
        num = (high << 8) + low;
        break;
    } 
    case '\4':{
        type = tftpAck;
        unsigned short high = (unsigned char)tftpPacket.at(2);
        unsigned short low = (unsigned char)tftpPacket.at(3);
        num = (high << 8) + low;
        //num = ((unsigned char)tftpPacket[2] << 8) + tftpPacket[3];
        break;
    }
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

void Tftp::makeTftpReadRequestPacket(std::string &tftpReadRequestPacket, std::string fileName, std::string mode)
{
    tftpReadRequestPacket.clear();
    //1.opcode
    tftpReadRequestPacket.push_back('\0');
    tftpReadRequestPacket.push_back('\1');
    //2.fileName
    tftpReadRequestPacket.append(fileName.c_str());
    tftpReadRequestPacket.push_back('\0');
    //3.mode
    tftpReadRequestPacket.append(mode.c_str());
    tftpReadRequestPacket.push_back('\0');
}

void Tftp::makeTftpWriteRequestPacket(std::string &tftpWriteRequestPacket, std::string fileName, std::string mode)
{   
    tftpWriteRequestPacket.clear();
    //1.opcode
    tftpWriteRequestPacket.push_back('\0');
    tftpWriteRequestPacket.push_back('\2');
    //2.fileName
    tftpWriteRequestPacket.append(fileName.c_str());
    tftpWriteRequestPacket.push_back('\0');
    //3.mode
    tftpWriteRequestPacket.append(mode.c_str());
    tftpWriteRequestPacket.push_back('\0');
}

void Tftp::makeTftpAckPacket(std::string &ack, block_t ackNo)
{   
    ack.clear();
    //1.opcode
    ack.push_back('\0');
    ack.push_back(static_cast<char>(4));
    //2.ackNo
    ack.push_back(static_cast<char>(ackNo >> 8));
    ack.push_back(static_cast<char>(ackNo));
}



void Tftp::sendFileAsServer(const std::string& fileName, const sockaddr_in* targetAddr){
    std::cout << "send file as server" << std::endl;
    block_t blockNo = 1;
    char buf[max_tftp_data_len];
    std::ifstream fin;
    try{
        fin.open("./tmp/" + fileName);
        std::cout << "fileName = " << fileName << std::endl;
        size_t count = 1;
        do{ 
            std::cout << "send file as server1" << std::endl;
            fin.read(buf, max_tftp_data_len);
            count = fin.gcount();
            std::cout << "count = " << count << std::endl;
            std::string data(buf, count);
            std::string tftpDataPacket;
            makeTftpDataPacket(tftpDataPacket, data, blockNo);
            //这里使用static_cast会报错
            socket.sendto(tftpDataPacket.c_str(), count + 4, 0, reinterpret_cast<const sockaddr*>(targetAddr), sizeof(sockaddr));
            
            for(;;){
                std::cout << "send file as server2" << std::endl;
                socket.recvfrom(0);
                tftpPacket = socket.getBuff();
                tftpType type = parseTftpPacket(tftpPacket, this->fileName, mode, num);
                if(type != tftpAck){
                    std::cout << "not ack type" << std::endl;
                }
                else if(num != blockNo){
                    socket.sendto(tftpDataPacket.c_str(), count + 4, 0, reinterpret_cast<const sockaddr*>(targetAddr), sizeof(sockaddr));
                }
                else break;
            }
            ++blockNo;
        }while(fin && count == max_tftp_data_len);
    }catch(std::exception e){

    }
    std::cout << "send file finished" << std::endl;
}

void Tftp::sendFileAsClient(const std::string& fileName, const sockaddr_in* targetAddr){
    std::cout << "send file "<< fileName  << " as client" << std::endl;
    tftpPacket.clear();
    makeTftpWriteRequestPacket(tftpPacket, fileName);
    std::string targetTftpPacket;
    std::cout << "send bytes" << socket.sendto(tftpPacket.c_str(), tftpPacket.size(), 0, (sockaddr*)targetAddr, sizeof(sockaddr)) << std::endl;
    for(;;){
        socket.recvfrom(0);
        targetTftpPacket = socket.getBuff();
        tftpType type = parseTftpPacket(targetTftpPacket, this->fileName, mode, num);
        if(type != tftpAck){

        }
        else if(num != 0){
            socket.sendto(tftpPacket.c_str(), tftpPacket.size(), 0, (sockaddr*)targetAddr, sizeof(sockaddr_in));
        }
        else {
            break;
        }
    }
    block_t blockNo = 1;
    char buf[max_tftp_data_len];
    std::ifstream fin;
    try{
        fin.open("./tmp/" + fileName);
        size_t count = 1;
        do{
            fin.read(buf, max_tftp_data_len);
            count = fin.gcount();
            
            std::string data(buf, count);
            std::cout << data.size() << std::endl;
            std::string tftpDataPacket;
            makeTftpDataPacket(tftpDataPacket, data, blockNo);
            //这里使用static_cast会报错
            //socket.sendto(tftpDataPacket.c_str(), count, 0, reinterpret_cast<const sockaddr*>(targetAddr), sizeof(sockaddr));
            socket.sendto(tftpDataPacket.c_str(), count + 4, 0);
            
            for(;;){
                socket.recvfrom(0);
                tftpPacket = socket.getBuff();
                tftpType type = parseTftpPacket(tftpPacket, this->fileName, mode, num);
                if(type != tftpAck){
                    std::cout << "not ack type" << std::endl;
                }
                else if(num != blockNo){
                    socket.sendto(tftpDataPacket.c_str(), count, 0);
                }
                else {
                    std::cout << "hhhh" << std::endl;
                    break;
                }
            }
            ++blockNo;
        }while(fin && count == max_tftp_data_len);
    }catch(std::exception e){

    }
    std::cout << "send file finished" << std::endl;
}

void Tftp::receiveFileAsClient(const std::string& fileName, const sockaddr_in* targetAddr){
    std::cout << "receive file as client" << std::endl;
    std::ofstream ofs;
    ofs.open("./tmp/" + fileName, std::ios_base::trunc | std::ios_base::binary);
    if(ofs.bad()){
        return;
    }
    tftpPacket.clear();
    makeTftpReadRequestPacket(tftpPacket, fileName);
    std::cout << "send bytes" << socket.sendto(tftpPacket.c_str(), tftpPacket.size(), 0, (sockaddr*)targetAddr, sizeof(sockaddr)) << std::endl;
    block_t blockNo = 1;
    char buf[max_tftp_data_len];
    for(;;){
        socket.recvfrom(0);
        targetAddr = socket.getFrom();
        targetTftpPacket = socket.getBuff();
        tftpType type = parseTftpPacket(targetTftpPacket, this->fileName, mode, num);
        std::cout << "num = " << num << std::endl;
        if(type != tftpData){

        }
        else if(num != blockNo){
            socket.sendto(tftpPacket.c_str(), tftpPacket.size(), 0, (sockaddr*)targetAddr, sizeof(sockaddr_in));
        }
        else {
            ofs.write(targetTftpPacket.substr(4).c_str(), targetTftpPacket.size() - 4);
            makeTftpAckPacket(tftpPacket, blockNo++);
            socket.sendto(tftpPacket.c_str(), tftpPacket.size(), 0, (sockaddr*)targetAddr, sizeof(sockaddr_in));
            std::cout << "targetTftpPacket.size() = " << targetTftpPacket.size() << std::endl; 
            if(targetTftpPacket.size() < 516) break;
        }
    }
    ofs.close();
}

void Tftp::receiveFileAsServer(const std::string& fileName, const sockaddr_in* targetAddr){
    std::cout << "receive file as server" << std::endl;
    std::ofstream ofs;
    ofs.open("./tmp/" + fileName, std::ios_base::trunc | std::ios_base::binary);
    if(ofs.bad()){
        return;
    }
    tftpPacket.clear();
    makeTftpAckPacket(tftpPacket, 0);
    std::cout << "send bytes" << socket.sendto(tftpPacket.c_str(), tftpPacket.size(), 0, (sockaddr*)targetAddr, sizeof(sockaddr)) << std::endl;
    block_t blockNo = 1;
    char buf[max_tftp_data_len];
    for(;;){
        socket.recvfrom(0);
        targetTftpPacket = socket.getBuff();
        tftpType type = parseTftpPacket(targetTftpPacket, this->fileName, mode, num);
        if(type != tftpData){

        }
        else if(num != blockNo){
            socket.sendto(tftpPacket.c_str(), tftpPacket.size(), 0, (sockaddr*)targetAddr, sizeof(sockaddr_in));
        }
        else {
            ofs.write(targetTftpPacket.substr(4).c_str(), targetTftpPacket.size() - 4);
            makeTftpAckPacket(tftpPacket, blockNo++);
            socket.sendto(tftpPacket.c_str(), tftpPacket.size(), 0, (sockaddr*)targetAddr, sizeof(sockaddr_in));
            if(targetTftpPacket.size() < 516) break;
        }
    }
    ofs.close();
}