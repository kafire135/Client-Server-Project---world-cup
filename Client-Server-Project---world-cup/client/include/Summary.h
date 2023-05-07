#pragma once
#include "../include/event.h"
#include<queue>
#include <string>

class Summary{

private:
    Event mainEvent;
    std::queue<std::string> reports;

public:
    Summary(const std::string & frame_body);
    Summary();
    void updateMainEvent(Event& event);
    void updateReports(Event& event);
    Event getMainEvent();
    std::queue<std::string> getReports();
};