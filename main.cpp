#include "locker.h"
#include "socket.h"
#include "thread.h"
#include "tftpserver.h"
#include "tftpclient.h"
#include "arinc615a.h"
#include<iostream>
#include<fstream>
#include<regex>
#include<queue>
#include<map>

const size_t max_tftpClient_cnt = 2;
TftpClient tftpClients[max_tftpClient_cnt];  //tftpClient池
cond gotNewJob;
std::queue<Job> jobs;                       //任务队列
std::map<sockaddr_in, cond> conds;


void* initTftpServer(void*){
    TftpServer* server = TftpServer::getTFTPServerInstance();
    server->startTftpServer();
    return NULL;
}

void func(sockaddr_in* addr){
    //sockaddr_in* addr_in = (sockaddr_in*)addr;
    //char ip_address[256];
    //if(inet_ntop(AF_INET, &(addr->sin_addr), ip_address, INET_ADDRSTRLEN) == NULL){
        //throw std::exception();
    //}
    //printf("IPv4 地址: %s\n", ip_address);
}

void* initfindServer(void*){
    udp_socket usocket;
    std::cout << "in initfindserver" << std::endl;
    if(usocket.bind(1001) < 0){
        std::cerr << "bind error" << std::endl;
        throw std::exception();
    }
    char buf[512];
    for(;;){
        usocket.recvfrom(0);
        std::cout << "hahah" << std::endl;
#ifdef _WIN32
        std::ifstream ifs("./tmp/find.bin");
#elif
        std::ifstream ifs("../tmp/find.bin");
#endif
        while(ifs){
            std::cout << "hahah" << std::endl;
            ifs.read(buf, 512);
            size_t count = ifs.gcount();
            std::cout << "count:" << count << std::endl;
            usocket.sendto(buf, count, 0);
        }
        ifs.close();
    }
    return NULL;
}

DWORD func(LPVOID) {
    std::cout << "hello world123456\n";
    return 1;
}


int main(){
    thread tftpServer_thread((MyThreadFunction)initTftpServer);
    thread findServer_thread((MyThreadFunction)initfindServer);
    return 0;
}