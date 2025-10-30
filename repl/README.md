Interpretors for Adrianus

* built in c++
* built in python

How to run make commands in Windows:

- find where make is installed
- run the make executable found at step 1 with params: all, clean-win, eg:
    "D:\Program Files\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin\mingw32-make.exe" all
    "D:\Program Files\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin\mingw32-make.exe" clean-win

Important! Running valgrind will save your life. After installing it with "sudo apt install valgrinf" or whatever, run this:
valgrind --tool=memcheck --leak-check=full ./main

Important! VSCode Ad syntax highlight
cd /cpp
vsce package
code --install-extension adlang-0.0.1.vsix

files:
/adrianus/cpp/package.json
/adrianus/cpp/language-configuration.json
/adrianus/cpp/syntaxes/adlang.tmLanguage.json
are used to generate adlang-0.0.1.vsix VSC extension for ad code highlighting
