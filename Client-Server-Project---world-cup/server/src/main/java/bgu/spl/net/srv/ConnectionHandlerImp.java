package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.StompMessagingProtocol;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.net.Socket;

public class ConnectionHandlerImp<T> implements Runnable, ConnectionHandler<T> {

    private final StompMessagingProtocol<T> protocol;
    private final MessageEncoderDecoder<T> encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private volatile boolean connected = true;


    public ConnectionHandlerImp(Socket sock, MessageEncoderDecoder<T> reader, StompMessagingProtocol<T> protocol, Connections<T> connections, int conncetionId) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
        Client<T> client = new Client<T>(conncetionId, this);
        connections.getClients().put(conncetionId, client);
        this.protocol.start(conncetionId, connections);
    }

    @Override
    public void run() {
        try (Socket sock = this.sock) { //just for automatic closing
            int read;

            in = new BufferedInputStream(sock.getInputStream());

            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) {
                T nextMessage = encdec.decodeNextByte((byte) read);
                if (nextMessage != null) {
                    System.out.println((String)nextMessage);
                    protocol.process(nextMessage);
                }
            }
            close();

        } catch (IOException ex) {
            ex.printStackTrace();
        }

    }

    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }

    @Override
    public void send(T msg) {
        if(msg!=null){
            try{
                sock.getOutputStream().write(encdec.encode(msg));
            }
            catch(IOException e){ throw new IllegalArgumentException("socket threw exception..."); }           
        }
    }
}
