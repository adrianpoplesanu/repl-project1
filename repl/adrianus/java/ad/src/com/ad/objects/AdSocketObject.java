package com.ad.objects;

public class AdSocketObject extends AdObject {
    private String name;
    private int port;
    private boolean isActive;
    private boolean isForever;
    private boolean isClient;
    private boolean isServer;

    public AdSocketObject(String name, int port, boolean isActive, boolean isForever, boolean isClient, boolean isServer) {
        type = ObjectTypeEnum.SOCKET;
        this.name = name;
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

    @Override
    public String inspect() {
        return "<socket object at address 0x" + Integer.toHexString(this.hashCode()) + ">";
    }

    @Override
    public void print() {

    }

    @Override
    public String hash() {
        return null;
    }
}
