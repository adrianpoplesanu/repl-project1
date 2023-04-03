package com.ad.threading;

import java.util.List;

public class ThreadPool {
    private List<AdWorker> runningWorkers;

    public int size() {
        return runningWorkers.size();
    }

    public void addWorker(AdWorker worker) {
        runningWorkers.add(worker);
    }

    public void removeWorker(AdWorker worker) {
        runningWorkers.remove(worker);
    }
}
