package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;
import com.ad.objects.ObjectTypeEnum;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.List;

public class SysSystemBuiltin extends AbstractBuiltin {
    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        // run a system command and print the output...
        boolean isWindows = System.getProperty("os.name")
                .toLowerCase().startsWith("windows");
        String result = "";
        AdObject command = args.get(0);
        if (command.getType() == ObjectTypeEnum.STRING) {
            String bash_command = ((AdStringObject) command).getValue();
            ProcessBuilder processBuilder = new ProcessBuilder();
            if (isWindows) {
                // TODO: test this on windows
                processBuilder.command("cmd.exe", "/c", bash_command);
            } else {
                processBuilder.command("bash", "-c", bash_command);
            }
            try {

                Process process = processBuilder.start();

                StringBuilder output = new StringBuilder();

                BufferedReader reader = new BufferedReader(
                        new InputStreamReader(process.getInputStream()));

                String line;
                while ((line = reader.readLine()) != null) {
                    output.append(line + "\n");
                }

                int exitVal = process.waitFor();
                if (exitVal == 0) {
                    //System.out.println("Success!");
                    //System.out.println(output);
                    //System.exit(0);
                    result = output.toString();
                } else {
                    //abnormal...
                }

            } catch (IOException e) {
                e.printStackTrace();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        return new AdStringObject(result);
    }
}
