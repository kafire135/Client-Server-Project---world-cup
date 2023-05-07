#include "../include/StompProtocol.h"
#include <iostream>
#include <fstream>
#include <string>
#include <queue>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

StompProtocol::StompProtocol() : topics(std::map<std::string, int>()), counterSub(0), counterReceipt(0),userName(""), userGame() {}


std::string StompProtocol::createFrame(std::string line){
    std::queue<std::string> q;
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    while((pos=line.find(delimiter))!= std::string::npos){
        token = line.substr(0,pos);
        q.push(token);
        line.erase(0,pos+delimiter.length());
    }
    q.push(line);
    std::string command = q.front();
    q.pop();
    //check first argument
    if(command=="login"){
        return login(q);
    }
    else if(command=="join"){
        return join(q);
    }
    else if(command=="exit"){
        return exit(q);
    }
    else if(command=="report"){
        //check what to do with file
        return report(q);
    }
    else if(command=="logout"){
        return logout(q);
    }
    else if(command=="summary"){
        return summary(q);
    }
    else{
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal conmand";
        return error;
    }

}

std::string StompProtocol::login(std::queue<std::string> q){
    if(q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal login conmand";
        return error;
    }
    std::string host = q.front();
    q.pop();
    if(q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal login conmand";
        return error;
    }
    std::string user = q.front();
    q.pop();
    if(q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal login conmand";
        return error;
    }
    std::string password = q.front();
    q.pop();
    if(!q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal login conmand";
        return error;
    }
    userName = user;
    std::string frame="";
    frame = frame + "CONNECT" + '\n';
    frame = frame + "accept-version:1.2" + '\n' + "host:stomp.cs.bgu.ac.il" + '\n' + "login:" + user +'\n' + "passcode:" + password + '\n' +'\n' +'\0';
    return frame;
}

std::string StompProtocol::join(std::queue<std::string> q){
    if(q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal join conmand";
        return error;
    }
    std::string topic = q.front();
    q.pop();
    if(!q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal join conmand";
        return error;
    }
    int subId=counterSub;
    if(topics.count(topic)==0){
        topics[topic] = counterSub;
        counterSub++;
    }
    else{
        subId=topics[topic];
    }
    std::string frame = "";
    frame = frame + "SUBSCRIBE" + '\n';
    frame = frame + "destination:" +topic + '\n' + "id:";
    frame = frame + std::to_string(subId) + '\n' + "receipt:" + std::to_string(counterReceipt) + '\n' + '\n' + '\0'; 
    counterReceipt++;
    return frame;
}

std::string StompProtocol::exit(std::queue<std::string> q){
    if(q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal exit conmand";
        return error;
    }
    std::string topic = q.front();
    q.pop();
    if(!q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal exit conmand";
        return error;
    }
    int subId = counterSub;
    if(topics.count(topic)==1){
        subId = topics[topic];
        topics.erase(topic);
    }
    std::string frame="";
    frame = frame + "UNSUBSCRIBE" + '\n';
    frame = frame + "id:" + std::to_string(subId) + '\n' + "receipt:" + std::to_string(counterReceipt) + '\n' + '\n' + '\0';
    counterReceipt++;
    return frame;

}

std::string StompProtocol::report(std::queue<std::string> q){
    if(q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal report conmand";
        return error;
    }
    std::string file = q.front();
    q.pop();
    if(!q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal report conmand";
        return error;
    }
    try{
        names_and_events ne = parseEventsFile(file); 
    }
    catch(...){
        std::string error = "";
        error = error + "ERROR" + '\n' + "something went wrong with this path";
        return error;
    }
    std::string frame = "";
    frame = frame + "SEND" + '\n' + "lot of reports";
    return frame;
}

std::string StompProtocol::logout(std::queue<std::string> q){
    if(!q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal logout conmand";
        return error;
    }
    std::string frame ="";
    frame = frame + "DISCONNECT" + '\n';
    frame = frame + "receipt:" + std::to_string(counterReceipt) + '\n' + '\n' + '\0';
    counterReceipt++;
    return frame;
}

std::string StompProtocol::summary(std::queue<std::string> q){
    if(q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal summary conmand";
        return error;
    }
    std::string gameName = q.front();
    q.pop();
    if(q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal summary conmand";
        return error;
    }
    std::string userName = q.front();
    q.pop();
    if(q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal summary conmand";
        return error;
    }
    std::string file = q.front();
    q.pop();
    if(!q.empty()){
        std::string error = "";
        error = error + "ERROR" + '\n' + "illegal summary conmand";
        return error;
    }
    std::pair<std::string, std::string> pair;
    pair.first= userName;
    pair.second = gameName;
    bool success = false;
    for(std::map<std::pair<std::string, std::string>, Summary>::iterator it = userGame.begin(); it!= userGame.end(); ++it){
        std::pair<std::string, std::string> key = it->first;  
        if(key==pair){
            Summary sum = it->second;
            success = createFile(file, sum);
            break;
        }
    }
    if(success){
        std::string frame = "";
        frame = frame + "SUCCESS" + '\n' + "your file is ready";
        return frame;
    }
    else{
        std::string error = "";
        error = error + "ERROR" + '\n' + "no summary for this userName and gameName";
        return error;
    }
}

void StompProtocol::update(std::pair<std::string, std::string> pair, Event event, std::string frame){
    bool found = false;
    for(std::map<std::pair<std::string, std::string>, Summary>::iterator it = userGame.begin(); it!= userGame.end(); ++it){
        std::pair<std::string, std::string> key = it->first;  
        if(key==pair){
            userGame[key].updateMainEvent(event);
            userGame[key].updateReports(event);
            found = true;
            break;
        }
    }
    if (!found){
        Summary summary(frame);
        summary.updateReports(event);
        userGame[pair] = summary;
    }
}

std::string StompProtocol::getUserName(){
    return userName;
}

bool StompProtocol::createFile(std::string fileName, Summary sum){
    std::fstream my_file;
    my_file.open(fileName, std::ofstream::out);
    if(!my_file){
        return false;
    }
    Event mainEvent = sum.getMainEvent();
    std::string output = "";
    output = output = mainEvent.get_team_a_name() + " vs " + mainEvent.get_team_b_name() + '\n';
    output = output + "Game stats:" +'\n' + "General stats:" + '\n';
    std::map<std::string, std::string> m = mainEvent.get_game_updates();
    for(std::map<std::string, std::string>::iterator it = m.begin(); it!= m.end(); ++it){
        output = output + it->first + ": " + it->second + '\n';
    }
    output = output + mainEvent.get_team_a_name() + " stats:" + '\n';
    std::map<std::string, std::string> a = mainEvent.get_team_a_updates();
    for(std::map<std::string, std::string>::iterator it = a.begin(); it!= a.end(); ++it){
        output = output + it->first + ": " + it->second + '\n';
    }
    output = output + mainEvent.get_team_b_name() + " stats:" + '\n';
    std::map<std::string, std::string> b = mainEvent.get_team_b_updates();
    for(std::map<std::string, std::string>::iterator it = b.begin(); it!= b.end(); ++it){
        output = output + it->first + ": " + it->second + '\n';
    }
    output = output + "Game event reports:" + '\n';
    std::queue<std::string> q = sum.getReports();
    while(!q.empty()){
        output = output + q.front() + '\n';
        q.pop();
    }
    my_file << output;
    my_file.close();
    return true;
}

