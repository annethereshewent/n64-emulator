
#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <print>
#include <bit>
#include <chrono>
#include <thread>
#include <string>
#include "CPU.hpp"
#include "../bus/Bus.cpp"
#include "CPUInstructions.cpp"
#include "COP1Instructions.cpp"
#include "COP0Instructions.cpp"
#include "COP0.cpp"
#include "COP1.cpp"
#include "Scheduler.cpp"
#include "Disassembler.cpp"

CPU::CPU(): bus(*this), cop0(*this), cop1(*this) {
    r[0] = 0;

    pc = 0xBFC00000;
    nextPc = 0xBFC00004;

    instructions = {
        CPU::reserved,
        COP0::reserved,
        CPU::j,
        CPU::jal,
        CPU::beq,
        CPU::bne,
        CPU::blez,
        CPU::bgtz,
        CPU::addi,
        CPU::addiu,
        CPU::slti,
        CPU::sltiu,
        CPU::andi,
        CPU::ori,
        CPU::xori,
        CPU::lui,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        CPU::beql,
        CPU::bnel,
        CPU::blezl,
        CPU::bgtzl,
        CPU::daddi,
        CPU::daddiu,
        CPU::ldl,
        CPU::ldr,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        CPU::lb,
        CPU::lh,
        CPU::lwl,
        CPU::lw,
        CPU::lbu,
        CPU::lhu,
        CPU::lwr,
        CPU::lwu,
        CPU::sb,
        CPU::sh,
        CPU::swl,
        CPU::sw,
        CPU::sdl,
        CPU::sdr,
        CPU::swr,
        CPU::cache,
        CPU::ll,
        COP1::lwc1,
        COP0::reserved,
        COP0::reserved,
        CPU::lld,
        COP1::ldc1,
        COP0::reserved,
        CPU::ld,
        CPU::sc,
        COP1::swc1,
        COP0::reserved,
        COP0::reserved,
        CPU::scd,
        COP1::sdc1,
        COP0::reserved,
        CPU::sd,
    };

    secondary = {
        CPU::sll,
        COP0::reserved,
        CPU::srl,
        CPU::sra,
        CPU::sllv,
        COP0::reserved,
        CPU::srlv,
        CPU::srav,
        CPU::jr,
        CPU::jalr,
        COP0::reserved,
        COP0::reserved,
        COP0::syscall,
        COP0::break_,
        COP0::reserved,
        CPU::sync,
        CPU::mfhi,
        CPU::mthi,
        CPU::mflo,
        CPU::mtlo,
        CPU::dsllv,
        COP0::reserved,
        CPU::dsrlv,
        CPU::dsrav,
        CPU::mult,
        CPU::multu,
        CPU::div,
        CPU::divu,
        CPU::dmult,
        CPU::dmultu,
        CPU::ddiv,
        CPU::ddivu,
        CPU::add,
        CPU::addu,
        CPU::sub,
        CPU::subu,
        CPU::and_,
        CPU::or_,
        CPU::xor_,
        CPU::nor,
        COP0::reserved,
        COP0::reserved,
        CPU::slt,
        CPU::sltu,
        CPU::dadd,
        CPU::daddu,
        CPU::dsub,
        CPU::dsubu,
        CPU::tge,
        CPU::tgeu,
        CPU::tlt,
        CPU::tltu,
        CPU::teq,
        COP0::reserved,
        CPU::tne,
        COP0::reserved,
        CPU::dsll,
        COP0::reserved,
        CPU::dsrl,
        CPU::dsra,
        CPU::dsll32,
        COP0::reserved,
        CPU::dsrl32,
        CPU::dsra32,
    };

    registerInstructions = {
        CPU::bltz,
        CPU::bgez,
        CPU::bltzl,
        CPU::bgezl,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        CPU::tgei,
        CPU::tgeiu,
        CPU::tlti,
        CPU::tltiu,
        CPU::teqi,
        COP0::reserved,
        CPU::tnei,
        COP0::reserved,
        CPU::bltzal,
        CPU::bgezal,
        CPU::bltzall,
        CPU::bgezall,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
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
void CPU::enterException(bool usePreviousPc, uint64_t offset) {
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
        pc = 0x80000000 + offset;
        nextPc = 0x80000000 + offset + 4;
    } else {
        pc = 0xbfc00200 + offset;
        nextPc = 0xbfc00200 + offset + 4;
    }

    cop0.addCycles(2);
}

void CPU::step() {
    r[0] = 0;

    auto [physicalPc, error, cached] = bus.translateAddress(pc);

    if (error) {
        return;
    }

    uint32_t opcode = !cached ? bus.memRead32(physicalPc, cached) : bus.readInstructionCache(physicalPc);
    uint32_t command = opcode >> 26;

    previousPc = pc;
    debugPc = physicalPc;

    if (!discarded) {
        pc = nextPc;
    } else {
        nextPc += 4;
        pc = nextPc;
    }

    discarded = false;
    nextPc += 4;

    bool oldDelaySlot = inDelaySlot;

    // if (!visited.contains(previousPc) && debugOn) {
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
    //     } else if (command == 1) {
    //         std::cout << "command is regImm\n";
    //         actualCommand = (opcode >> 16) & 0x1f;
    //     }
    //     std::cout << "pc = " << std::hex << bus.translateAddress(previousPc) << ", command = " << std::dec << actualCommand << "\n";
    //     // std::cout << "pc = " << std::hex << bus.translateAddress(previousPc) << "\n";
    //     visited.insert(previousPc);
    // }

    if (!visited.contains(previousPc) && debugOn) {
        std::string disassembled = disassemble(this, opcode);

        std::println("[CPU] [PC: 0x{:x}] [Opcode: 0x{:08x}] {}", debugPc, opcode, disassembled);

        visited.insert(previousPc);
    }

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
            throw std::runtime_error("not yet implemented: 18");
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

                bus.videoInterface.field ^= bus.videoInterface.ctrl.serrate;

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

                scheduler.addEvent(Event(CompareCount, cop0.count + 0xffffffff));

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
                if (bus.audioInterface.dmaReady) {
                    bus.pushSamples(bus.audioInterface.fifo[0].length, bus.audioInterface.fifo[0].address);
                    bus.audioInterface.dmaReady = false;
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
                throw std::runtime_error("should never happen");
                break;
        }
    }
}

void CPU::fastForwardAbsoluteLoop(uint64_t target) {
    auto [physicalPc, error, cached] = bus.translateAddress(pc);
    if (pc == target && bus.memRead32(physicalPc, cached, false, true) == 0) {
        cop0.count = scheduler.getTimeToNext();
    }
}

void CPU::fastForwardRelativeLoop(int16_t amount) {
    auto [physicalPc, error, cached] = bus.translateAddress(pc);
    if (amount == -4 && bus.memRead32(physicalPc, cached, false, true) == 0) {
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