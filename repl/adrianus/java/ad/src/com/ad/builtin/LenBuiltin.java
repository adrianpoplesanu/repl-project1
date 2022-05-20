package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.*;

import java.util.List;

public class LenBuiltin extends AbstractBuiltin {
    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        AdObject obj;
        try {
            obj = args.get(0);
        } catch (Exception e) {
            return new AdErrorObject("no parameter provided to len builtin");
        }
        if (obj.getType() == ObjectTypeEnum.STRING) {
            AdStringObject stringObject = (AdStringObject) obj;
            AdIntegerObject result = new AdIntegerObject(stringObject.getValue().length());
            return result;
        }
        return null;
    }
}
