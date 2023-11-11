#include "tftpserver.h"
#include "tftpclient.h"
#include "thread.h"
#include "arinc615a.h"
#include <thread>
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
            std::string pattern_LNO("\\.LNO$");
            std::regex r_LNO(pattern_LNO);
            if(std::regex_search(fileName, r_LCI) == true){
                cond* m_cond = new cond();
                information_para* args = new information_para();
                args->addr = *socket.getFrom();
                args->m_cond = m_cond;
                //args->addr.sin_port = htons(8888);
                conds[*socket.getFrom()] = m_cond;
                std::thread informationThread((MyThreadFunction)Arinc615a::information, args);
                informationThread.detach();
            }
            else if (std::regex_search(fileName, r_LUI) == true) {
                cond* m_cond = new cond();
                upload_para* args = new upload_para();
                args->addr = *socket.getFrom();
                args->m_cond = m_cond;
                conds[*socket.getFrom()] = m_cond;
                std::cout << "LUI MATCH SUCCESS" << std::endl;
                std::thread uploadThread(Arinc615a::upload, args);
                uploadThread.detach();
                //this->sendFile(path, socket.getFrom());
            }
            else if(std::regex_search(fileName, r_LNO) == true){
                cond* m_cond = new cond();
                oDownload_para* args = new oDownload_para();
                args->addr = *socket.getFrom();
                args->m_cond = m_cond;
                conds[*socket.getFrom()] = m_cond;
                std::cout << "LNO MATCH SUCCESS" << std::endl;
                std::thread oDownloadThead(Arinc615a::operatorDownload, args);
                oDownloadThead.detach();
            }
            break;
        }
        case TftpServer::tftpWriteRequest: {
            std::string pattern_LUR("\\.LUR$");
            std::regex r_LUR(pattern_LUR);
            std::string pattern_LNA("\\.LNA$");
            std::regex r_LNA(pattern_LNA);
            std::cout << "tftpWriteRequest" << std::endl;
            if(std::regex_search(fileName, r_LUR) == true){
                jobs.push(Job(Job::receive, Job::server, fileName, *socket.getFrom()));
                gotNewJob.signal();
            }
            else if(std::regex_search(fileName, r_LNA) == true){
                jobs.push(Job(Job::receive, Job::server, fileName, *socket.getFrom()));
                gotNewJob.signal();
            }
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
            socket.sendto(tftpDataPacket.c_str(), count + 4, 0, reinterpret_cast<const sockaddr*>(targetAddr), sizeof(sockaddr));
            
            for(;;){
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

void TftpServer::receiveFile(const std::string &fileName, const sockaddr_in *targetAddr)
{
}
