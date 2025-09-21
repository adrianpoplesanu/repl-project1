#ifndef AD_VM_INSTRUCTIONS_H
#define AD_VM_INSTRUCTIONS_H

class Instructions {
public:
    std::vector<unsigned char> bytes;

    int size;
    unsigned char get(int);
    void add(unsigned char);
    void removeLast();
};

#endif