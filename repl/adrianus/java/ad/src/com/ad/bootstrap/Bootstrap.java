package com.ad.bootstrap;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class Bootstrap {
    final File bootstrapFolder = new File("bootstrap");

    private List<File> bootstrapFiles;

    public Bootstrap() {
        bootstrapFiles = new ArrayList<>();
        for (final File fileEntry : bootstrapFolder.listFiles()) {
            if (!fileEntry.isDirectory()) {
                //System.out.println(fileEntry.getName());
            }
        }
    }

    public void loadBootstrap() {
        addBootstrap("");
    }

    public void addBootstrap(String source) {

    }
}
