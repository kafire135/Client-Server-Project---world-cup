package bgu.spl.net.impl.stomp;

import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {

        // int port = Integer.parseInt(args[0].substring(0, args[0].indexOf(" ")));
        // String server = args[0].substring(args[0].indexOf(" ")+1);
        //mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="7777 tpc"

        int port = Integer.parseInt(args[0]);
        String server = args[1];
        if(server.equals("tpc")){

            Server.threadPerClient(
            port, //port
            () -> new StompProtocolImp(), //protocol factory
            LineMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();
        }

        else if(server.equals("reactor")){

            Server.reactor(
            Runtime.getRuntime().availableProcessors(),
            port, //port
            () -> new StompProtocolImp(), //protocol factory
            LineMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();
        }

        

        
    }
}
