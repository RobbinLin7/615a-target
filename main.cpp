#define UNIX
#include "locker.h"
#include "thread.h"
#include "socket.h"
#include "tftp.h"
#include "arinc615a.h"
#include<iostream>
void* initTftpServer(void*){
    //std::cout << "hello world " << pthread_self() << std::endl;
    udp_socket usocket;
    if(usocket.bind(69) < 0){
        std::cerr << "bind error" << std::endl;
        throw std::exception();
    }
    usocket.recvfrom(0);
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
    using namespace std;
    string tftpRequest;
    string fileName, mode;
    block_t num;
    for(;;){
        usocket.recvfrom(0);
        tftpRequest = usocket.getBuff();
        //func(&usocket.getFrom());
        tftp::parseTftpPacket(tftpRequest, fileName, mode, num);
        Arinc615a arinc615a(usocket.getFrom());
        arinc615a.information();
    }
    return NULL;
}


int main(){
    thread tftpServer_thread(initTftpServer);
    thread findServer_thread(initfindServer);
    return 0;
}