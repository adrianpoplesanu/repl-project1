package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdIntegerObject;
import com.ad.objects.AdListObject;
import com.ad.objects.AdNullObject;
import com.ad.objects.AdObject;

import java.util.ArrayList;
import java.util.List;

import static com.ad.evaluator.Evaluator.NULLOBJECT;

public class ListBuiltin extends AbstractBuiltin {
    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        if (args.size() == 0) {
            List<AdObject> elements = new ArrayList<>();
            return new AdListObject(elements);
        }
        if (args.size() == 1) {
            List<AdObject> elements = new ArrayList<>();
            int size = ((AdIntegerObject) args.get(0)).getValue();
            for (int i = 0; i < size; i++) elements.add(NULLOBJECT);
            AdListObject listObject = new AdListObject(elements);
            return listObject;
        }
        if (args.size() == 2) {
            List<AdObject> elements = new ArrayList<>();
            int size = ((AdIntegerObject) args.get(0)).getValue();
            AdObject defaultObject = args.get(1);
            for (int i = 0; i < size; i++) elements.add(defaultObject.copy());
            AdListObject listObject = new AdListObject(elements);
            return listObject;
        }
        return null;
    }
}
