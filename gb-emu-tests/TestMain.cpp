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
    TEST_CALL(CPUTests, Timing_Test);
    TEST_CALL(CPUTests, Endian_Test);
    TEST_CALL(CPUTests, GetHighByte_Test);
    TEST_CALL(CPUTests, GetLowByte_Test);
    TEST_CALL(CPUTests, GetByteRegister_Test);
    TEST_CALL(CPUTests, GetUShortRegister_Test);

    // TODO: Organize the following...
    // Z80 Instruction Set Tests
    TEST_CALL(CPUTests, LD_BC_A_Test);
    TEST_CALL(CPUTests, LDI_HL_A_Test);
    TEST_CALL(CPUTests, LDIA_HL_Test);
    TEST_CALL(CPUTests, LDrR_Test);
    TEST_CALL(CPUTests, NOP_Test);
    TEST_CALL(CPUTests, LDBn_Test);
    TEST_CALL(CPUTests, LDCn_Test);
    TEST_CALL(CPUTests, LDBCnn_Test);
    TEST_CALL(CPUTests, LDDEnn_Test);
    TEST_CALL(CPUTests, LDHLnn_Test);
    TEST_CALL(CPUTests, LDSPnn_Test);
    TEST_CALL(CPUTests, LDDn_Test);
    TEST_CALL(CPUTests, STOP_Test);
    TEST_CALL(CPUTests, RLA_Test);
    TEST_CALL(CPUTests, RRA2_Test);
    TEST_CALL(CPUTests, LDA_DE__Test);
    TEST_CALL(CPUTests, LDA_BC__Test);
    TEST_CALL(CPUTests, LDEn_Test);
    TEST_CALL(CPUTests, JRNZe_Test);
    TEST_CALL(CPUTests, JRZe_Test);
    TEST_CALL(CPUTests, JRNCe_Test);
    TEST_CALL(CPUTests, JRe_Test);
    TEST_CALL(CPUTests, LDHn_Test);
    TEST_CALL(CPUTests, LDLn_Test);
    TEST_CALL(CPUTests, LDD_HL_A_Test);
    TEST_CALL(CPUTests, LDDA_HL__Test);
    TEST_CALL(CPUTests, LDAn_Test);
    TEST_CALL(CPUTests, HALT_Test);
    TEST_CALL(CPUTests, POPBC_Test);
    TEST_CALL(CPUTests, POPDE_Test);
    TEST_CALL(CPUTests, POPHL_Test);
    TEST_CALL(CPUTests, POPAF_Test);
    TEST_CALL(CPUTests, PUSHBC_Test);
    TEST_CALL(CPUTests, PUSHDE_Test);
    TEST_CALL(CPUTests, PUSHHL_Test);
    TEST_CALL(CPUTests, PUSHAF_Test);
    TEST_CALL(CPUTests, CALLnn_Test);
    TEST_CALL(CPUTests, CALLccnn_Test);
    TEST_CALL(CPUTests, RETcc_Test);
    TEST_CALL(CPUTests, LD_0xFF00n_A_Test);
    TEST_CALL(CPUTests, LD_0xFF00C_A_Test);
    TEST_CALL(CPUTests, LDA_0xFF00n__Test);
    TEST_CALL(CPUTests, LDA_0xFF00C__Test);
    TEST_CALL(CPUTests, INCA_Test);
    TEST_CALL(CPUTests, INCB_Test);
    TEST_CALL(CPUTests, INCC_Test);
    TEST_CALL(CPUTests, INCD_Test);
    TEST_CALL(CPUTests, INCE_Test);
    TEST_CALL(CPUTests, INCH_Test);
    TEST_CALL(CPUTests, INCL_Test);
    TEST_CALL(CPUTests, INCBC_Test);
    TEST_CALL(CPUTests, INCDE_Test);
    TEST_CALL(CPUTests, INCHL_Test);
    TEST_CALL(CPUTests, INCSP_Test);
    TEST_CALL(CPUTests, INCHL_Test);
    TEST_CALL(CPUTests, DECBC_Test);
    TEST_CALL(CPUTests, DECDE_Test);
    TEST_CALL(CPUTests, DECHL_Test);
    TEST_CALL(CPUTests, DECSP_Test);
    TEST_CALL(CPUTests, DECB_Test);
    TEST_CALL(CPUTests, DECC_Test);
    TEST_CALL(CPUTests, DECD_Test);
    TEST_CALL(CPUTests, DECE_Test);
    TEST_CALL(CPUTests, DECH_Test);
    TEST_CALL(CPUTests, DECL_Test);
    TEST_CALL(CPUTests, DECA_Test);
    TEST_CALL(CPUTests, DEC_HL_Test);
    TEST_CALL(CPUTests, XORA_Test);
    TEST_CALL(CPUTests, XORB_Test);
    TEST_CALL(CPUTests, XORC_Test);
    TEST_CALL(CPUTests, XORD_Test);
    TEST_CALL(CPUTests, XORE_Test);
    TEST_CALL(CPUTests, XORH_Test);
    TEST_CALL(CPUTests, XORL_Test);
    TEST_CALL(CPUTests, XORn_Test);
    TEST_CALL(CPUTests, XOR_HL_Test);
    TEST_CALL(CPUTests, ADDHLss_Test);
    TEST_CALL(CPUTests, ANDr_Test);
    TEST_CALL(CPUTests, AND_HL__Test);
    TEST_CALL(CPUTests, LDSPHL_Test);
    TEST_CALL(CPUTests, LD_HL_n_Test);
    TEST_CALL(CPUTests, ADDSPdd_Test);
    TEST_CALL(CPUTests, SUB_HL__Test);
    TEST_CALL(CPUTests, SBCAn_Test);

    TEST_CALL(CPUTests, ORA_Test);
    TEST_CALL(CPUTests, ORB_Test);
    TEST_CALL(CPUTests, ORC_Test);
    TEST_CALL(CPUTests, ORD_Test);
    TEST_CALL(CPUTests, ORE_Test);
    TEST_CALL(CPUTests, ORH_Test);
    TEST_CALL(CPUTests, ORL_Test);
    TEST_CALL(CPUTests, OR_HL_Test);
    TEST_CALL(CPUTests, ORn_Test);

    TEST_CALL(CPUTests, ANDn_Test);

    TEST_CALL(CPUTests, RET_Test);
    TEST_CALL(CPUTests, CPn_Test);
    TEST_CALL(CPUTests, CPr_Test);
    TEST_CALL(CPUTests, CP_HL__Test);
    TEST_CALL(CPUTests, SUBB_Test);
    TEST_CALL(CPUTests, SUBC_Test);
    TEST_CALL(CPUTests, SUBD_Test);
    TEST_CALL(CPUTests, SUBE_Test);
    TEST_CALL(CPUTests, SUBH_Test);
    TEST_CALL(CPUTests, SUBL_Test);
    TEST_CALL(CPUTests, SUBA_Test);
    TEST_CALL(CPUTests, SUBn_Test);
    TEST_CALL(CPUTests, LD_nn_A_Test);
    TEST_CALL(CPUTests, ADDA_HL__Test);
    TEST_CALL(CPUTests, ADCA_HL__Test);
    TEST_CALL(CPUTests, ADDAr_Test);
    TEST_CALL(CPUTests, ADDAn_Test);
    TEST_CALL(CPUTests, ADCAr_Test);
    TEST_CALL(CPUTests, JPnn_Test);
    TEST_CALL(CPUTests, DI_Test);
    TEST_CALL(CPUTests, LDA_nn_Test);
    TEST_CALL(CPUTests, EI_Test);
    TEST_CALL(CPUTests, RETI_Test);
    TEST_CALL(CPUTests, SBCA_HL__Test);
    TEST_CALL(CPUTests, SBCAr_Test);

    TEST_CALL(CPUTests, LDr_HL_Test);

    TEST_CALL(CPUTests, LD_HL_r_Test);

    TEST_CALL(CPUTests, ADCAn_Test);

    // Z80 Instruction Set - CB Tests
    TEST_CALL(CPUTests, BITbr_Test);
    TEST_CALL(CPUTests, BITb_HL_Test);
    TEST_CALL(CPUTests, RESbr_Test);
    TEST_CALL(CPUTests, RESb_HL_Test);
    TEST_CALL(CPUTests, SETbr_Test);
    TEST_CALL(CPUTests, SETb_HL_Test);

    TEST_CALL(CPUTests, RLCB_Test);
    TEST_CALL(CPUTests, RLCC_Test);
    TEST_CALL(CPUTests, RLCD_Test);
    TEST_CALL(CPUTests, RLCE_Test);
    TEST_CALL(CPUTests, RLCH_Test);
    TEST_CALL(CPUTests, RLCL_Test);
    TEST_CALL(CPUTests, RLC_HL_Test);
    TEST_CALL(CPUTests, RLCA_Test);
    TEST_CALL(CPUTests, RLCA2_Test);

    TEST_CALL(CPUTests, RRCB_Test);
    TEST_CALL(CPUTests, RRCC_Test);
    TEST_CALL(CPUTests, RRCD_Test);
    TEST_CALL(CPUTests, RRCE_Test);
    TEST_CALL(CPUTests, RRCH_Test);
    TEST_CALL(CPUTests, RRCL_Test);
    TEST_CALL(CPUTests, RRC_HL_Test);
    TEST_CALL(CPUTests, RRCA_Test);

    TEST_CALL(CPUTests, RLB_Test);
    TEST_CALL(CPUTests, RLC_Test);
    TEST_CALL(CPUTests, RLD_Test);
    TEST_CALL(CPUTests, RLE_Test);
    TEST_CALL(CPUTests, RLH_Test);
    TEST_CALL(CPUTests, RLL_Test);
    TEST_CALL(CPUTests, RL_HL_Test);
    TEST_CALL(CPUTests, RLA2_Test);

    TEST_CALL(CPUTests, RRB_Test);
    TEST_CALL(CPUTests, RRC_Test);
    TEST_CALL(CPUTests, RRD_Test);
    TEST_CALL(CPUTests, RRE_Test);
    TEST_CALL(CPUTests, RRH_Test);
    TEST_CALL(CPUTests, RRL_Test);
    TEST_CALL(CPUTests, RR_HL_Test);
    TEST_CALL(CPUTests, RRA_Test);

    TEST_CALL(CPUTests, SLAB_Test);
    TEST_CALL(CPUTests, SLAC_Test);
    TEST_CALL(CPUTests, SLAD_Test);
    TEST_CALL(CPUTests, SLAE_Test);
    TEST_CALL(CPUTests, SLAH_Test);
    TEST_CALL(CPUTests, SLAL_Test);
    TEST_CALL(CPUTests, SLA_HL_Test);
    TEST_CALL(CPUTests, SLAA_Test);

    TEST_CALL(CPUTests, SRAB_Test);
    TEST_CALL(CPUTests, SRAC_Test);
    TEST_CALL(CPUTests, SRAD_Test);
    TEST_CALL(CPUTests, SRAE_Test);
    TEST_CALL(CPUTests, SRAH_Test);
    TEST_CALL(CPUTests, SRAL_Test);
    TEST_CALL(CPUTests, SRA_HL_Test);
    TEST_CALL(CPUTests, SRAA_Test);

    TEST_CALL(CPUTests, SRLB_Test);
    TEST_CALL(CPUTests, SRLC_Test);
    TEST_CALL(CPUTests, SRLD_Test);
    TEST_CALL(CPUTests, SRLE_Test);
    TEST_CALL(CPUTests, SRLH_Test);
    TEST_CALL(CPUTests, SRLL_Test);
    TEST_CALL(CPUTests, SRL_HL_Test);
    TEST_CALL(CPUTests, SRLA_Test);

    TEST_CALL(CPUTests, SWAP_HL_Test);
    TEST_CALL(CPUTests, SWAPr_Test);
    TEST_CALL(CPUTests, JP_HL_Test);
    TEST_CALL(CPUTests, SCF_Test);
    TEST_CALL(CPUTests, CCF_Test);
    TEST_CALL(CPUTests, CPL_Test);
    TEST_CALL(CPUTests, JPccnn_Test);
    TEST_CALL(CPUTests, JRCe_Test);
    TEST_CALL(CPUTests, LD_DE_A_Test);
    TEST_CALL(CPUTests, LDHLSPe_Test);
    TEST_CALL(CPUTests, RSTn_Test);
    TEST_CALL(CPUTests, LD_nn_SP_Test);
    TEST_CALL(CPUTests, DAA_Test);
    TEST_CALL(CPUTests, RRCA2_Test);

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
