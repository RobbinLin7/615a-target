#ifndef _ARINC615A_H_
#define _ARINC615A_H_
#include "socket.h"
#include "locker.h"
#include "job.h"
#include <queue>
#include "global.h"


struct information_para {
    sockaddr_in addr;
    cond* m_cond;
    enum informationStatus {SEND_LCI, SEND_LCL, END}status;
};

struct upload_para {
    sockaddr_in addr;
    cond* m_cond;
    enum uploadStatus{SEND_LUI, WRITE_LUS, RECEIVE_LUR, READ_LUH, READ_DATA_FILE, END}status;
    unsigned short statusCode;
    std::string statusDescription;
    std::vector<std::string> fileList;
    unsigned cur = 0;//记录上传到第几个文件，下标从0开始
};

struct oDownload_para{
    sockaddr_in addr;
    cond* m_cond;
    enum uploadStatus{SEND_LNO, WRITE_LNS, WRITE_LNL, RECEIVE_LNA, WRITE_DATA_FILE, END}status;
    unsigned short statusCode;
    std::string statusDescription;
    std::vector<std::string> fileList;          //记录设备端目录下有哪些可供下载的文件
    std::vector<std::string> fileType;
    std::vector<std::string> requestFileList;   //记录要下载的文件
    unsigned cur = 0;                           //记录下载到第几个文件，下标从0开始
};

class Arinc615a{
public:
    Arinc615a() = delete;
    
    static void information(information_para* arg);
    static void upload(upload_para* arg);
    static void mediaDownload();
    static void operatorDownload(oDownload_para *arg);


    static void makeLUS(upload_para* arg);
    static void parseLUH(upload_para* arg);

    static void makeLNS(oDownload_para* arg);
    static void makeLNL(oDownload_para* arg);
    static void parseLNA(oDownload_para* arg);
    
};
#endif