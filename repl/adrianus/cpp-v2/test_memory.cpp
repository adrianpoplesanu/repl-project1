#include <iostream>

int main(int argc, char* argv[]) {
    int *p, *m;
    p = new int(3);
    m = p;
    delete m;
    return 0;
}
