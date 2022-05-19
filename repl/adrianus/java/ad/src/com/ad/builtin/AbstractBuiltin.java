package com.ad.builtin;

import java.util.ArrayList;
import java.util.List;

public abstract class AbstractBuiltin implements BuiltinInterface {
    protected List<Integer> acceptedParameterSize = new ArrayList<>();

    public AbstractBuiltin() {
    }
}
