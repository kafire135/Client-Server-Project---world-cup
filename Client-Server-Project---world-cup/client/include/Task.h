#pragma once

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <mutex>
#include <thread>
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"

class Task{

private:
    ConnectionHandler& _handler;
    std::mutex& _mutex;   
    StompProtocol& _protocol;
    bool terminate;
    bool disconnected;
    std::string lastCommand;
    std::string getUserName(std::string frame);
    std::string _login;

public:
    Task(ConnectionHandler& handler, std::mutex& mutex, StompProtocol& protocol, std::string login);
    void keyBoardAction();
    void socketAction();
    std::string createBody(Event event);

};