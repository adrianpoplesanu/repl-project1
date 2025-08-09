package com.ad.objects;

import java.io.BufferedReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.List;

public class AdSocketObject extends AdObject {
    private String name;
    private String host;
    private int port;
    private boolean isActive;
    private boolean isForever;
    private boolean isClient;
    private boolean isServer;
    private ServerSocket serverSocket;
    private List<Socket> connections;
    private Socket clientSocket;
    private PrintWriter printWriter;
    private BufferedReader bufferedReader;

    public AdSocketObject(String name, String host, int port, boolean isActive, boolean isForever, boolean isClient, boolean isServer) {
        type = ObjectTypeEnum.SOCKET;
        this.name = name;
        this.host = host;
        this.port = port;
        this.isActive = isActive;
        this.isForever = isForever;
        this.isClient = isClient;
        this.isServer = isServer;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }

    public boolean isActive() {
        return isActive;
    }

    public void setActive(boolean active) {
        isActive = active;
    }

    public boolean isForever() {
        return isForever;
    }

    public void setForever(boolean forever) {
        isForever = forever;
    }

    public boolean isClient() {
        return isClient;
    }

    public void setClient(boolean client) {
        isClient = client;
    }

    public boolean isServer() {
        return isServer;
    }

    public void setServer(boolean server) {
        isServer = server;
    }

    public String getHost() {
        return host;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public ServerSocket getServerSocket() {
        return serverSocket;
    }

    public void setServerSocket(ServerSocket serverSocket) {
        this.serverSocket = serverSocket;
    }

    public Socket getClientSocket() {
        return clientSocket;
    }

    public void setClientSocket(Socket clientSocket) {
        this.clientSocket = clientSocket;
    }

    public PrintWriter getPrintWriter() {
        return printWriter;
    }

    public void setPrintWriter(PrintWriter printWriter) {
        this.printWriter = printWriter;
    }

    public BufferedReader getBufferedReader() {
        return bufferedReader;
    }

    public void setBufferedReader(BufferedReader bufferedReader) {
        this.bufferedReader = bufferedReader;
    }

    public List<Socket> getConnections() {
        return connections;
    }

    public void setConnections(List<Socket> connections) {
        this.connections = connections;
    }

    @Override
    public String inspect() {
        return "<socket object at address: 0x" + Integer.toHexString(this.hashCode()) + ">";
    }

    @Override
    public String repr() {
        return "<socket object at address: 0x" + Integer.toHexString(this.hashCode()) + ">";
    }

    @Override
    public void print() {

    }

    @Override
    public String hash() {
        return null;
    }
}
