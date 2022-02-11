package com.ad.bootstrap;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.List;
import java.util.StringJoiner;

public class Bootstrap {
    final File bootstrapFolder = new File("bootstrap");

    private List<File> bootstrapFiles;

    public Bootstrap() {
        bootstrapFiles = new ArrayList<>();
        for (final File fileEntry : bootstrapFolder.listFiles()) {
            if (!fileEntry.isDirectory()) {
                String source = "";
                try {
                    source = new String (Files.readAllBytes(fileEntry.toPath()));
                } catch (IOException e) {
                    source = "";
                }
                addBootstrap(source);
            }
        }
    }

    public void loadBootstrap() {
        addBootstrap("");
    }

    public void addBootstrap(String source) {
        //System.out.println(source);
    }
}
