#pragma once

#include "CPU.hpp"
#include "../util/BitUtils.cpp"

COP1::COP1(CPU& cpu): cpu(cpu) {
    instructions = {
        COP1::mfc1,
        COP1::dmfc1,
        COP1::cfc1,
        COP1::dcfc1,
        COP1::mtc1,
        COP1::dmtc1,
        COP1::ctc1,
        COP1::dctc1,
        COP1::cop1_b_instrs,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::cop1_s_instrs,
        COP1::cop1_d_instrs,
        COP1::reserved,
        COP1::reserved,
        COP1::cop1_w_instrs,
        COP1::cop1_l_instrs,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
    };

    sInstructions = {
        COP1::addS,
        COP1::subS,
        COP1::mulS,
        COP1::divS,
        COP1::sqrtS,
        COP1::absS,
        COP1::movS,
        COP1::negS,
        COP1::roundLS,
        COP1::truncLS,
        COP1::ceilLS,
        COP1::floorLS,
        COP1::roundWS,
        COP1::truncWS,
        COP1::ceilWS,
        COP1::floorWS,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::cvtDS,
        COP1::reserved,
        COP1::reserved,
        COP1::cvtWS,
        COP1::cvtLS,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::CFS,
        COP1::cUnS,
        COP1::cEqS,
        COP1::cUeqS,
        COP1::cOltS,
        COP1::cUltS,
        COP1::cOleS,
        COP1::cUleS,
        COP1::cSfS,
        COP1::cNgleS,
        COP1::cSeqS,
        COP1::cNglS,
        COP1::cLtS,
        COP1::cNgeS,
        COP1::cLeS,
        COP1::cNgtS,
    };


    bInstructions = {
        COP1::bc1f,
        COP1::bc1t,
        COP1::bc1fl,
        COP1::bc1tl
    };

    dInstructions = {
        COP1::addD,
        COP1::subD,
        COP1::mulD,
        COP1::divD,
        COP1::sqrtD,
        COP1::absD,
        COP1::movD,
        COP1::negD,
        COP1::roundLD,
        COP1::truncLD,
        COP1::ceilLD,
        COP1::floorLD,
        COP1::roundWD,
        COP1::truncWD,
        COP1::ceilWD,
        COP1::floorWD,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::cvtSD,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::cvtWD,
        COP1::cvtLD,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::reserved,
        COP1::cFD,
        COP1::cUnD,
        COP1::cEqD,
        COP1::cUeqD,
        COP1::cOltD,
        COP1::cUltD,
        COP1::cOleD,
        COP1::cUleD,
        COP1::cSfD,
        COP1::cNgleD,
        COP1::cSeqD,
        COP1::cNglD,
        COP1::cLtD,
        COP1::cNgeD,
        COP1::cLeD,
        COP1::cNgtD,
    };
}

void COP1::setCop1Registers(uint8_t fr) {
    if (!fr) {
        for (int i = 0; i < fgr32.size(); i += 2) {
            fgr32[i] = (uint32_t)fgr64[i];
            fgr32[i+1] = (uint32_t)(fgr64[i] >> 32);
        }
    } else {
        for (int i = 0; i < fgr64.size(); i += 2) {
            fgr64[i] = ((uint64_t)fgr32[i]) | ((uint64_t)fgr32[i + 1] << 32);
        }
    }
}

float COP1::getFloat(uint32_t index) {
    if (!getBit(cpu.cop0.status, Fr)) {
        return ((union convu32){.u32 = fgr32[index]}).f32;
    }

    return ((union convu32){.u32 = (uint32_t)fgr64[index]}).f32;
}

void COP1::setFloat(uint32_t index, float value) {
    uint32_t bits = ((union convu32){.f32 = value}).u32;

    if (!getBit(cpu.cop0.status, Fr)) {
        fgr32[index] = bits;
    } else {
        fgr64[index] = (uint64_t)bits;
    }
}

double COP1::getDouble(uint32_t index) {
    if (!getBit(cpu.cop0.status, Fr)) {
        uint64_t bits = (uint64_t)fgr32[index] | ((uint64_t)fgr32[index + 1] << 32);

        return ((union convu64){.u64 = bits}).f64;
    }

    return ((union convu64){.u64 = fgr64[index]}).f64;
}

void COP1::setDouble(uint32_t index, double value) {
    uint64_t bits = ((union convu64){.f64 = value}).u64;

    if (!getBit(cpu.cop0.status, Fr)) {
        fgr32[index] = (uint32_t)bits;
        fgr32[index + 1] = (uint32_t)(bits >> 32);
    } else {
        fgr64[index] = bits;
    }
}