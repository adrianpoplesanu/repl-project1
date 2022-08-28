package com.ad.utils;

import com.ad.evaluator.Evaluator;
import com.ad.objects.AdObject;

import java.io.IOException;
import java.net.ServerSocket;

public class SocketUtils {
    public static AdObject startServerSocket(int port, boolean isActive, boolean isForever) throws IOException {
        ServerSocket serverSocket = null;
        serverSocket = new ServerSocket(port);
        if (isForever) {
            while (true) {
                //... always
            }
        } else {
            //... just once
        }
        return Evaluator.NULLOBJECT;
    }
}
