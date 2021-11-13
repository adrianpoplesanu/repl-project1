package com.ad.environment;

public class EnvironmentUtils {
    public static Environment newEnvironment() {
        Environment env = new Environment();
        return env;
    }

    public static Environment newEnclosedEnvironment(Environment o) {
        Environment env = new Environment(o);
        return env;
    }
}
