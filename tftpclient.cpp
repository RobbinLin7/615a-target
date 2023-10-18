#include "tftpclient.h"

void TftpClient::sendFile(const std::string &fileName, const sockaddr_in *targetAddr){
    makeTftpWriteRequestPacket(tftpPacket, fileName);
    std::string targetTftpPacket;
    std::cout << "send bytes" << socket.sendto(tftpPacket.c_str(), tftpPacket.size(), 0, (sockaddr*)targetAddr, sizeof(sockaddr)) << std::endl;
    for(;;){
        socket.recvfrom(0);
        targetTftpPacket = socket.getBuff();
        tftpType type = parseTftpPacket(targetTftpPacket, this->fileName, mode, num);
        if(type != tftpAck){

        }
        else if(num != 0){
            socket.sendto(tftpPacket.c_str(), tftpPacket.size(), 0, (sockaddr*)targetAddr, sizeof(sockaddr_in));
        }
        else {
            break;
        }
    }
    block_t blockNo = 1;
    char buf[max_tftp_data_len];
    std::ifstream fin;
    try{
        fin.open("./tmp/" + fileName);
        size_t count = 1;
        do{
            fin.read(buf, max_tftp_data_len);
            count = fin.gcount();
            
            std::string data(buf, count);
            std::cout << data.size() << std::endl;
            std::string tftpDataPacket;
            makeTftpDataPacket(tftpDataPacket, data, blockNo);
            //这里使用static_cast会报错
            //socket.sendto(tftpDataPacket.c_str(), count, 0, reinterpret_cast<const sockaddr*>(targetAddr), sizeof(sockaddr));
            socket.sendto(tftpDataPacket.c_str(), count + 4, 0);
            
            for(;;){
                socket.recvfrom(0);
                tftpPacket = socket.getBuff();
                tftpType type = parseTftpPacket(tftpPacket, this->fileName, mode, num);
                if(type != tftpAck){
                    std::cout << "not ack type" << std::endl;
                }
                else if(num != blockNo){
                    socket.sendto(tftpDataPacket.c_str(), count, 0);
                }
                else {
                    std::cout << "hhhh" << std::endl;
                    break;
                }
            }
            ++blockNo;
        }while(fin && count == max_tftp_data_len);
    }catch(std::exception e){

    }
    std::cout << "send file finished" << std::endl;
}

void TftpClient::receiveFile(const std::string &fileName, const sockaddr_in *targetAddr){

}

void TftpClient::run() {
    while (true) {
        while (jobs.empty() == true) {
            gotNewJob.wait();
        }
        Job job = jobs.front();
        jobs.pop();
        if (job.getJobType() == Job::send) {
            sendFile(job.getFileName(), &job.getTargetAddr());
        }
        else if (job.getJobType() == Job::receive) {
            receiveFile(job.getFileName(), &job.getTargetAddr());
        }
    }
}
