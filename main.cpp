#define UNIX
#include "locker.h"
#include "thread.h"
#include "socket.h"
#include "tftpserver.h"
#include "arinc615a.h"
#include<iostream>
#include<fstream>
#include<regex>
void* initTftpServer(void*){
    TftpServer* server = TftpServer::getTFTPServerInstance();
    server->startTftpServer();
    return NULL;
}

void func(sockaddr_in* addr){
    //sockaddr_in* addr_in = (sockaddr_in*)addr;
    char ip_address[256];
    if(inet_ntop(AF_INET, &(addr->sin_addr), ip_address, INET_ADDRSTRLEN) == NULL){
        throw std::exception();
    }
    printf("IPv4 地址: %s\n", ip_address);
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
        std::ifstream ifs("../tmp/find.bin");
        while(ifs){
            ifs.read(buf, 512);
            size_t count = ifs.gcount();
            usocket.sendto(buf, count, 0);
        }
        ifs.close();
    }
    return NULL;
}


int main(){
    thread tftpServer_thread(initTftpServer);
    thread findServer_thread(initfindServer);
    return 0;
}