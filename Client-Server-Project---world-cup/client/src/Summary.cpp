#include "../include/Summary.h"

Summary::Summary() : mainEvent(), reports() {}

Summary::Summary(const std::string & frame_body) : mainEvent(frame_body), reports() {}

void Summary::updateMainEvent(Event& event){
    std::map<std::string, std::string> m = event.get_game_updates();
    for(std::map<std::string, std::string>::iterator it = m.begin(); it!= m.end(); ++it){
        std::string key = it->first;
        mainEvent.set_game_updates(it->first, it->second);
    }
    std::map<std::string, std::string> a = event.get_team_a_updates();
    for(std::map<std::string, std::string>::iterator it = a.begin(); it!= a.end(); ++it){
        std::string key = it->first;
        mainEvent.set_team_a_updates(it->first, it->second);
    }
    std::map<std::string, std::string> b = event.get_team_b_updates();
    for(std::map<std::string, std::string>::iterator it = b.begin(); it!= b.end(); ++it){
        std::string key = it->first;
        mainEvent.set_team_b_updates(it->first, it->second);
    }
} 


void Summary::updateReports(Event& event){
    std::string header = "";
    std::string time = std::to_string(event.get_time());
    header = header + time + " - " + event.get_name() + ":";
    reports.push(header);
    reports.push("");
    reports.push(event.get_discription());
}

Event Summary::getMainEvent(){
    return mainEvent;
}

std::queue<std::string> Summary::getReports(){
    return reports;
}