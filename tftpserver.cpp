#include "tftpserver.h"
TftpServer::TftpServer(){
    if(socket.bind(69) < 0){
        std::cerr << "bind error" << std::endl;
        throw std::exception();
    }
}

TftpServer *TftpServer::getTFTPServerInstance()
{
    return new TftpServer();
}

void TftpServer::startTftpServer(){
    for(;;){
        socket.recvfrom(0);
        tftpPacket = socket.getBuff();
        TftpServer::tftpType type = TftpServer::parseTftpPacket(tftpPacket, fileName, mode, num);
        switch (type)
        {
        case TftpServer::tftpReadRequest:{
            /* code */
            std::cout << "tftpReadRequest " <<  fileName  << " " << fileName.size() << std::endl;
            std::string pattern("\\.LCI$");
            std::regex r_LCI(pattern);
            if(std::regex_search(fileName, r_LCI) == true){
                std::cout << "LCI MATCH SUCCESS" << std::endl;
                std::string path = "../tmp/" + fileName;
                this->sendFile(path, socket.getFrom());
                makeTftpWriteRequestPacket(tftpPacket, "02DA.LCL");
                path = "../tmp/02DA.LCL";
                //this->sendFile(path, socket.getFrom);
            }
            break;
        }
        case TftpServer::tftpWriteRequest:
            std::cout << "tftpWriteRequest" << std::endl;
            break;
        case TftpServer::tftpData:
            std::cout << "tftpData" << std::endl;
            break;
        case TftpServer::tftpAck:
            std::cout << "tftpAck" << std::endl;
            break;
        case TftpServer::tftpError:
            std::cout << "tftpError" << std::endl;
            break;
        default:
            std::cout << "undefined " << type << std::endl;
            break;
        }
    }
    //return NULL;
}



void TftpServer::makeTftpAckPacket(std::string& ack, block_t ackNo){
    //1.opcode
    ack.push_back('\0');
    ack.push_back(static_cast<char>(4));
    //2.ackNo
    ack.push_back(static_cast<char>(ackNo >> 8));
    ack.push_back(static_cast<char>(ackNo));
}

void TftpServer::sendFile(std::string &fileName, const sockaddr_in* targetAddr){
    block_t blockNo = 1;
    char buf[max_tftp_data_len];
    std::ifstream fin;
    try{
        fin.open(fileName);
        size_t count = 1;
        do{
            fin.read(buf, max_tftp_data_len);
            count = fin.gcount();
            std::string data(buf, count);
            std::string tftpDataPacket;
            makeTftpDataPacket(tftpDataPacket, data, blockNo);
            //这里使用static_cast会报错
            socket.sendto(tftpDataPacket.c_str(), count, 0, reinterpret_cast<const sockaddr*>(targetAddr), sizeof(sockaddr));
            
            for(;;){
                socket.recvfrom(0);
                tftpPacket = socket.getBuff();
                tftpType type = parseTftpPacket(tftpPacket, fileName, mode, num);
                if(type != tftpAck){

                }
                else if(num != count){
                    socket.sendto(tftpDataPacket.c_str(), count, 0, reinterpret_cast<const sockaddr*>(targetAddr), sizeof(sockaddr));
                }
            }
        }while(fin && count == max_tftp_data_len);
        fin.read(buf, max_tftp_data_len);

    }catch(std::exception e){

    }
}

void TftpServer::receiveFile(std::string &fileName){

}

TftpServer::tftpType TftpServer::parseTftpPacket(std::string &tftpPacket, std::string& fileName, std::string& mode, block_t& num)
{   
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
    case '\3':
        type = tftpData;
        num = (tftpPacket[2] << 8) + tftpPacket[3]; 
    case '\4':
        type = tftpAck;
        num = (tftpPacket[2] << 8) + tftpPacket[3]; 
    case '\5':
        type = tftpError;
      break;
    default:
        type = undefined;
        break;
    }
    return type;
}



void TftpServer::makeTftpDataPacket(std::string &tftpDataPacket, std::string &data, block_t blockNo){
    //1.opcode
    tftpDataPacket.push_back('\0');
    tftpDataPacket.push_back('\3');
    //2.blockNo
    tftpDataPacket.push_back(static_cast<char>(blockNo >> 8));
    tftpDataPacket.push_back(static_cast<char>(blockNo));
    //3.data
    tftpDataPacket.append(data);
}

void TftpServer::makeTftpReadReuqestPacket(std::string &tftpReadRequestPacket, std::string fileName, std::string mode)
{
    //1.opcode
    tftpReadRequestPacket.push_back('\0');
    tftpReadRequestPacket.push_back('\1');
    //2.fileName
    tftpReadRequestPacket.append(fileName.c_str());
    //3.mode
    tftpReadRequestPacket.append(mode.c_str());
}

void TftpServer::makeTftpWriteRequestPacket(std::string& tftpWriteRequestPacket, std::string fileName, std::string mode){
    //1.opcode
    tftpWriteRequestPacket.push_back('\0');
    tftpWriteRequestPacket.push_back('\2');
    //2.fileName
    tftpWriteRequestPacket.append(fileName.c_str());
    //3.mode
    tftpWriteRequestPacket.append(mode.c_str());
}

