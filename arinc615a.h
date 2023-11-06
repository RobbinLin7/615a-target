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
    enum informationStatus {SEND_LCI, SEND_LCL} status;
};

struct upload_para {
    sockaddr_in addr;
    cond* m_cond;
    enum uploadStatus{SEND_LUI, WRITE_LUS, READ_LUR, READ_FILE, END};
};

class Arinc615a{
public:
    Arinc615a() = delete;
    enum uploadStatus{};
    enum mediaDownloadStatus{};
    enum operatorDownloadStatus{};
    
    static void information(void* arg);
    
    bool upload()const;
    bool mediaDownload()const;
    bool operatorDownload()const;
};
#endif