#include "../include/Task.h"

Task::Task(ConnectionHandler& handler, std::mutex& mutex, StompProtocol& protocol, std::string login) : _handler(handler), _mutex(mutex), _protocol(protocol), terminate(false), disconnected(false), lastCommand(""), _login(login)  {}

void Task::keyBoardAction(){
    std::cout << "keyBoardAction starts" << std::endl;
    if (!_handler.sendLine(_login)) {
        std::cout << "Disconnected. Exiting...\n" << std::endl;
        terminate = true;
    }
    int len=_login.length();
    std::cout << "Sent " << len+1 << " bytes to server" << std::endl;
    while(!terminate){
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
		std::string line(buf);
		std::string frame = _protocol.createFrame(line);
        size_t pos = 0;
        std::string delimiter = "\n";
        pos=frame.find(delimiter);
        std::string token = frame.substr(0,pos);
        if(token=="ERROR"){
            std::cout << frame << std::endl;
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            terminate = true;
            frame ="";
            frame = frame + "DISCONNECT" + '\n';
            frame = frame + "receipt:" + std::to_string(0) + '\n' + '\n' + '\0';
            if (!_handler.sendLine(frame)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                terminate = true;
                break;
            }
            break;
        }
        else if(token=="DISCONNECT"){
            disconnected = true;
            lastCommand = "disconnect";
        }
        else if(token=="SUBSCRIBE"){
            lastCommand = "joined channel " + line.substr(5);
        }
        else if(token=="UNSUBSCRIBE"){
            lastCommand = "Exited channel " +line.substr(5);
        }
		len=frame.length();
        if(terminate){
            break;
        }
        if (token=="SUCCESS"){
            std::cout << frame << std::endl;
        }
        else{
            if(token=="SEND"){
                names_and_events ne = parseEventsFile(line.substr(7));
                std::string destination = "";
                destination = destination + "destination:" + ne.team_a_name + "_" + ne.team_b_name;
                std::string enter = "";
                std::string userName = "user: " + _protocol.getUserName();
                std::vector<Event> events;
                int counter = 0;
                while(!ne.events.empty()){
                    events.push_back(ne.events.back());
                    ne.events.pop_back();
                    counter++;
                }
                for(int i=0; i<counter; i++){
                    Event event = events.back();
                    events.pop_back();
                    std::string body = "";
                    body = body + createBody(event);
                    std::string frameToSend = "";
                    frameToSend = frameToSend + token + '\n' + destination + '\n' + enter + '\n' + userName + '\n' + body + '\n' + '\0';
                    len = frameToSend.length();
                    if (!_handler.sendLine(frameToSend)) {
                        std::cout << "Disconnected. Exiting...\n" << std::endl;
                        terminate = true;
                        break;
                    }
                    std::cout << "Sent " << len+1 << " bytes to server" << std::endl;
                }
                if(terminate){
                    break;
                } 
            }
            else{
                if (!_handler.sendLine(frame)) {
                    std::cout << "Disconnected. Exiting...\n" << std::endl;
                    terminate = true;
                    break;
                }
		        // connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
                std::cout << "Sent " << len+1 << " bytes to server" << std::endl;
            }
                
        }
            
    }
    std::cout << "keyBoardAction terminated" << std::endl;
}

void Task::socketAction(){
    std::cout << "socketAction starts" << std::endl;
    while(!terminate){
        std::string answer;
        if (!_handler.getLine(answer)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            terminate = true;
            break;
        }        
		int len=answer.length();
        answer.resize(len-1);
        std::string delimiter = "\n";
        size_t pos=answer.find(delimiter);
        std::string token = answer.substr(0,pos);
        if (token == "CONNECTED"){
            std::cout << "Login successful\n" << std::endl;
        }
        if (token == "RECEIPT" && !terminate){
            if(lastCommand!="disconnect"){
                std::cout << lastCommand + '\n' << std::endl;
            }
            else{
                std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl << std::endl;
            }
        }
        if (token == "ERROR" || disconnected) {
            std::vector<std::string> v;
            std::string d = "\n";
            size_t p = 0;
            std::string t;
            while((p=answer.find(d))!= std::string::npos){
                t = answer.substr(0,p);
                v.push_back(t);
                answer.erase(0,p+d.length());
            }
            v.push_back(answer);
            std::cout << answer +'\n' << std::endl;
            terminate = true;
            break;
        }
        if (token == "MESSAGE"){
            std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl << std::endl;
            Event event(answer);
            std::string copy(answer);
            std::string userName = getUserName(copy);
            std::pair<std::string,std::string> pair;
            pair.first = userName;
            std::string gameName = "";
            gameName = gameName + event.get_team_a_name()+"_"+event.get_team_b_name();
            pair.second = gameName;
            _protocol.update(pair,event,answer);
        }
    }
    std::cout << "the socket is closed, if the client still running press any key and enter to close the client" << std::endl;

}

std::string Task:: getUserName (std::string frame){
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
    q.pop();
    q.pop();
    q.pop();
    q.pop();
    q.pop();
    std::string userName = q.front();
    userName = userName.substr(6);
    return userName;
}

std::string Task::createBody(Event event){
    std::string body = "";
    body = body + "team a: " + event.get_team_a_name() + '\n';
    body = body + "team b: " + event.get_team_b_name() + '\n';
    body = body + "event name: " + event.get_name() + '\n';
    body = body + "time: " + std::to_string(event.get_time()) + '\n';
    body = body + "general game updates: " + '\n';
    std::map<std::string, std::string> m = event.get_game_updates();
    for(std::map<std::string, std::string>::iterator it = m.begin(); it!= m.end(); ++it){
        body = body + '\t' + it->first + ": " + it->second + '\n';
    }
    body = body + "team a updates: " + '\n'; 
    std::map<std::string, std::string> a = event.get_team_a_updates();
    for(std::map<std::string, std::string>::iterator it = a.begin(); it!= a.end(); ++it){
        body = body + '\t' + it->first + ": " + it->second + '\n';
    }
    body = body + "team b updates: " + '\n'; 
    std::map<std::string, std::string> b = event.get_team_b_updates();
    for(std::map<std::string, std::string>::iterator it = b.begin(); it!= b.end(); ++it){
        body = body + '\t' + it->first + ": " + it->second + '\n';
    }
    body = body + "description:" + '\n';
    body = body + event.get_discription();
    return body;
}