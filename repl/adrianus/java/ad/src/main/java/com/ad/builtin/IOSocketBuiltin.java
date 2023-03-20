package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.*;

import java.util.List;

public class IOSocketBuiltin extends AbstractBuiltin {
    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        AdStringObject nameObject = (AdStringObject) args.get(0);
        AdStringObject hostObject = (AdStringObject) args.get(1);
        AdIntegerObject portObject = (AdIntegerObject) args.get(2);
        AdBooleanObject isActiveObject = (AdBooleanObject) args.get(3);
        AdBooleanObject isForeverObject = (AdBooleanObject) args.get(4);
        AdBooleanObject isClientObject = (AdBooleanObject) args.get(5);
        AdBooleanObject isServerObject = (AdBooleanObject) args.get(6);
        return new AdSocketObject(nameObject.getValue(),
                hostObject.getValue(),
                portObject.getValue(),
                isActiveObject.getValue(),
                isForeverObject.getValue(),
                isClientObject.getValue(),
                isServerObject.getValue());
    }
}
