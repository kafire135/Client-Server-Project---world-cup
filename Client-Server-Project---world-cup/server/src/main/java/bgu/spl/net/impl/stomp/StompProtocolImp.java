package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.LinkedList;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class StompProtocolImp implements StompMessagingProtocol<String> {

    private boolean shouldTerminate = false;
    private int connectionId;
    private Connections<String> connections;
    private int receiptId;

    @Override
    public void start(int connectionId, Connections<String> connections){
        this.connectionId = connectionId;
        this.connections = connections;
    }
    
    @Override
    public void process(String message){
        if(!shouldTerminate){
            String command = message.substring(0, message.indexOf('\n'));
            int messageId = connections.addMessage(message);
            receiptId = messageId;
            if (command.equals("CONNECT")){
                try{
                    LinkedList<String> headers = new LinkedList<>();
                    headers.add("accept-version");
                    headers.add("host");
                    headers.add("login");
                    headers.add("passcode");
                    HashMap<String, String> map = createHeaders(message, headers);
                    String userName = map.get("login");
                    String password = map.get("passcode");
                    connections.connect(connectionId,userName,password,message);
                    String m = StompFrame.createConnected();
                    connections.send(connectionId, m);
                }
                catch(IllegalArgumentException e){
                String m = StompFrame.createError(messageId, "invalid connect received", message, e.getMessage());
                connections.send(connectionId, m);
                connections.disconnect(connectionId);
                shouldTerminate = true;
                }
            }
        
            else if (command.equals("DISCONNECT")){
                try{
                    LinkedList<String> headers = new LinkedList<>();
                    headers.add("receipt");
                    createHeaders(message, headers);
                    connections.disconnect(connectionId, message, receiptId);
                    String m = StompFrame.createReceipt(receiptId);
                    connections.send(connectionId, m);
                    connections.disconnect(connectionId);
                }
                catch(IllegalArgumentException e){
                    String m = StompFrame.createError(receiptId, "invalid disconnet received", message, e.getMessage());
                    connections.send(connectionId, m);
                    connections.disconnect(connectionId);
                }
                shouldTerminate = true;            
            }

            else if (command.equals("SUBSCRIBE")){
                try{
                    LinkedList<String> headers = new LinkedList<>();
                    headers.add("destination");
                    headers.add("id");
                    headers.add("receipt");
                    HashMap<String, String> map = createHeaders(message, headers);
                    String topic = map.get("destination");
                    String subId = map.get("id");
                    connections.subscribe(connectionId, topic, Integer.parseInt(subId), message, receiptId);
                    if(map.containsKey("receipt")){
                        String m = StompFrame.createReceipt(receiptId);
                        connections.send(connectionId, m);
                    }
                }
                catch(Exception e){
                    String m = StompFrame.createError(receiptId, "invalid subscribe received", message, e.getMessage());
                    connections.send(connectionId, m);
                    connections.disconnect(connectionId);
                    shouldTerminate = true;
                }       
            }

            else if (command.equals("UNSUBSCRIBE")){
                try{
                    LinkedList<String> headers = new LinkedList<>();
                    headers.add("id");
                    headers.add("receipt");
                    HashMap<String, String> map = createHeaders(message, headers);
                    String subId = map.get("id");
                    connections.unsubscribe(connectionId, Integer.parseInt(subId), message, receiptId);
                    if(map.containsKey("receipt")){
                        String m = StompFrame.createReceipt(receiptId);
                        connections.send(connectionId, m);
                    }
                }
                catch(Exception e){
                    String m = StompFrame.createError(receiptId, "invalid unsubscribe received", message, e.getMessage());
                    connections.send(connectionId, m);
                    connections.disconnect(connectionId);
                    shouldTerminate = true;
                }       
            }

            else if (command.equals("SEND")){
                try{
                    LinkedList<String> headers = new LinkedList<>();
                    headers.add("destination");
                    HashMap<String, String> map = createHeaders(message, headers);
                    String topic = map.get("destination");
                    connections.send(connectionId, topic, message);
                    String m = StompFrame.createMessageOne(messageId, topic, message);
                    synchronized(connections.getLockTopics()){
                        for (int id : connections.getTopicsToClients().get(topic)) {
                            int subId = connections.getClients().get(id).getTopicsToSub().get(topic);
                            String m2 = StompFrame.createMessageTwo(subId,m);
                            try{
                                connections.send(id, m2);           
                            }
                            catch(IllegalArgumentException e){}
                        }
                    }
                    if(map.containsKey("receipt")){
                        String m3 = StompFrame.createReceipt(receiptId);
                        connections.send(connectionId, m3);
                    }  
                }
                catch(IllegalArgumentException e){
                    String m = StompFrame.createError(receiptId, "invalid send received", message, e.getMessage());
                    connections.send(connectionId, m);
                    connections.disconnect(connectionId);
                    shouldTerminate = true;
                }      
            }

            else{
                String m = StompFrame.createError(receiptId, "invalid command", message, "illegal command name");
                connections.send(connectionId, m);
                connections.disconnect(connectionId);
                shouldTerminate = true;
            }
        }
        
    }
	
    @Override
    public boolean shouldTerminate(){
        return shouldTerminate;

    }

    public HashMap<String, String> createHeaders(String message, LinkedList<String>headers){
        HashMap<String, String> map = new HashMap<>();
        String[] arr = message.split("\n");
        int i = 1;
        while (i<arr.length && !arr[i].equals("")){
            int index = arr[i].indexOf(":");
            String key = arr[i].substring(0, index);
            if(!headers.contains(key)){
                throw new IllegalArgumentException("the header " + key + " is illegal");
            }
            if(map.containsKey(key)){
                throw new IllegalArgumentException("the header " + key + " exists twice");
            }
            String value = arr[i].substring(index+1);
            map.put(key, value);
            if(key.equals("receipt")){
                try{
                    receiptId = Integer.parseInt(value);
                }
                catch(NumberFormatException e){
                    throw new IllegalArgumentException("receipt id is not integer");
                }
            }
            i++;
        }
        if(map.size()==headers.size() || (map.size()==headers.size()-1) && headers.contains("receipt")){
            return map;
        }
        throw new IllegalArgumentException("there are missing headers");
    }
    
}
