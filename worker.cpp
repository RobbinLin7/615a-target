#include "worker.h"
#include "global.h"
extern std::mutex mtx;
int Worker::i = 0;
void Worker::run(){
    Job job;
    while (true) {
        while (jobs.empty() == true) {
            std::cout << "worker " << id << "sleep" << std::endl;
            gotNewJob.wait();
            std::unique_lock<std::mutex> lock(mtx);
            if (jobs.empty() == false) {
                job = jobs.front();
                jobs.pop();
                break;
            }
        }
        std::cout << "worker " << id << "wake up" << std::endl;
        //Job job = jobs.front();
        //jobs.pop();
        std::cout << "got new job" << std::endl;
        sockaddr_in addr = job.getTargetAddr();
        if (job.getJobType() == Job::send) {
            if(job.getRole() == Job::client){
                tftp.sendFileAsClient(job.getFileName(), &addr);
            }
            else{
                tftp.sendFileAsServer(job.getFileName(), &addr);
            }
        }
        else if (job.getJobType() == Job::receive) {
            if(job.getRole() == Job::client){
                tftp.receiveFileAsClient(job.getFileName(), &addr);
            }
            else{
                tftp.receiveFileAsServer(job.getFileName(), &addr);
            }
        }
        if(job.getFileName().substr(job.getFileName().find_first_of('.') + 1) == "LUS" || \
        job.getFileName().substr(job.getFileName().find_first_of('.') + 1) == "LNS"){
            std::cout << "LUS No need to wait" << std::endl;
            continue;
        }
        conds[job.getTargetAddr()]->signal();
        std::cout << "worker exit" << std::endl;
    }
}

