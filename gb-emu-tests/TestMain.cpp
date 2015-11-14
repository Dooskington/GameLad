#include "stdafx.h"

#if !WINDOWS
#include <CPU.hpp>
#include "CPUTests.cpp"
#include "GPUTests.cpp"
#include "JoypadTests.cpp"
#include "MBCTests.cpp"

int main(int arg, char** argv)
{
    int passed = 0;
    int failed = 0;

    std::cout << "----------------------------------" << std::endl;
    TEST_SETUP(CPUTests);

    // Misc Tests
    TEST_CALL(CPUTests, Endian_Test);
    TEST_CALL(CPUTests, GetHighByte_Test);
    TEST_CALL(CPUTests, GetLowByte_Test);
    TEST_CALL(CPUTests, GetByteRegister_Test);
    TEST_CALL(CPUTests, GetUShortRegister_Test);

    // TODO: Organize the following...
    // Z80 Instruction Set Tests
    TEST_CALL(CPUTests, LDrR_Test);
    TEST_CALL(CPUTests, NOP_Test);
    TEST_CALL(CPUTests, LDBn_Test);
    TEST_CALL(CPUTests, LDCn_Test);
    TEST_CALL(CPUTests, LDBCnn_Test);
    TEST_CALL(CPUTests, LDDEnn_Test);
    TEST_CALL(CPUTests, LDHLnn_Test);
    TEST_CALL(CPUTests, LDSPnn_Test);
    TEST_CALL(CPUTests, LDDn_Test);
    TEST_CALL(CPUTests, RLA_Test);
    TEST_CALL(CPUTests, LDA_DE__Test);
    TEST_CALL(CPUTests, LDEn_Test);
    TEST_CALL(CPUTests, JRNZe_Test);
    TEST_CALL(CPUTests, LDHn_Test);
    TEST_CALL(CPUTests, LDLn_Test);
    TEST_CALL(CPUTests, LDD_HL_A_Test);
    TEST_CALL(CPUTests, LDAn_Test);
    TEST_CALL(CPUTests, LD_HL_A_Test);
    TEST_CALL(CPUTests, POPBC_Test);
    TEST_CALL(CPUTests, PUSHBC_Test);
    TEST_CALL(CPUTests, CALLnn_Test);
    TEST_CALL(CPUTests, LD_0xFF00n_A_Test);
    TEST_CALL(CPUTests, LD_0xFF00C_A_Test);
    TEST_CALL(CPUTests, INCA_Test);
    TEST_CALL(CPUTests, INCB_Test);
    TEST_CALL(CPUTests, INCC_Test);
    TEST_CALL(CPUTests, INCD_Test);
    TEST_CALL(CPUTests, INCE_Test);
    TEST_CALL(CPUTests, INCH_Test);
    TEST_CALL(CPUTests, INCL_Test);
    TEST_CALL(CPUTests, XORA_Test);
    TEST_CALL(CPUTests, XORB_Test);
    TEST_CALL(CPUTests, XORC_Test);
    TEST_CALL(CPUTests, XORD_Test);
    TEST_CALL(CPUTests, XORE_Test);
    TEST_CALL(CPUTests, XORH_Test);
    TEST_CALL(CPUTests, XORL_Test);

    // Z80 Instruction Set - CB Tests
    TEST_CALL(CPUTests, BITbr_Test);
    TEST_CALL(CPUTests, RESbr_Test);

    TEST_CALL(CPUTests, RLC_Test);

    TEST_CLEANUP();

    TEST_SETUP(GPUTests);
    TEST_CALL(GPUTests, GPUCycleTest);
    TEST_CLEANUP();

    TEST_SETUP(JoypadTests);
    TEST_CALL(JoypadTests, FullInputTest);
    TEST_CLEANUP();

    TEST_SETUP(MBCTests);
    TEST_CALL(MBCTests, ROMOnlyTest);
    TEST_CALL(MBCTests, MBC1Test);
    TEST_CALL(MBCTests, MBC2Test);
    TEST_CALL(MBCTests, MBC3Test);
    TEST_CLEANUP();

    std::cout << "----------------------------------" << std::endl;
    std::cout << "Passed: " << passed << "   Failed: " << failed << "   Total: " << passed + failed << std::endl;

    return 0;
}
#endif
