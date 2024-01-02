#ifndef _JOB_H_
#define _JOB_H_
#pragma once
#include<string>
class Job {
public:
    enum JobType { send, receive };
    enum Role {client, server};
    Job() {}
    Job(JobType jobType, Role role, std::string fileName, sockaddr_in addr): jobType(jobType), role(role),
    fileName(fileName), targetAddr(addr) {}
    std::string getFileName() {
        return fileName;
    }
    sockaddr_in getTargetAddr() {
        return targetAddr;
    }
    JobType getJobType(){
        return jobType;
    }
    Role getRole(){
        return role;
    }
private:
    JobType jobType;
    Role role;
    std::string fileName;
    sockaddr_in targetAddr;
};
#endif