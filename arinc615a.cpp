#include "arinc615a.h"
#include <iostream>

void Arinc615a::information(void* arg){
    std::cout << "in information" << std::endl;
    information_para* arg1 = static_cast<information_para*>(arg);
    sockaddr_in& addr = arg1->addr;
    cond& m_cond = arg1->m_cond; 
    informationStatus& status = arg1->status;
    while (status != SEND_LCL) {
        m_cond.wait();
    }
    jobs.push(Job(Job::send, "02DA.LCL", addr));
    m_cond.signal();
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