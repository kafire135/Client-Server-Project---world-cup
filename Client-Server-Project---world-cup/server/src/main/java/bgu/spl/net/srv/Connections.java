package bgu.spl.net.srv;

import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    void connect(int connectionId, String userName, String password, T message);

    void disconnect(int connectionId, T message, int receiptId);

    void subscribe(int connectionId, String topic, int subId, T message, int receiptId);

    void unsubscribe(int connectionId, int subId, T message, int receiptId);

    void send(int connectionId, String topic, T message);

    ConcurrentHashMap<Integer, Client<T>> getClients();

    int addMessage(T message);

    Object getLockTopics();

    ConcurrentHashMap<String, Vector<Integer>> getTopicsToClients();


}
