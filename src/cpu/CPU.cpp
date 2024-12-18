#include "CPU.hpp"
#include <iostream>
#include <fstream>

using namespace std;

void CPU::set(int i, uint64_t val) {
    if (i != 0) {
        r[i] = val;
    }
}

CPU::CPU() {
    // set registers to initial values, per https://n64.readthedocs.io/index.html#simulating-the-pif-rom
    r[0] = 0;

    pc = 0xBFC00000;
}

void CPU::loadRom(std::string filename) {
    std::ifstream file(filename, std::ios::binary);

    // get its size:
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<uint8_t> rom;
    rom.resize(fileSize);

    file.read(reinterpret_cast<char*>(rom.data()), fileSize);

    // copy first 0x1000 bytes of rom to sp dmem
}

void CPU::step() {
    uint32_t opcode = bus.memRead32(pc);

    std::cout << "received opcode " << std::hex << opcode << "\n";

    pc += 4;
}