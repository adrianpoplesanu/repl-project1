package com.ad.utils;

public class AdStringsUtils {
    public static String unescapeString(String target) {
        return target
                .replace("\\\"", "\"")
                .replace("\\\'","\'")
                .replace("\\r", "\r")
                .replace("\\n", "\n");
    }
}
