#include "arinc615a.h"
#include <iostream>

void Arinc615a::information(void* arg){
    std::cout << "in information" << std::endl;
    information_para* arg1 = static_cast<information_para*>(arg);
    sockaddr_in addr = arg1->addr;
    cond* m_cond = arg1->m_cond; 
    std::cout << "enter m_cond wait" << std::endl;
    m_cond->wait();
    std::cout << "after m_cond wait" << std::endl;
    //informationStatus status = arg1->status;
    //while (status != SEND_LCL) {
       // m_cond.wait();
    //}
    jobs.push(Job(Job::send, "02DA.LCL", addr));
    gotNewJob.signal();
    //等待LCL发送完成
    m_cond->wait();
    std::cout << "information finished\n";
    //m_cond.signal();
}

bool Arinc615a::upload() const{
    std::cout << "in upload" << std::endl;

    return true;
}

bool Arinc615a::mediaDownload() const{
    return false;
}

bool Arinc615a::operatorDownload() const{
    return true;
}