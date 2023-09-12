#define UNIX
#include "locker.h"
#include "thread.h"
#include "socket.h"
#include<iostream>
void* initTftpServer(void*){
    //std::cout << "hello world " << pthread_self() << std::endl;
    udp_socket usocket;
    std::cout << usocket.bind(69) << std::endl;
    usocket.recvfrom(0);
    return NULL;
}

void* initfindServer(void*){
    udp_socket usocket;
    std::cout << "in initfindserver" << std::endl;
    if(usocket.bind(1001) < 0){
        std::cout << "bind error" << std::endl;
    }
    for(;;){
        usocket.recvfrom(0);
        std::cout << "recv find request " << pthread_self() << std::endl;
    }
    return NULL;
}

int main(){
    thread tftpServer_thread(initTftpServer);
    thread findServer_thread(initfindServer);
    return 0;
}