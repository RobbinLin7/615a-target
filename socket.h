#ifndef _SOCKET_H_
#define _SOCKET_H_
#include<sys/socket.h>
#include<netinet/in.h>
#include<iostream>
#include<unistd.h>
const size_t max_bytes = 512;
class udp_socket{
public:
    udp_socket(){
        sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
        std::cout << sockfd << std::endl;
    }
    ~udp_socket(){
        close(sockfd);
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
    ssize_t recvfrom(int flags){
        return ::recvfrom(sockfd, buff, max_bytes, flags, &from, &addrlen);
    }

    ssize_t sendto(int nbytes, int flags, const sockaddr* to, socklen_t addrlen){
        return ::sendto(sockfd, buff, nbytes, flags, to, addrlen);
    }
private:
    int sockfd;
    sockaddr from;
    socklen_t addrlen;
    char buff[max_bytes];
};


class tcp_socket{

};
#endif