#pragma once

#include "CPU.hpp"

COP1::COP1() {
    instructions = {
        COP1::mfc1,           // 0
        COP1::dmfc1,          // 1
        COP1::cfc1,           // 2
        COP1::dcfc1,          // 3
        COP1::mtc1,           // 4
        COP1::dmtc1,          // 5
        COP1::ctc1,           // 6
        COP1::dctc1,          // 7
        COP1::cop1_b_instrs,  // 8
        COP1::reserved,       // 9
        COP1::reserved,       // 10
        COP1::reserved,       // 11
        COP1::reserved,       // 12
        COP1::reserved,       // 13
        COP1::reserved,       // 14
        COP1::reserved,       // 15
        COP1::cop1_s_instrs,  // 16
        COP1::cop1_d_instrs,  // 17
        COP1::reserved,       // 18
        COP1::reserved,       // 19
        COP1::cop1_w_instrs,  // 20
        COP1::cop1_l_instrs,  // 21
        COP1::reserved,       // 22
        COP1::reserved,       // 23
        COP1::reserved,       // 24
        COP1::reserved,       // 25
        COP1::reserved,       // 26
        COP1::reserved,       // 27
        COP1::reserved,       // 28
        COP1::reserved,       // 29
        COP1::reserved,       // 30
        COP1::reserved,       // 31
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
}