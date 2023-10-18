#ifndef _JOB_H_
#define _JOB_H_
#pragma once
#include<string>
class Job {
public:
    enum JobType { send, receive };
    Job(JobType jobType, std::string fileName, sockaddr_in addr) :jobType(jobType), fileName(fileName), targetAddr(addr) {}
    JobType getJobType() {
        return jobType;
    }
    std::string getFileName() {
        return fileName;
    }
    sockaddr_in getTargetAddr() {
        return targetAddr;
    }
private:
    JobType jobType;
    std::string fileName;
    sockaddr_in targetAddr;
};
#endif