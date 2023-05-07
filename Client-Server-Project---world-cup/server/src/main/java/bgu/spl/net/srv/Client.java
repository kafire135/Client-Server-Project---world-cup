package bgu.spl.net.srv;

import java.util.NoSuchElementException;
import java.util.concurrent.ConcurrentHashMap;


public class Client<T> {

    private int connectionId;
    private String userName;
    private ConcurrentHashMap<String, Integer> topicsToSub;
    private ConcurrentHashMap<Integer, String> subTotopics;
    private ConnectionHandler<T> handler;

    public Client(int connectionId, ConnectionHandler<T> handler){
        this.connectionId = connectionId;
        this.userName = null;
        topicsToSub = new ConcurrentHashMap<>();
        subTotopics = new ConcurrentHashMap<>();
        this.handler = handler;
    }

    public String getUserName(){
        return userName;
    }

    public int getConnectionId(){
        return connectionId;
    }

    public ConnectionHandler<T> getConnectionHandler(){
        return handler;
    }

    public ConcurrentHashMap<Integer, String> getSubToTopics(){
        return subTotopics;
    }

    public ConcurrentHashMap<String, Integer> getTopicsToSub(){
        return topicsToSub;
    }

    public boolean isUserConnected(){
        return userName != null;
    }

    public void connectUser(String userName){
        this.userName = userName;
    }

    public void dissconnectUser(){
        this.userName = null;
    }

    public void subscribe(String topic, int subId){
        if(topicsToSub.containsKey(topic)){
            throw new IllegalArgumentException("client allready subscribed to this topic");
        }
        if(subTotopics.containsKey(subId)){
            throw new IllegalArgumentException("client allready has this subId");
        }
        topicsToSub.put(topic, subId);
        subTotopics.put(subId, topic);
    }

    public void unsubscribe(int subId){
        if(!subTotopics.containsKey(subId)){
            throw new NoSuchElementException("the subId doesn't exist");
        }
        String topic = subTotopics.get(subId);
        subTotopics.remove(subId);
        topicsToSub.remove(topic);
    }
    
}
