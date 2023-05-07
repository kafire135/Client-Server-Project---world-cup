#pragma once

#include "../include/Summary.h"
#include <map>
#include <queue>
#include <string>

using std::map;

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    std::map<std::string, int> topics;
    int counterSub;
    int counterReceipt;
    std::string userName;
    std::map<std::pair<std::string, std::string>, Summary> userGame;
    std::string login(std::queue<std::string> q);
    std::string join(std::queue<std::string> q);
    std::string exit(std::queue<std::string> q);
    std::string report(std::queue<std::string> q);
    std::string logout(std::queue<std::string> q);
    std::string summary(std::queue<std::string> q);
    bool createFile(std::string fileName, Summary sum);
    
public:
    StompProtocol();
    std::string createFrame(std::string line); 
    void update (std::pair<std::string, std::string> pair, Event event, std::string frame);
    std::string getUserName();
};
