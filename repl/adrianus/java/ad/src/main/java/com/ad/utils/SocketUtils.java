package com.ad.utils;

import com.ad.evaluator.Evaluator;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class SocketUtils {
    public static AdObject listen(int port, boolean isActive, boolean isForever) throws IOException {
        ServerSocket serverSocket = null;
        serverSocket = new ServerSocket(port);

        String message = "aaa";

        if (isForever) {
            while (true) {
                //... always, isForever trebuie verificat dupa ce s-a stabilit conexiunea si pastrata, sau prins in socket.ad
            }
        } else {
            Socket clientSocket = serverSocket.accept();
            PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

            message = in.readLine();
            //System.out.println(message); // aici primesc date de la client
            //out.println("hello client"); // aici trimit date catre client
            out.println("HTTP/1.1 200 OK\r\n" +
                    "Server: AdServer\r\n" +
                    "Content-Type: text/html\r\n" +
                    "Content-Length: 24\r\n" +
                    "Connection: close\r\n" +
                    "\r\n" +
                    "this is an http response");
        }
        return new AdStringObject(message);
        //return Evaluator.NULLOBJECT;
    }

    public static AdObject send(String host, int port, boolean isActive, boolean isForever) throws IOException {
        Socket clientSocket = new Socket(host, port);
        PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
        BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

        out.println("GET / HTTP/1.1"); // aici trimit date catre server
        System.out.println(in.readLine()); // aici primesc date de la server
        // aici ar trebui verificat isForever, sau prins in socket.ad

        in.close();
        out.close();
        clientSocket.close();

        return Evaluator.NULLOBJECT;
    }

    public static void close() {
        // aici inchid un server sau un client care are isForever true
    }
}
