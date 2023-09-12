#include "arinc615a.h"
#include <iostream>

bool Arinc615a::information() const{
    char ip_address[256];
    if(inet_ntop(AF_INET, &(addr->sin_addr), ip_address, INET_ADDRSTRLEN) == NULL){
        throw std::exception();
    }
    printf("IPv4 地址: %s\n", ip_address);
    std::cout << "in information" << std::endl;
    return true;
}

bool Arinc615a::upload() const{
    return true;
}

bool Arinc615a::mediaDownload() const{
    return false;
}

bool Arinc615a::operatorDownload() const{
    return true;
}