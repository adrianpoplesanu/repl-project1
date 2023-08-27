package com.ad.threading;

import com.ad.environment.Environment;
import com.ad.evaluator.Evaluator;
import com.ad.objects.AdObject;
import com.ad.objects.AdThreadObject;
import com.ad.objects.ObjectTypeEnum;

import java.util.ArrayList;

public class AdWorker implements Runnable {
    private AdObject target;
    private AdObject rawThread;
    private Environment environment;

    private ArrayList<AdObject> params;

    public AdWorker(AdObject target, Environment environment, ArrayList<AdObject> params, AdObject rawThread) {
        this.target = target;
        this.environment = environment;
        this.params = params;
        this.rawThread = rawThread;
    }

    @Override
    public void run() {
        if (target.getType() == ObjectTypeEnum.FUNCTION) {
            Evaluator evaluator = new Evaluator();
            AdThreadObject threadObject = (AdThreadObject) rawThread;
            AdObject result = evaluator.applyFunction(target, params, environment);
            threadObject.setResult(result);
        }
    }
}
