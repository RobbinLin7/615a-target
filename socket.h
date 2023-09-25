#ifndef _SOCKET_H_
#define _SOCKET_H_
#include<iostream>

#ifdef _WIN32
#include<winsock2.h>
#include<windows.h>
#pragma comment(lib,"ws2_32.lib")
typedef struct sockaddr_in sockaddr_in;
typedef struct WSAData WSAData;
typedef struct sockaddr sockaddr;
typedef int socklen_t;
#define MY_CLOSE_SOCKET(s) closesocket(s)
#endif // 


#ifdef UNIX
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#define MY_CLOSE_SOCKET(s) close(s)
#endif // 

const size_t max_bytes = 512;
void func(sockaddr_in* addr);
class udp_socket{
public:
    udp_socket(){
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            // 处理初始化失败的情况
            throw std::exception();
        }
#endif
        sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
        std::cout << sockfd << std::endl;
    }
    ~udp_socket(){
        MY_CLOSE_SOCKET(sockfd);
    }
    int connect(const sockaddr* serveraddr, socklen_t addrlen){
        return ::connect(sockfd, serveraddr, addrlen);
    }
    int bind(uint16_t port, uint32_t ip = INADDR_ANY){
        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(ip);
        server_addr.sin_port = htons(port);
        return ::bind(sockfd, (sockaddr*)&server_addr, sizeof(server_addr));
    }
    int recvfrom(int flags){
        
        // char ip_address[256];
        // if(inet_ntop(AF_INET, &(from.sin_addr), ip_address, INET_ADDRSTRLEN) == NULL){
        //     throw std::exception();
        // }
        // printf("IPv4 地址: %s\n", ip_address); 
        return count = ::recvfrom(sockfd, buff, max_bytes, flags, (sockaddr*)&from, &addrlen);
    }

    int sendto(const char* buff, int nbytes, int flags, const sockaddr* to, socklen_t addrlen){
        func((sockaddr_in*)to);
        return ::sendto(sockfd, buff, nbytes, flags, to, addrlen);
    }

    int sendto(const char* buff, int nbytes, int flags){
        return ::sendto(sockfd, buff, nbytes, flags, (sockaddr*)&from, sizeof(sockaddr_in));
    }

    sockaddr_in* getFrom() const{
        return &from;
    }

    std::string getBuff(){
        return std::string(buff, count);
    }
private:
    int sockfd;
    mutable sockaddr_in from;
    socklen_t addrlen = sizeof(sockaddr);
    char buff[max_bytes];
    int count;
};


class tcp_socket{


};
#endif