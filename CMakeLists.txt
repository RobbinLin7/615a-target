cmake_minimum_required(VERSION 3.15)

# set the project name
project(615A_Target)

SET(SRC_LIST main.cpp tftpserver.cpp tftpclient.cpp tftp.cpp arinc615a.cpp worker.cpp)


# add the executable
add_executable(615A_Target ${SRC_LIST})
