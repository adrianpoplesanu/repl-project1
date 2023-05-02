package com.ad.objects;

import com.ad.threading.AdWorker;

import java.util.ArrayList;
import java.util.List;

public class AdThreadObject extends AdObject {

    private String name;
    private AdObject callback;
    private Thread thread;
    private AdWorker worker;
    private ArrayList<AdObject> params;

    public AdThreadObject() {
        type = ObjectTypeEnum.THREAD;
    }

    public AdThreadObject(String name) {
        this();
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public AdObject getCallback() {
        return callback;
    }

    public void setCallback(AdObject callback) {
        this.callback = callback;
    }

    public Thread getThread() {
        return thread;
    }

    public void setThread(Thread thread) {
        this.thread = thread;
    }

    public AdWorker getWorker() {
        return worker;
    }

    public void setWorker(AdWorker worker) {
        this.worker = worker;
    }

    public ArrayList<AdObject> getParams() {
        return params;
    }

    public void setParams(ArrayList<AdObject> params) {
        this.params = params;
    }

    @Override
    public String inspect() {
        return null;
    }

    @Override
    public void print() {

    }

    @Override
    public String hash() {
        return null;
    }
}
