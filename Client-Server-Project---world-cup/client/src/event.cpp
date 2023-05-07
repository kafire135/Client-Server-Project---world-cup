#include "../include/event.h"
#include "../include/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <queue>
using json = nlohmann::json;

Event::Event(std::string team_a_name, std::string team_b_name, std::string name, int time,
             std::map<std::string, std::string> game_updates, std::map<std::string, std::string> team_a_updates,
             std::map<std::string, std::string> team_b_updates, std::string discription)
    : team_a_name(team_a_name), team_b_name(team_b_name), name(name),
      time(time), game_updates(game_updates), team_a_updates(team_a_updates),
      team_b_updates(team_b_updates), description(discription)
{
}

Event::~Event()
{
}

const std::string &Event::get_team_a_name() const
{
    return this->team_a_name;
}

const std::string &Event::get_team_b_name() const
{
    return this->team_b_name;
}

const std::string &Event::get_name() const
{
    return this->name;
}

int Event::get_time() const
{
    return this->time;
}

const std::map<std::string, std::string> &Event::get_game_updates() const
{
    return this->game_updates;
}

const std::map<std::string, std::string> &Event::get_team_a_updates() const
{
    return this->team_a_updates;
}

const std::map<std::string, std::string> &Event::get_team_b_updates() const
{
    return this->team_b_updates;
}

const std::string &Event::get_discription() const
{
    return this->description;
}

void Event::set_game_updates(std::string key, std::string value){
    game_updates[key] = value;
}

void Event::set_team_a_updates(std::string key, std::string value){
    team_a_updates[key] = value;
}

void Event::set_team_b_updates(std::string key, std::string value){
    team_b_updates[key] = value;
}

Event::Event() : team_a_name(""), team_b_name(""), name(""), time(0), game_updates(), team_a_updates(), team_b_updates(), description(""){}

Event::Event(const std::string &frame_body) : team_a_name(""), team_b_name(""), name(""), time(0), game_updates(), team_a_updates(), team_b_updates(), description("")
{
    std::string frame(frame_body);
    std::queue<std::string> q;
    std::string delimiter = "\n";
    size_t pos = 0;
    std::string token;
    while((pos=frame.find(delimiter))!= std::string::npos){
        token = frame.substr(0,pos);
        q.push(token);
        frame.erase(0,pos+delimiter.length());
    }
    q.push(frame);
    for(int i=0; i<6; i++){
        q.pop();
    }
    std::string team_a = q.front();
    team_a_name = team_a.substr(8);
    q.pop();
    std::string team_b = q.front();
    team_b_name = team_b.substr(8);
    q.pop();
    std::string event_name = q.front();
    name = event_name.substr(12);
    q.pop();
    std::string s_time = q.front();
    s_time = s_time.substr(6);
    time = std::stoi(s_time);
    q.pop();
    q.pop();
    while(q.front()[0]=='\t'){
        std::string front = q.front();
        std::string del = ":";
        pos = 0;
        if((pos=front.find(del))!= std::string::npos){
            std::string first = front.substr(1,pos);
            front.erase(0,pos+delimiter.length());
            std::string second = front.substr(1);
            game_updates[first] = second;
        }
        q.pop();
    }
    q.pop();
    while(q.front()[0]=='\t'){
        std::string front = q.front();
        std::string del = ":";
        pos = 0;
        if((pos=front.find(del))!= std::string::npos){
            std::string first = front.substr(1,pos);
            front.erase(0,pos+delimiter.length());
            std::string second = front.substr(1);
            team_a_updates[first] = second;
        }
        q.pop();
    }
    q.pop();
    while(q.front()[0]=='\t'){
        std::string front = q.front();
        std::string del = ":";
        pos = 0;
        if((pos=front.find(del))!= std::string::npos){
            std::string first = front.substr(1,pos);
            front.erase(0,pos+delimiter.length());
            std::string second = front.substr(1);
            team_b_updates[first] = second;
        }
        q.pop();
    }
    q.pop();
    while(!q.empty()){
        description = description + q.front() + '\n';
        q.pop();
    }
}

names_and_events parseEventsFile(std::string json_path)
{
    std::ifstream f(json_path);
    json data = json::parse(f);

    std::string team_a_name = data["team a"];
    std::string team_b_name = data["team b"];

    // run over all the events and convert them to Event objects
    std::vector<Event> events;
    for (auto &event : data["events"])
    {
        std::string name = event["event name"];
        int time = event["time"];
        std::string description = event["description"];
        std::map<std::string, std::string> game_updates;
        std::map<std::string, std::string> team_a_updates;
        std::map<std::string, std::string> team_b_updates;
        for (auto &update : event["general game updates"].items())
        {
            if (update.value().is_string())
                game_updates[update.key()] = update.value();
            else
                game_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team a updates"].items())
        {
            if (update.value().is_string())
                team_a_updates[update.key()] = update.value();
            else
                team_a_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team b updates"].items())
        {
            if (update.value().is_string())
                team_b_updates[update.key()] = update.value();
            else
                team_b_updates[update.key()] = update.value().dump();
        }
        
        events.push_back(Event(team_a_name, team_b_name, name, time, game_updates, team_a_updates, team_b_updates, description));
    }
    names_and_events events_and_names{team_a_name, team_b_name, events};

    return events_and_names;
}