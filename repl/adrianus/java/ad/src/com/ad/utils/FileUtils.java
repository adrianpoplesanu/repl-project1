package com.ad.utils;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.stream.Stream;

public class FileUtils {
    public static String readFile(String path) {
        String content;
        try {
            content = readLineByLineJava8(path);
        } catch(Exception e) {
            content = "Error reading file. TODO: return either AdError or AdNull";
        }
        return content;
    }

    private static String readLineByLineJava8(String filePath)
    {
        StringBuilder contentBuilder = new StringBuilder();

        try (Stream<String> stream = Files.lines( Paths.get(filePath), StandardCharsets.UTF_8)) {
            stream.forEach(s -> contentBuilder.append(s).append("\n"));
        }
		catch (IOException e) {
            e.printStackTrace();
        }

        return contentBuilder.toString();
    }

    public static void test(String[] args) {
        readFile("/Users/adrianpoplesanu/Documents/git-projects/repl-project1/repl/adrianus/java/ad/examples/test16.ad");
    }
}
