#include <cstdint>

typedef union FCSR {
    struct {
        unsigned int roundingMode: 2;
        unsigned int flagInexact: 1;
        unsigned int flagUnderflow: 1;
        unsigned int flagOverflow: 1;
        unsigned int flagDivisionByZero: 1;
        unsigned int flagInvalidOp: 1;
        unsigned int inexactOperationEnable: 1;
        unsigned int underflowEnable: 1;
        unsigned int overflowEnable: 1;
        unsigned int divisionByZeroEnable: 1;
        unsigned int invalidOpEnable: 1;
        unsigned int causeUnderflow: 1;
        unsigned int causeOverflow: 1;
        unsigned int causeDivisionByZero: 1;
        unsigned int causeInvalidOp: 1;
        unsigned int caseUnimplemented: 1;
        unsigned int unused: 5;
        unsigned int condition: 1;
        unsigned int flushDenormToZero: 1;
    };

    uint32_t value = 0;
} fcsrbitset;