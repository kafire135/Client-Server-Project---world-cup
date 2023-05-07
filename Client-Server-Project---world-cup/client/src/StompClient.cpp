#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"
#include "../include/Task.h"
#include <cstring>

#include <thread>

//valgrind --leak-check=full --show-reachable=yes bin/StompWCIClient

int main(int argc, char *argv[]) {
	// TODO: implement the STOMP client
    std::cout << "Stomp client started- please login :)" << std::endl;
    StompProtocol protocol;
    const short bufsize = 1024;
    char buf[bufsize];
    std::cin.getline(buf, bufsize);
	std::string line(buf);
	std::string frame = protocol.createFrame(line);
    std::string delimiter = "\n";
    size_t pos = 0;
    std::string token;
    pos=frame.find(delimiter);
    token = frame.substr(0,pos);
    if(token!="CONNECT"){
        std::cout << "ERROR you must login correctly first" << std::endl;
        std::cout << "Disconnected. Exiting...\n" << std::endl;
        return -1;
    }
    std::string host_port = line.substr(6);
    delimiter = " ";
    pos = line.find(delimiter);
    token = host_port.substr(0,pos);
    delimiter = ":";
    pos = host_port.find(delimiter);
    std::string host = host_port.substr(0, pos);
    std::string port_s = host_port.substr(pos+1);
    char* chars = new char[port_s.length()+1];
    strcpy(chars, port_s.c_str());
    short port;
    try{
        port = atoi(chars);
        
    }
    catch(...){
        std::cerr << "port is illegal" << std::endl;
        return 1;
    }
    delete[] chars;
    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

    std::mutex mutex;
    Task task(connectionHandler, mutex, protocol, frame);
    std::thread keyBoard(&Task::keyBoardAction, &task);
    std::thread socket(&Task::socketAction, &task);
    keyBoard.join();
    socket.join();
    return 0;
}


