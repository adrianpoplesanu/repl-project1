package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;

import java.util.List;
import java.util.Scanner;

public class InputBuiltin implements BuiltinInterface {
    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        Scanner scanner = new Scanner(System.in);
        String line = scanner.nextLine();
        return new AdStringObject(line);
    }
}
