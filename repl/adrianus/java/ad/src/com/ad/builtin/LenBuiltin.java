package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdIntegerObject;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;
import com.ad.objects.ObjectTypeEnum;

import java.util.ArrayList;

public class LenBuiltin implements BuiltinInterface {
    @Override
    public AdObject call(ArrayList<AdObject> args, Environment env) {
        AdObject obj = args.get(0);
        if (obj.getType() == ObjectTypeEnum.STRING) {
            AdStringObject stringObject = (AdStringObject) obj;
            AdIntegerObject result = new AdIntegerObject(stringObject.getValue().length());
            return result;
        }
        return null;
    }
}
