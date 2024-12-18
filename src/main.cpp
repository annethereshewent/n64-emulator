#include <iostream>
#include "cpu/CPU.cpp"
#include <iterator>

int main(int argc, char **argv) {
    CPU cpu;

    if (argc < 1) {
        std::cout << "Please specify a filename.\n";
        exit(1);
    }

    cpu.loadRom(argv[1]);

    while (true) {
        cpu.step();
    }

    return 0;
}