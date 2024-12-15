#include "CPU.hpp"
#include <iostream>
using namespace std;

void CPU::helloWorld() {
    set(1, 0xffffffffffffffff);
    set(0, 0xffffffff);
}

void CPU::set(int i, uint64_t val) {
    r[i] = val;

    r[0] = 0;
}

CPU::CPU() {
    r[0] = 0;
    pc = 0xBFC00000;
}