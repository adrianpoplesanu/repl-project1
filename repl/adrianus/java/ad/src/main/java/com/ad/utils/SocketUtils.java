package com.ad.utils;

import com.ad.evaluator.Evaluator;
import com.ad.objects.AdObject;
import com.ad.objects.AdSocketObject;
import com.ad.objects.AdStringObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class SocketUtils {

    public static AdObject createServer(AdSocketObject socketObject) throws IOException {
        socketObject.setServerSocket(new ServerSocket(socketObject.getPort()));
        return Evaluator.NULLOBJECT;
    }

    public static AdObject createClient(AdSocketObject socketObject) throws IOException {
        socketObject.setClientSocket(new Socket(socketObject.getHost(), socketObject.getPort()));
        return Evaluator.NULLOBJECT;
    }

    public static AdObject accept(AdSocketObject socketObject) throws IOException {
        ServerSocket serverSocket = socketObject.getServerSocket();
        Socket clientSocket;

        clientSocket = serverSocket.accept();

        AdSocketObject adClientSocket = new AdSocketObject("client", "localhost", 5003, true, false, true, false);
        adClientSocket.setClientSocket(clientSocket);
        return adClientSocket;
    }

    public static AdObject send(AdSocketObject socketObject, AdObject obj) throws IOException {
        Socket clientSocket = socketObject.getClientSocket();
        PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);

        String message = ((AdStringObject) obj).getValue();
        out.print(AdStringsUtils.unescapeString(message));
        out.flush();

        return Evaluator.NULLOBJECT;
    }

    public static AdObject read(AdSocketObject socketObject) throws IOException {
        Socket clientSocket = socketObject.getClientSocket();
        BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

        String message = "";
        while(!in.ready()) {
            try {
                Thread.sleep(1);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
        while(in.ready()) {
            message += (char)in.read();
        }
        return new AdStringObject(message);
    }

    public static void close(AdSocketObject socketObject) throws IOException {
        if (socketObject.getPrintWriter() != null) {
            socketObject.getPrintWriter().close();
        }
        if (socketObject.getBufferedReader() != null) {
            socketObject.getBufferedReader().close();
        }
        if (socketObject.getServerSocket() != null) {
            socketObject.getServerSocket().close();
        }
        if (socketObject.getClientSocket() != null) {
            socketObject.getClientSocket().close();
        }
    }
}
