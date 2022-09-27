package com.ad.environment;

public class EnvironmentUtils {
    public static Environment newEnvironment() {
        Environment env = new Environment();
        return env;
    }

    public static Environment newEnclosedEnvironment(Environment outer) {
        Environment env = new Environment(outer);
        return env;
    }

    public static Environment newEnclosedEnvironment(Environment outer, Environment bootstrap) {
        Environment env = new Environment(outer, bootstrap);
        return env;
    }
}
