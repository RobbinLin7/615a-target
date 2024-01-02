#include "arinc615a.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <filesystem>

void Arinc615a::information(information_para* arg){
    std::cout << "in information" << std::endl;
    cond* m_cond = arg->m_cond;
    //information_para* arg1 = static_cast<information_para*>(arg);
    sockaddr_in addr = arg->addr;
    while(arg->status != information_para::END){
        switch (arg->status){
        case information_para::SEND_LCI:
//<<<<<<< HEAD
            jobs.push(Job(Job::send, Job::server, "02DA.LCI", addr));
//=======
            //jobs.push(Job(Job::send, Job::server, "./tmp/02DA.LCI", addr));
//>>>>>>> 9953ffaca2e91362743a759e3b91c6e1c69f0979
            gotNewJob.signal();
            m_cond->wait();
            arg->status = information_para::SEND_LCL;
            break;
        case information_para::SEND_LCL:{
            sockaddr_in target_server_addr = addr;
            target_server_addr.sin_port = htons(8888);
            jobs.push(Job(Job::send, Job::client, "02DA.LCL", target_server_addr));
            gotNewJob.signal();
            m_cond->wait();
            arg->status = information_para::END;
            break;
        }
        case information_para::END:
            break;
        default:
            break;
        }
    }
    std::cout << "information finished\n";
}
static int h = 0;
void Arinc615a::upload(upload_para* arg){
    std::cout << "in upload" << std::endl;
    cond* m_cond = arg->m_cond;
    sockaddr_in addr = arg->addr;
    sockaddr_in target_server_addr = addr;
    target_server_addr.sin_port = htons(8888);
    while(arg->status != upload_para::END){
        switch(arg->status){
            case upload_para::SEND_LUI:{
                jobs.push(Job(Job::send, Job::server, "02DA.LUI", addr));
                gotNewJob.signal();
                m_cond->wait();
                arg->statusCode = 1;
                arg->statusDescription = "accpeted";
                arg->status = upload_para::WRITE_LUS;
                break;
            }
            case upload_para::WRITE_LUS:{
                makeLUS(arg);
                jobs.push(Job(Job::send, Job::client, "02DA.LUS", target_server_addr));
                gotNewJob.signal();
                switch(arg->statusCode){
                    case 0x0001:
                        std::cout << "0x0001" << std::endl;
                        arg->status = upload_para::RECEIVE_LUR;
                        break;
                    case 0x0002:
                        std::cout << "0x0002" << std::endl;
                        arg->status = upload_para::READ_DATA_FILE;
                        break;
                    case 0x0003:
                        std::cout << "0x0003" << std::endl;
                        arg->status = upload_para::END;
                        break;
                    case 0x1003:
                        std::cout << "0x1003" << std::endl;
                        break;
                    case 0x1004:
                        std::cout << "0x1004" << std::endl;
                        break;
                    case 0x1005:
                        std::cout << "0x1005" << std::endl;
                        break;
                    default:
                        std::cout << "default" << std::endl;
                        break;
                }
                break;
            }
            case upload_para::RECEIVE_LUR:
                std::cout << "LUR" << std::endl;
                m_cond->wait();
                arg->status = upload_para::READ_LUH;
                ++h;
                break;
            case upload_para::READ_LUH:
                jobs.push(Job(Job::receive, Job::client, "02DA.LUH", target_server_addr));
                gotNewJob.signal();
                m_cond->wait();
                parseLUH(arg);
                arg->status = upload_para::READ_DATA_FILE;
                break;
            case upload_para::READ_DATA_FILE:
                jobs.push(Job(Job::receive, Job::client, arg->fileList[arg->cur++], target_server_addr));
                std::cout << arg->cur << " 23423" << std::endl;
                gotNewJob.signal();
                m_cond->wait();
                if(arg->cur < arg->fileList.size()){
                    arg->statusCode = 0x0002;
                    arg->statusDescription = "upload in progress";
                }
                else{
                    arg->statusCode = 0x0003;
                    arg->statusDescription = "upload complete";
                }
                arg->status = upload_para::WRITE_LUS;
                break;

            case upload_para::END:
                break;
        }
    }
    std::cout << "upload finished\n";
}

void Arinc615a::operatorDownload(oDownload_para* arg){
    std::cout << "in operator download" << std::endl;
    cond* m_cond = arg->m_cond;
    sockaddr_in addr = arg->addr;
    sockaddr_in target_server_addr = addr;
    target_server_addr.sin_port = htons(8888);
    while(arg->status != oDownload_para::END){
        switch(arg->status){
            case oDownload_para::SEND_LNO:
                jobs.push(Job(Job::send, Job::server, "02DA.LNO", addr));
                gotNewJob.signal();
                m_cond->wait();
                arg->statusCode = 0x0001;
                arg->statusDescription = "oDownload accpet";
                arg->status = oDownload_para::WRITE_LNS;
                break;
            case oDownload_para::WRITE_LNS:
                makeLNS(arg);
                jobs.push(Job(Job::send, Job::client, "02DA.LNS", target_server_addr));
                gotNewJob.signal();
                switch(arg->statusCode){
                    case 0x0001:
                        arg->status = oDownload_para::WRITE_LNL;
                        break;
                    case 0x0002:
                        arg->status = oDownload_para::WRITE_DATA_FILE;
                        break;
                    case 0x0003:
                        arg->status = oDownload_para::END;
                        break;
                    default:
                        std::cout << "oDownload default ...." << std::endl;
                        break;
                }
                break;
            case oDownload_para::WRITE_LNL:
                makeLNL(arg);
                jobs.push(Job(Job::send, Job::client, "02DA.LNL", target_server_addr));
                gotNewJob.signal();
                m_cond->wait();
                arg->status = oDownload_para::RECEIVE_LNA;
                break;
            case oDownload_para::RECEIVE_LNA:
                m_cond->wait();
                parseLNA(arg);
                arg->status = oDownload_para::WRITE_LNS;
                arg->statusCode = 0x0002;
                arg->statusDescription = "odownload is about to begin";
                break;
            case oDownload_para::WRITE_DATA_FILE:
                jobs.push(Job(Job::send, Job::client, arg->requestFileList[arg->cur++], target_server_addr));
                gotNewJob.signal();
                m_cond->wait();
                if(arg->cur == arg->requestFileList.size()){
                    arg->statusCode = 0x0003;
                    arg->statusDescription = "odownload complete";
                }
                else{
                    arg->statusCode = 0x0002;
                    arg->statusDescription = "odownload in progress";
                }
                arg->status = oDownload_para::WRITE_LNS;
                break;
            case oDownload_para::END:
                break;
            default:
                break;
        }
    }
}



void Arinc615a::makeLUS(upload_para* arg){
    std::ofstream os("./tmp/02DA.LUS", std::ios_base::trunc | std::ios_base::binary);
    if(os){
        os.seekp(4);
        os << "A3";
        os.write((char*)&arg->statusCode, sizeof(arg->statusCode));
        os << (unsigned char)arg->statusDescription.size();
        os << arg->statusDescription; 
        auto size = os.tellp();
        os.seekp(0);
        os.write(reinterpret_cast<char*>(&size), sizeof(unsigned));
        os.close();
    }
}

void Arinc615a::parseLUH(upload_para *arg){
    std::ifstream ifs("./tmp/02DA.LUH", std::ios::in);
    ifs.seekg(0, std::ios_base::end);
    std::cout << ifs.tellg() << std::endl;
    std::cout << "before" << std::endl;
    if (!ifs.is_open()) std::cout << "failed to open " << std::endl;
    ifs.seekg(0, std::ios::beg);
    if(ifs){
        std::cout << "after" << std::endl;
        ifs.seekg(40);
        unsigned short fileListSize;
        ifs.read((char*)&fileListSize, sizeof(fileListSize));
        //ifs >> fileListSize;
        std::cout << "fileListSize " << fileListSize << std::endl;
        for(int i = 0; i < fileListSize; i++){
            auto now = ifs.tellg();
            std::cout << "now = " << now << std::endl;
            ifs.seekg(2, std::ios_base::cur);
            unsigned short fileNameLen;
            ifs.read((char*)&fileNameLen, sizeof(unsigned short));
            
            //ifs >> fileNameLen;
            std::string fileName(fileNameLen, '\0');
            ifs.read(&fileName[0], fileNameLen);
            arg->fileList.push_back(fileName);
            std::cout << fileName << std::endl;
            ifs.seekg(789 + now);
        }
        ifs.close();
    }
}

void Arinc615a::makeLNS(oDownload_para *arg){
    std::ofstream os("./tmp/02DA.LNS", std::ios_base::trunc | std::ios_base::trunc);
    if(os){
        os.seekp(4);
        os << "A3";
        os.write(reinterpret_cast<char*>(&arg->statusCode), sizeof(arg->statusCode));
        //os.seekp(273);
        os << (unsigned char)arg->statusDescription.size();
        os << arg->statusDescription;
        // std::filesystem::directory_entry entry("../tmp");
        // if(entry.exists()){
        //     std::filesystem::directory_iterator list("../tmp");
        //     for(auto it : list){
        //         arg->fileList.push_back(it.path().filename());
        //     }
        // }
        // os.write(reinterpret_cast<char*>(arg->fileList.size()), sizeof(short));
        // for(int i = 0; i < arg->fileList.size(); i++){
        //     auto cur = os.tellp();
        //     os << (unsigned char)arg->fileList[i].size();
        //     os << arg->fileList[i];
        //     os.seekp(cur + 514);
        // }
        os.seekp(273);
        unsigned short requestFileListSize = arg->requestFileList.size();
        os.write(reinterpret_cast<char*>(&requestFileListSize), sizeof(unsigned short));
        auto size = os.tellp();
        os.seekp(0);
        os.write(reinterpret_cast<char*>(&size), sizeof(unsigned));
        os.close();
    }
}

void Arinc615a::makeLNL(oDownload_para *arg){
    std::ofstream ofs("./tmp/02DA.LNL", std::ios_base::trunc);
    if(ofs){
        ofs.seekp(4);
        ofs << "A3";
        std::filesystem::directory_entry entry("./tmp");
        if(entry.exists()){
            std::filesystem::directory_iterator list("./tmp");
            for(auto it : list){
                arg->fileList.push_back(it.path().filename().string());
                if(it.is_directory()){
                    arg->fileType.push_back("directorys");
                }
                else{
                    arg->fileType.push_back("files");
                }
            }
        }
        short fileListSize = arg->fileList.size();
        ofs.write(reinterpret_cast<char*>(&fileListSize), sizeof(short));
        for(int i = 0; i < arg->fileList.size(); i++){
            ofs << (unsigned char)arg->fileList[i].size();
            auto cur = ofs.tellp();
            ofs << arg->fileList[i];
            ofs.seekp(255 + cur);
            ofs << (unsigned char)arg->fileType[i].size();
            cur = ofs.tellp();
            ofs << arg->fileType[i];
            if(i < arg->fileList.size() - 1){
                ofs.seekp(255 + cur);
            }
        }
        auto size = ofs.tellp();
        ofs.seekp(0);
        ofs.write(reinterpret_cast<char*>(&size), sizeof(signed));
        ofs.close();
    }
}

void Arinc615a::parseLNA(oDownload_para *arg){
    std::ifstream ifs("./tmp/02DA.LNA", std::ios_base::binary);
    if(ifs){
        //ifs.seekg(4);
        unsigned fileLen;
        ifs.read(reinterpret_cast<char*>(&fileLen), sizeof(unsigned));
        std::string version(2, '\0');
        ifs.read(&version[0], 2);
        short fileCnt;
        ifs.read(reinterpret_cast<char*>(&fileCnt), sizeof(fileCnt));
        for(int i = 0; i < fileCnt; i++){
            unsigned char fileNameLen;
            ifs.read(reinterpret_cast<char*>(&fileNameLen), sizeof(fileNameLen));
            std::string fileName(fileNameLen, '\0');
            ifs.read(&fileName[0], fileNameLen);
            arg->requestFileList.push_back(fileName);
            std::cout << "fileName=" << fileName << std::endl;
            ifs.seekg(768 - fileNameLen, std::ios_base::cur);
        }
        ifs.close();
    }
}
