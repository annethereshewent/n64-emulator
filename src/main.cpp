#include <iostream>
#include "cpu/CPU.cpp"
#include <iterator>

int main() {
    CPU cpu;

    cpu.helloWorld();

    std::cout << "the value of register 1 is " << cpu.r[1] << "\n";
    std::cout << "the value of register 0 is (should be 0): " << cpu.r[0] << "\n";
    std::cout << "the PC is initialized to " << std::hex << cpu.pc << "\n";
    std::cout << "the size of rdram is " << cpu.bus.rdram.size() << "\n";

    return 0;
}