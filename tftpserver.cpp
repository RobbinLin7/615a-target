#include "tftpserver.h"
#include "tftpclient.h"
#include "thread.h"
#include "arinc615a.h"
TftpServer* TftpServer::tftpServerInstance = nullptr;
void func(sockaddr_in* addr);
TftpServer::TftpServer(){
    if(socket.bind(69) < 0){
        std::cerr << "bind error" << std::endl;
        throw std::exception();
    }
}

TftpServer *TftpServer::getTFTPServerInstance()
{   
    if(tftpServerInstance == nullptr){
        return (tftpServerInstance = new TftpServer());
    }
    return tftpServerInstance;
}

void TftpServer::startTftpServer(){
    for(;;){
        socket.recvfrom(0);
        tftpPacket = socket.getBuff();
        TftpServer::tftpType type = Tftp::parseTftpPacket(tftpPacket, fileName, mode, num);
        switch (type)
        {
        case TftpServer::tftpReadRequest:{
            /* code */
            std::cout << "tftpReadRequest " <<  fileName  << " " << fileName.size() << std::endl;
            std::string pattern_LCI("\\.LCI$");
            std::regex r_LCI(pattern_LCI);
            std::string pattern_LUI("\\.LUI$");
            std::regex r_LUI(pattern_LUI);
            if(std::regex_search(fileName, r_LCI) == true){
                std::shared_ptr<cond> m_cond = std::shared_ptr<cond>(new cond());
                conds[*socket.getFrom()] = *m_cond;
                thread informationThread((MyThreadFunction)Arinc615a::information);
                std::cout << "LCI MATCH SUCCESS" << std::endl;
                std::string path = "./tmp/" + fileName;
                this->sendFile(path, socket.getFrom());
                m_cond->signal();
                std::shared_ptr<TftpClient> tftpClient = std::shared_ptr<TftpClient>(new TftpClient());
                //TftpClient* tftpClient = new TftpClient();
                sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_port = htons(8888);
                addr.sin_addr = socket.getFrom()->sin_addr;
                tftpClient->sendFile("02DA.LCL", &addr);
            }
            else if (std::regex_search(fileName, r_LUI) == true) {
                std::cout << "LUI MATCH SUCCESS" << std::endl;
                std::string path = "./tmp/" + fileName;
                this->sendFile(path, socket.getFrom());
            }
            break;
        }
        case TftpServer::tftpWriteRequest: {
            std::cout << "tftpWriteRequest" << std::endl;
            std::shared_ptr<TftpClient> tftpClient = std::shared_ptr<TftpClient>(new TftpClient());
            //tftpClient->receiveFile()
            break;
        }
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


void TftpServer::sendFile(const std::string &fileName, const sockaddr_in* targetAddr){
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
                tftpType type = parseTftpPacket(tftpPacket, this->fileName, mode, num);
                if(type != tftpAck){
                    std::cout << "not ack type" << std::endl;
                }
                else if(num != blockNo){
                    socket.sendto(tftpDataPacket.c_str(), count, 0, reinterpret_cast<const sockaddr*>(targetAddr), sizeof(sockaddr));
                }
                else break;
            }
            ++blockNo;
        }while(fin && count == max_tftp_data_len);
    }catch(std::exception e){

    }
    std::cout << "send file finished" << std::endl;
}

void TftpServer::receiveFile(const std::string &fileName, const sockaddr_in *targetAddr)
{
}
