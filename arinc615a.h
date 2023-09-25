#ifndef _ARINC615A_H_
#define _ARINC615A_H_
//#include<sys/socket.h>
//#include<netinet/in.h>
//#include<arpa/inet.h>
//#include"locker.h"
class Arinc615a{
public:
    Arinc615a(sockaddr_in* addr):addr(addr){}
    bool information()const;
    bool upload()const;
    bool mediaDownload()const;
    bool operatorDownload()const;
private:
    sockaddr_in* addr;
    //cond m_cond;
};
#endif