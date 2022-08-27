package com.ad.bootstrap;

import com.ad.ast.AstProgram;
import com.ad.environment.Environment;
import com.ad.evaluator.Evaluator;
import com.ad.parser.Parser;

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
        /*bootstrapFiles = new ArrayList<>();
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
        }*/
    }

    public void loadBootstrap(AstProgram program, Parser parser, Evaluator evaluator, Environment env) {
        for (final File fileEntry : bootstrapFolder.listFiles()) {
            if (!fileEntry.isDirectory()) {
                String source = "";
                try {
                    source = new String (Files.readAllBytes(fileEntry.toPath()));
                } catch (IOException e) {
                    source = "";
                }
                addBootstrap(program, parser, evaluator, env, source);
            }
        }
    }

    public void addBootstrap(AstProgram program, Parser parser, Evaluator evaluator, Environment env, String source) {
        parser.load(source);
        program.reset();
        parser.buildProgramStatements(program);
        evaluator.eval(program, env);
    }
}
