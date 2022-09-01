package com.ad.environment;

import com.ad.objects.AdObject;

public interface EnvironmentInterface {
    AdObject get(String key);
    void set(String key, AdObject adObject);
    boolean check(String key);
    void setLocalParam(String key, AdObject adObject);
    Environment getOuter();
    void setOuter(Environment outer);
}
