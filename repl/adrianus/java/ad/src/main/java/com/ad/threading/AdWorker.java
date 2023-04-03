package com.ad.threading;

import com.ad.environment.Environment;
import com.ad.evaluator.Evaluator;
import com.ad.objects.AdObject;
import com.ad.objects.ObjectTypeEnum;

import java.util.ArrayList;

public class AdWorker implements Runnable {
    private AdObject target;
    private Environment environment;

    public AdWorker(AdObject target, Environment environment) {
        this.target = target;
        this.environment = environment;
    }

    @Override
    public void run() {
        if (target.getType() == ObjectTypeEnum.FUNCTION) {
            Evaluator evaluator = new Evaluator();
            ArrayList<AdObject> arguments = new ArrayList<>();
            evaluator.applyFunction(target, arguments, environment);
        }
    }
}
