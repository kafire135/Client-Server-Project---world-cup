package bgu.spl.net.impl.stomp;

import java.util.LinkedList;

public class StompFrame {

    public static String createConnected(){
        String command = "CONNECTED";
        String version = "version:1.2";
        String body = "";
        LinkedList<String> list = new LinkedList<>();
        list.add(command);
        list.add(version);
        list.add(body);
        return serialized(list);
        
    }

    public static String createReceipt(int receiptId){
        String command = "RECEIPT";
        String receipt = "receipt-id:" + receiptId;
        String body = "";
        LinkedList<String> list = new LinkedList<>();
        list.add(command);
        list.add(receipt);
        list.add(body);
        return serialized(list);
        
    }

    public static String createMessageOne(int msgId, String dest, String msg){
        String messageId = "message-id:" +msgId;
        String destination = "destination:" + dest;
        String enter = "";        
        LinkedList<String> list = new LinkedList<>();
        list.add(messageId);
        list.add(destination);
        list.add(enter);
        String[] headers = msg.split("\n");
        for (int i=3; i<headers.length; i++){
            list.add(headers[i]);
        }
        list.add(enter);
        return serialized(list);
    }

    public static String createMessageTwo(int subId, String msg){
        String command = "MESSAGE";
        String sub = "subscription:" + subId;
        msg = command + '\n' + sub + '\n' + msg;
        return msg;
    }

    public static String createError(int receiptId, String title, String message, String description){
        String command = "ERROR";
        String receipt = "receipt-id: " + receiptId;
        String messageTitle = "message: " + title;
        String enter = "";
        String desc = "The message:";
        String line = "-----";
        String body = message;
        String messageDesc = description;
        LinkedList<String> list = new LinkedList<>();
        list.add(command);
        list.add(receipt);
        list.add(messageTitle);
        list.add(enter);
        list.add(desc);
        list.add(line);
        list.add(body);
        list.add(line);
        list.add(messageDesc);
        return serialized(list);

    }

    public static String serialized(LinkedList<String> list){
        String output = "";
        for (String line : list) {
            output = output + line + "\n";
        }
        return output;
    }
    
}
