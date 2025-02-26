
#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <print>
#include <bit>
#include <chrono>
#include <thread>
#include "CPU.hpp"
#include "../bus/Bus.cpp"
#include "CPUInstructions.cpp"
#include "COP1Instructions.cpp"
#include "COP0Instructions.cpp"
#include "COP0.cpp"
#include "COP1.cpp"
#include "Scheduler.cpp"


CPU::CPU(): bus(*this), cop0(*this), cop1(*this) {
    r[0] = 0;

    pc = 0xBFC00000;
    nextPc = 0xBFC00004;

    instructions = {
        CPU::reserved,  // 0
        COP0::reserved, // 1
        CPU::j,         // 2
        CPU::jal,       // 3
        CPU::beq,       // 4
        CPU::bne,       // 5
        CPU::blez,      // 6
        CPU::bgtz,      // 7
        CPU::addi,      // 8
        CPU::addiu,     // 9
        CPU::slti,      // a
        CPU::sltiu,     // b
        CPU::andi,      // c
        CPU::ori,       // d
        CPU::xori,      // e
        CPU::lui,       // f
        COP0::reserved, // 10
        COP0::reserved, // 11
        COP0::reserved, // 12
        COP0::reserved, // 13
        CPU::beql,      // 14
        CPU::bnel,      // 15
        CPU::blezl,     // 16
        CPU::bgtzl,     // 17
        CPU::daddi,     // 18
        CPU::daddiu,    // 19
        CPU::ldl,       // 1a
        CPU::ldr,       // 1b
        COP0::reserved, // 1c
        COP0::reserved, // 1d
        COP0::reserved, // 1e
        COP0::reserved, // 1f
        CPU::lb,        // 20
        CPU::lh,        // 21
        CPU::lwl,       // 22
        CPU::lw,        // 23
        CPU::lbu,       // 24
        CPU::lhu,       // 25
        CPU::lwr,       // 26
        CPU::lwu,       // 27
        CPU::sb,        // 28
        CPU::sh,        // 29
        CPU::swl,       // 2a
        CPU::sw,        // 2b
        CPU::sdl,       // 2c
        CPU::sdr,       // 2d
        CPU::swr,       // 2e
        CPU::cache,     // 2f
        CPU::ll,        // 30
        COP1::lwc1,     // 31
        COP0::reserved, // 32
        COP0::reserved, // 33
        CPU::lld,       // 34
        COP1::ldc1,     // 35
        COP0::reserved, // 36
        CPU::ld,        // 37
        CPU::sc,        // 38
        COP1::swc1,     // 39
        COP0::reserved, // 3a
        COP0::reserved, // 3b
        CPU::scd,       // 3c
        COP1::sdc1,     // 3d
        COP0::reserved, // 3e
        CPU::sd,        // 3f
    };

    secondary = {
        CPU::sll,                 // 0
        COP0::reserved,           // 1
        CPU::srl,                 // 2
        CPU::sra,                 // 3
        CPU::sllv,                // 4
        COP0::reserved,           // 5
        CPU::srlv,                // 6
        CPU::srav,                // 7
        CPU::jr,                  // 8
        CPU::jalr,                // 9
        COP0::reserved,           // 10
        COP0::reserved,           // 11
        COP0::syscall,            // 12
        COP0::break_,             // 13
        COP0::reserved,           // 14
        CPU::sync,                // 15
        CPU::mfhi,                // 16
        CPU::mthi,                // 17
        CPU::mflo,                // 18
        CPU::mtlo,                // 19
        CPU::dsllv,               // 20
        COP0::reserved,           // 21
        CPU::dsrlv,               // 22
        CPU::dsrav,               // 23
        CPU::mult,                // 24
        CPU::multu,               // 25
        CPU::div,                 // 26
        CPU::divu,                // 27
        CPU::dmult,               // 28
        CPU::dmultu,              // 29
        CPU::ddiv,                // 30
        CPU::ddivu,               // 31
        CPU::add,                 // 32
        CPU::addu,                // 33
        CPU::sub,                 // 34
        CPU::subu,                // 35
        CPU::and_,                // 36
        CPU::or_,                 // 37
        CPU::xor_,                // 38
        CPU::nor,                 // 39
        COP0::reserved,           // 40
        COP0::reserved,           // 41
        CPU::slt,                 // 42
        CPU::sltu,                // 43
        CPU::dadd,                // 44
        CPU::daddu,               // 45
        CPU::dsub,                // 46
        CPU::dsubu,               // 47
        CPU::tge,                 // 48
        CPU::tgeu,                // 49
        CPU::tlt,                 // 50
        CPU::tltu,                // 51
        CPU::teq,                 // 52
        COP0::reserved,           // 53
        CPU::tne,                 // 54
        COP0::reserved,           // 55
        CPU::dsll,                // 56
        COP0::reserved,           // 57
        CPU::dsrl,                // 58
        CPU::dsra,                // 59
        CPU::dsll32,              // 60
        COP0::reserved,           // 61
        CPU::dsrl32,              // 62
        CPU::dsra32,              // 63
    };

    registerInstructions = {
        CPU::bltz,                 // 0
        CPU::bgez,                 // 1
        CPU::bltzl,                // 2
        CPU::bgezl,                // 3
        COP0::reserved,            // 4
        COP0::reserved,            // 5
        COP0::reserved,            // 6
        COP0::reserved,            // 7
        CPU::tgei,                 // 8
        CPU::tgeiu,                // 9
        CPU::tlti,                 // 10
        CPU::tltiu,                // 11
        CPU::teqi,                 // 12
        COP0::reserved,            // 13
        CPU::tnei,                 // 14
        COP0::reserved,            // 15
        CPU::bltzal,               // 16
        CPU::bgezal,               // 17
        CPU::bltzall,              // 18
        CPU::bgezall,              // 19
        COP0::reserved,            // 20
        COP0::reserved,            // 21
        COP0::reserved,            // 22
        COP0::reserved,            // 23
        COP0::reserved,            // 24
        COP0::reserved,            // 25
        COP0::reserved,            // 26
        COP0::reserved,            // 27
        COP0::reserved,            // 28
        COP0::reserved,            // 29
        COP0::reserved,            // 30
        COP0::reserved,            // 31
    };
}

void CPU::checkIrqs(bool usePreviousPc) {
    if ((bus.mips.mipsInterrupt.value & bus.mips.mipsMask.value) == 0 && cop0.pendingInterrupt) {
        cop0.pendingInterrupt = false;
        cop0.cause = 1 << 15;
    }

    if ((cop0.status.value & cop0.cause & 0xff00) != 0 &&
        (cop0.status.interruptEnable && !cop0.status.erl && !cop0.status.exl)
    ) {
        enterException(false);
    }
}

// TODO: fix this hack with usePreviousPc because it kind of sucks
void CPU::enterException(bool usePreviousPc) {
    if (!cop0.status.exl) {
        if (inDelaySlot) {
            cop0.epc = pc - 4;
            cop0.cause |= 1 << 31;
        } else {
            cop0.epc = usePreviousPc ? previousPc : pc;
            cop0.cause &= ~(1 << 31);
        }
    }

    cop0.status.exl = 1;

    discarded = false;

    if (!cop0.status.bev) {
        pc = 0x80000180;
        nextPc = 0x80000184;
    } else {
        pc = 0xbfc00200 + 0x180;
        nextPc = 0xbfc00200 + 0x184;
    }

    cop0.addCycles(2);
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

    std::vector<uint8_t> formattedRom;

    formattedRom.resize(fileSize);

    uint32_t cartridgeFormat = std::byteswap(*(uint32_t*)&rom[0]);

    switch (cartridgeFormat) {
        case 0x80371240:
            formattedRom = rom;
            break;
        case 0x37804012:
            for (int i = 0; i < rom.size(); i += 2) {
                // TODO: refactor this to something more efficient (possibly use memcpy?)
                uint16_t data = *(uint16_t*)&rom[i];

                for (int j = 0; j < 2; j++) {
                    int shift = 1 - j;

                    uint8_t byte = (data >> (shift * 8)) & 0xff;

                    formattedRom[i + j] = byte;
                }
            }
            break;
        case 0x40123780:
            for (int i = 0; i < rom.size(); i += 4) {
                // TODO: refactor this to something more efficient
                uint32_t data = *(uint32_t*)&rom[i];

                for (int j = 3; j >= 0; j--) {
                    int shift = 1 - j;

                    uint8_t byte = (data >> shift) & 0xff;

                    formattedRom[i + j] = byte;
                }
            }
            break;
    }

    bus.cartridge = formattedRom;

    memcpy(&bus.gameId, &bus.cartridge[0x3b], 3);
    bus.gameId[3] = 0;

    char header[3];

    memcpy(header, &bus.cartridge[0x3c], 2);
    header[2] = 0;

    if (strcmp(header, "ED") == 0) {
        uint8_t saveType = bus.cartridge[0x3f] >> 4;

        switch (saveType) {
            case 0:
                bus.saveType = -1;
                break;
            case 1:
                bus.saveType = EEPROM_4K;
                break;
            case 2:
                bus.saveType = EEPROM_16K;
                break;
            case 3:
                bus.saveType = -1;
                std::cout << "TODO: implement SRAM save type\n";
                break;
            case 5:
                bus.saveType = -1;
                std::cout << "TODO: implement flash save type\n";
                break;
            default:
                std::cout << "unknown save type detected: " << std::dec << saveType << "\n";
                exit(1);
                break;
        }
    } else {
        // TODO: actually determine this
        bus.saveType = EEPROM_4K;
    }
}

void CPU::step() {
    uint32_t opcode = (pc & 0x20000000) != 0 ? bus.memRead32(pc) : bus.readInstructionCache(pc);
    uint32_t command = opcode >> 26;

    previousPc = pc;

    if (!discarded) {
        pc = nextPc;
    } else {
        nextPc += 4;
        pc = nextPc;
    }

    discarded = false;
    nextPc += 4;

    bool oldDelaySlot = inDelaySlot;

    // if (!visited.contains(previousPc)) {
    //     uint32_t actualCommand = command;
    //     if (command == 0) {
    //         std::cout << "command is secondary\n";
    //         actualCommand = opcode & 0x3f;
    //     } else if (command == 16) {
    //         std::cout << "command is cop0\n";
    //         actualCommand = (opcode >> 21) & 0x1f;
    //     } else if (command == 17) {
    //         std::cout << "command is cop1\n";
    //         actualCommand = (opcode >> 21) & 0x1f;
    //     }
    //     std::cout << "pc = " << std::hex << previousPc << ", command = " << std::dec << actualCommand << "\n";
    //     // std::cout << "pc = " << std::hex << previousPc << "\n";
    //     visited.insert(previousPc);
    // }

    switch(command) {
        case 0:
            secondary[opcode & 0x3f](this, opcode);
            break;
        case 1:
            registerInstructions[(opcode >> 16) & 0x1f](this, opcode);
            break;
        case 16:
            cop0.instructions[(opcode >> 21) & 0x1f](this, opcode);
            break;
        case 17:
            cop1.instructions[(opcode >> 21) & 0x1f](this, opcode);
            break;
        case 18:
            std::cout << "not yet implemented: 18\n";
            exit(1);
            break;
        default:
            instructions[command](this, opcode);
            break;
    }

    cop0.addCycles(1);

    if (oldDelaySlot && inDelaySlot) {
        inDelaySlot = false;
    }

    while (scheduler.hasNextEvent(cop0.count)) {
        Event event = scheduler.getNextEvent();

        switch (event.eventType) {
            case VideoInterrupt:
                rdp_update_screen();
                bus.rdp.frameFinished = true;
                bus.setInterrupt(VI_INTERRUPT_FLAG);

                scheduler.addEvent(Event(VideoInterrupt, cop0.count + bus.videoInterface.delay));
                break;
            case PIFExecuteCommand:
                bus.pif.executeCommand();

                bus.serialInterface.status.dmaBusy = 0;
                bus.serialInterface.status.ioBusy = 0;
                bus.serialInterface.status.interrupt = 1;

                bus.setInterrupt(SI_INTERRUPT_FLAG);
                break;
            case RspDmaPop:
                bus.rsp.popDma();
                break;
            case RunRspPc:
                bus.rsp.restartRsp();
                break;
            case PIDma:
                bus.finishPiDma();
                break;
            case CompareCount:
                cop0.pendingInterrupt = true;

                scheduler.addEvent(Event(CompareCount, scheduler.getTimeToNext() + 0xffffffff));

                checkIrqs();
                break;
            case SIDma:
                if (bus.serialInterface.dir == DmaDirection::Write) {
                    bus.pif.executeCommand();
                } else {
                    bus.serialInterface.handleDma(bus);
                }

                bus.serialInterface.dir = DmaDirection::None;

                bus.serialInterface.status.dmaBusy = 0;
                bus.serialInterface.status.ioBusy = 0;
                bus.serialInterface.status.interrupt = 1;

                bus.setInterrupt(SI_INTERRUPT_FLAG);
                break;
            case AIDma:
                if (bus.audioInterface.lastRead != 0) {
                    bus.audioInterface.lastRead = 0;

                    // TODO: actually play audio!
                }

                bus.audioInterface.popDma();
                break;
            case RDPEvent:
                bus.rdp.status.gclk = 0;
                bus.rdp.status.cmdBusy = 0;
                bus.rdp.status.pipeBusy = 0;

                bus.setInterrupt(DP_INTERRUPT_FLAG);
                break;
            case NoEvent:
                std::println("should never happen");
                exit(1);
                break;
        }
    }
}

void CPU::fastForwardAbsoluteLoop(uint64_t target) {
    if (pc == target && bus.memRead32(pc, false, true) == 0) {
        cop0.count = scheduler.getTimeToNext();
    }
}

void CPU::fastForwardRelativeLoop(int16_t amount) {
    if (amount == -4 && bus.memRead32(pc, false, true) == 0) {
        cop0.count = scheduler.getTimeToNext();
    }
}

void CPU::limitFps() {
    auto p1 = std::chrono::system_clock::now();

    uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(
        p1.time_since_epoch()).count();

    if (timestamp != 0) {
        uint64_t diff = time - timestamp;

        if (diff < FPS_INTERVAL) {
            std::this_thread::sleep_for(std::chrono::milliseconds(FPS_INTERVAL - diff));
        }
    }

    p1 = std::chrono::system_clock::now();

    timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        p1.time_since_epoch()).count();;
}

uint32_t CPU::getImmediate(uint32_t instruction) {
    return instruction & 0xffff;
}

uint64_t CPU::getSignedImmediate(uint32_t instruction) {
    return (int16_t)(int64_t)(uint64_t)(instruction & 0xffff);
}

uint32_t CPU::getRs(uint32_t instruction) {
    return (instruction >> 21) & 0x1f;
}

uint32_t CPU::getRt(uint32_t instruction) {
    return (instruction >> 16) & 0x1f;
}

uint32_t CPU::getRd(uint32_t instruction) {
    return (instruction >> 11) & 0x1f;
}

uint32_t CPU::shiftAmount(uint32_t instruction) {
    return (instruction >> 6) & 0x1f;
}

uint32_t CPU::getFd(uint32_t instruction) {
    return (instruction >> 6) & 0x1f;
}