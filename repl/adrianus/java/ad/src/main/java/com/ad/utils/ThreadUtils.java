package com.ad.utils;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;
import com.ad.threading.AdWorker;

public class ThreadUtils {
    public static AdWorker generateAdWorker(AdObject object, Environment environment) {
        AdWorker adWorker = new AdWorker(object, environment);
        return adWorker;
    }

    public static Thread generateThread(AdWorker adWorker) {
        Thread thread = new Thread(adWorker);
        return thread;
    }
}
