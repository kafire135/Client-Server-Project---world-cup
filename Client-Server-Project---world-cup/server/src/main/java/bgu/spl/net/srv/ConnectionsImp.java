package bgu.spl.net.srv;

import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;


public class ConnectionsImp<T> implements Connections<T> {

    private ConcurrentHashMap<String, User> users;
    private ConcurrentHashMap<Integer, Client<T>> clients;
    private ConcurrentHashMap<String, Vector<Integer>> topicsToClients;
    private ConcurrentHashMap<Integer, T> messages;
    private Object lockUsers;
    private Object lockClients;
    private Object lockTopics;
    private Object lockMessages;

    public ConnectionsImp(){
        users = new ConcurrentHashMap<>();
        clients = new ConcurrentHashMap<>();
        topicsToClients = new ConcurrentHashMap<>();
        messages = new ConcurrentHashMap<>();
        lockUsers = new Object();
        lockClients = new Object();
        lockTopics = new Object();
        lockMessages = new Object();
    }

    public ConcurrentHashMap<String, User> getUsers(){
        return users;
    }

    public ConcurrentHashMap<Integer, Client<T>> getClients(){
        return clients;
    }

    public ConcurrentHashMap<String, Vector<Integer>> getTopicsToClients(){
        return topicsToClients;
    }

    public ConcurrentHashMap<Integer, T> getMessages(){
        return messages;
    }

    public int addMessage(T message){
        synchronized(lockMessages){
            messages.put(messages.size(), message);
            return messages.size()-1;
        }
    }

    public Object getLockTopics(){
        return lockTopics;
    }

    public void connect(int connectionId, String userName, String password, T message){
        if(clients.get(connectionId).isUserConnected()){
            throw new IllegalArgumentException("The client is already logged in, log out before trying again");
        }
        synchronized(lockUsers){
            if(users.containsKey(userName)){
                User user = users.get(userName);
                if(!user.checkPassword(password)){
                    throw new IllegalArgumentException("Wrong password");
                }
                if(user.isConnected()){
                    throw new IllegalArgumentException("User already logged in");
                }
                user.connect();
                clients.get(connectionId).connectUser(userName);
                return;
            }
            else
            {
                if(userName==null || password==null){
                    throw new IllegalArgumentException("user name and password can't be null");
                }
                User user = new User(userName, password);
                user.connect();
                users.put(userName, user);
                clients.get(connectionId).connectUser(userName);
            }
        }
    }

    public void subscribe(int connectionId, String topic, int subId, T message, int receiptId){
        if(!clients.containsKey(connectionId)){
            throw new IllegalArgumentException("client doesn't exist");
        }
        Client<T> client = clients.get(connectionId);
        if(!client.isUserConnected()){
            throw new IllegalArgumentException("the client isn't connected");
        }
        if(client.getSubToTopics().containsKey(subId)){
            throw new IllegalArgumentException("subId allready exists for this client");
        }
        synchronized(lockTopics){
            if(!topicsToClients.containsKey(topic)){
                Vector<Integer> vec = new Vector<>();
                topicsToClients.put(topic, vec);
            }
            else if(topicsToClients.get(topic).contains(connectionId)){
                throw new IllegalArgumentException("client is allready subscribed to this topic");
            }
            client.subscribe(topic, subId);
            topicsToClients.get(topic).add(connectionId);
        }
    }

    public void unsubscribe(int connectionId, int subId, T message, int receiptId){
        if(!clients.containsKey(connectionId)){
            throw new IllegalArgumentException("client doesn't exist");
        }
        Client<T> client = clients.get(connectionId);
        if(!client.isUserConnected()){
            throw new IllegalArgumentException("the client isn't connected");
        }
        if(!client.getSubToTopics().containsKey(subId)){
            throw new IllegalArgumentException("no such subId for this client");
        }
        String topic = (String) client.getSubToTopics().get(subId);
        client.unsubscribe(subId);
        synchronized(lockTopics){
            topicsToClients.get(topic).removeElement(connectionId);
        }
    }
    

    @Override
    public boolean send(int connectionId, T msg) {
        synchronized(lockClients){
            ConnectionHandler<T> handler = clients.get(connectionId).getConnectionHandler();
            try{
                handler.send(msg);
            }
            catch(IllegalArgumentException e){ throw e;} 
        }
        return true;
    }

    @Override
    public void send(String channel, T msg) {}

    public void send(int connectionId, String topic, T message){
        if(!clients.containsKey(connectionId)){
            throw new IllegalArgumentException("client doesn't exist");
        }
        Client<T> client = clients.get(connectionId);
        if(!client.isUserConnected()){
            throw new IllegalArgumentException("the client isn't connected");
        }
        synchronized(lockTopics){
            if(!topicsToClients.containsKey(topic)){
                throw new IllegalArgumentException("topic doesn't exist");
            }
            if(!topicsToClients.get(topic).contains(connectionId)){
                throw new IllegalArgumentException("client is unsubscribed to this topic");
            }
        }
    }

    public void disconnect(int connectionId, T message, int receiptId){
        if(!clients.containsKey(connectionId)){
            throw new IllegalArgumentException("client doesn't exist");
        }
        if(!clients.get(connectionId).isUserConnected()){
            throw new IllegalArgumentException("the client is allready disconnected");
        }
    }

    @Override
    public void disconnect(int connectionId) {
        Client<T> client = clients.get(connectionId);
        for(int subId : client.getSubToTopics().keySet()){
            unsubscribe(connectionId, subId, null, -1);
        }
        if(client.getUserName()!=null){
            users.get(client.getUserName()).dissconnect();
            client.dissconnectUser();
        }
        clients.remove(connectionId);    
    }
  
}
