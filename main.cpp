#include "locker.h"
#include "socket.h"
#include "thread.h"
#include "worker.h"
#include "tftpserver.h"
#include "tftpclient.h"
#include "arinc615a.h"
#include<iostream>
#include<fstream>
#include<regex>
#include<queue>
#include<map>
#include<thread>
std::mutex mtx;
const size_t max_worker_cnt = 5;
Worker workers[max_worker_cnt];
//TftpClient tftpClients[max_tftpClient_cnt];  //tftpClient池
cond gotNewJob;
std::queue<Job> jobs;                       //任务队列

std::string sockaddr_in_to_string(const sockaddr_in& addr) {
    //static char ip_address[256];
    const char* ip = inet_ntoa(addr.sin_addr);
    return std::string(ip);
}
bool cmp(const sockaddr_in& addr1, const sockaddr_in& addr2) {
    return sockaddr_in_to_string(addr1) < sockaddr_in_to_string(addr2);
}
std::map<sockaddr_in, cond*, decltype(cmp)*> conds(cmp);




void initTftpServer(void){
    TftpServer* server = TftpServer::getTFTPServerInstance();
    server->startTftpServer();
}

void func(sockaddr_in* addr){
    //sockaddr_in* addr_in = (sockaddr_in*)addr;
    //char ip_address[256];
    //if(inet_ntop(AF_INET, &(addr->sin_addr), ip_address, INET_ADDRSTRLEN) == NULL){
        //throw std::exception();
    //}
    //printf("IPv4 地址: %s\n", ip_address);
}

void initfindServer(void){
    udp_socket usocket;
    std::cout << "in initfindserver" << std::endl;
    if(usocket.bind(1001) < 0){
        std::cerr << "bind error" << std::endl;
        throw std::exception();
    }
    char buf[512];
    for(;;){
        usocket.recvfrom(0);
        //std::cout << "hahah" << std::endl;
        std::ifstream ifs("./tmp/find.bin");
        while(ifs){
            //std::cout << "hahah" << std::endl;
            ifs.read(buf, 512);
            size_t count = ifs.gcount();
            //std::cout << "count:" << count << std::endl;
            usocket.sendto(buf, count, 0);
        }
        ifs.close();
    }
}

void initTftpClient(void) {
    static int ptr = 0;
    workers[ptr++].run();
}


int main(){
    std::thread tftpServer_thread(initTftpServer);
    std::thread findServer_thread(initfindServer);
    std::thread worker_threads[max_worker_cnt];
    for (int i = 0; i < max_worker_cnt; i++) {
        worker_threads[i] = std::thread(initTftpClient);
    }
    tftpServer_thread.join();
    findServer_thread.join();
    for (int i = 0; i < max_worker_cnt; i++) {
        worker_threads[i].join();
    }
    return 0;
}
