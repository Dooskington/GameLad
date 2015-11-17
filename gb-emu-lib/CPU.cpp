#include "pch.hpp"
#include "CPU.hpp"

#include <vector>

// The number of CPU cycles per frame
const unsigned int CyclesPerFrame = 70224;

struct Trace
{
    byte opCode;
    ushort AF;
    ushort BC;
    ushort DE;
    ushort HL;
    ushort SP;
    ushort PC;
};
std::vector<Trace> trace;

CPU::CPU() :
    m_cycles(0),
    m_isHalted(false),
    m_IFWhenHalted(0x00),
    m_AF(0x0000),
    m_BC(0x0000),
    m_DE(0x0000),
    m_HL(0x0000),
    m_SP(0x0000),
    m_PC(0x0000),
    m_IME(0x00)
{
    for (unsigned int index = 0; index < ARRAYSIZE(m_operationMap); index++)
    {
        m_operationMap[index] = nullptr;
    }
    for (unsigned int index = 0; index < ARRAYSIZE(m_operationMapCB); index++)
    {
        m_operationMapCB[index] = nullptr;
    }

    // TODO: Do something with the hundreds of lines below. All this should probably
    // be in its own file. I apologize in advance for this large monstrosity. But it works.

    /*
        Z80 Command Set
    */

    // 00
    m_operationMap[0x00] = &CPU::NOP;
    m_operationMap[0x01] = &CPU::LDrrnn;
    m_operationMap[0x02] = &CPU::LD_BC_A;
    m_operationMap[0x03] = &CPU::INCrr;
    m_operationMap[0x04] = &CPU::INCr;
    m_operationMap[0x05] = &CPU::DECr;
    m_operationMap[0x06] = &CPU::LDrn;
    m_operationMap[0x07] = &CPU::RLCA;
    m_operationMap[0x08] = &CPU::LD_nn_SP;
    m_operationMap[0x09] = &CPU::ADDHLss;
    m_operationMap[0x0A] = &CPU::LDA_BC_;
    m_operationMap[0x0B] = &CPU::DECrr;
    m_operationMap[0x0C] = &CPU::INCr;
    m_operationMap[0x0D] = &CPU::DECr;
    m_operationMap[0x0E] = &CPU::LDrn;
    //m_operationMap[0x0F] TODO

    // 10
    m_operationMap[0x10] = &CPU::STOP;
    m_operationMap[0x11] = &CPU::LDrrnn;
    m_operationMap[0x12] = &CPU::LD_DE_A;
    m_operationMap[0x13] = &CPU::INCrr;
    m_operationMap[0x14] = &CPU::INCr;
    m_operationMap[0x15] = &CPU::DECr;
    m_operationMap[0x16] = &CPU::LDrn;
    m_operationMap[0x17] = &CPU::RLA;
    m_operationMap[0x18] = &CPU::JRe;
    m_operationMap[0x19] = &CPU::ADDHLss;
    m_operationMap[0x1A] = &CPU::LDA_DE_;
    m_operationMap[0x1B] = &CPU::DECrr;
    m_operationMap[0x1C] = &CPU::INCr;
    m_operationMap[0x1D] = &CPU::DECr;
    m_operationMap[0x1E] = &CPU::LDrn;
    m_operationMap[0x1F] = &CPU::RRA;

    // 20
    m_operationMap[0x20] = &CPU::JRcce;
    m_operationMap[0x21] = &CPU::LDrrnn;
    m_operationMap[0x22] = &CPU::LDI_HL_A;
    m_operationMap[0x23] = &CPU::INCrr;
    m_operationMap[0x24] = &CPU::INCr;
    m_operationMap[0x25] = &CPU::DECr;
    m_operationMap[0x26] = &CPU::LDrn;
    m_operationMap[0x27] = &CPU::DAA;
    m_operationMap[0x28] = &CPU::JRcce;
    m_operationMap[0x29] = &CPU::ADDHLss;
    m_operationMap[0x2A] = &CPU::LDIA_HL_;
    m_operationMap[0x2B] = &CPU::DECrr;
    m_operationMap[0x2C] = &CPU::INCr;
    m_operationMap[0x2D] = &CPU::DECr;
    m_operationMap[0x2E] = &CPU::LDrn;
    m_operationMap[0x2F] = &CPU::CPL;

    // 30
    m_operationMap[0x30] = &CPU::JRcce;
    m_operationMap[0x31] = &CPU::LDrrnn;
    m_operationMap[0x32] = &CPU::LDD_HL_A;
    m_operationMap[0x33] = &CPU::INCrr;
    m_operationMap[0x34] = &CPU::INC_HL_;
    m_operationMap[0x35] = &CPU::DEC_HL_;
    m_operationMap[0x36] = &CPU::LD_HL_n;
    m_operationMap[0x37] = &CPU::SCF;
    m_operationMap[0x38] = &CPU::JRcce;
    m_operationMap[0x39] = &CPU::ADDHLss;
    m_operationMap[0x3A] = &CPU::LDDA_HL_;
    m_operationMap[0x3B] = &CPU::DECrr;
    m_operationMap[0x3C] = &CPU::INCr;
    m_operationMap[0x3D] = &CPU::DECr;
    m_operationMap[0x3E] = &CPU::LDrn;
    m_operationMap[0x3F] = &CPU::CCF;

    // 40
    m_operationMap[0x40] = &CPU::LDrR;
    m_operationMap[0x41] = &CPU::LDrR;
    m_operationMap[0x42] = &CPU::LDrR;
    m_operationMap[0x43] = &CPU::LDrR;
    m_operationMap[0x44] = &CPU::LDrR;
    m_operationMap[0x45] = &CPU::LDrR;
    m_operationMap[0x46] = &CPU::LDr_HL_;
    m_operationMap[0x47] = &CPU::LDrR;
    m_operationMap[0x48] = &CPU::LDrR;
    m_operationMap[0x49] = &CPU::LDrR;
    m_operationMap[0x4A] = &CPU::LDrR;
    m_operationMap[0x4B] = &CPU::LDrR;
    m_operationMap[0x4C] = &CPU::LDrR;
    m_operationMap[0x4D] = &CPU::LDrR;
    m_operationMap[0x4E] = &CPU::LDr_HL_;
    m_operationMap[0x4F] = &CPU::LDrR;

    // 50
    m_operationMap[0x50] = &CPU::LDrR;
    m_operationMap[0x51] = &CPU::LDrR;
    m_operationMap[0x52] = &CPU::LDrR;
    m_operationMap[0x53] = &CPU::LDrR;
    m_operationMap[0x54] = &CPU::LDrR;
    m_operationMap[0x55] = &CPU::LDrR;
    m_operationMap[0x56] = &CPU::LDr_HL_;
    m_operationMap[0x57] = &CPU::LDrR;
    m_operationMap[0x58] = &CPU::LDrR;
    m_operationMap[0x59] = &CPU::LDrR;
    m_operationMap[0x5A] = &CPU::LDrR;
    m_operationMap[0x5B] = &CPU::LDrR;
    m_operationMap[0x5C] = &CPU::LDrR;
    m_operationMap[0x5D] = &CPU::LDrR;
    m_operationMap[0x5E] = &CPU::LDr_HL_;
    m_operationMap[0x5F] = &CPU::LDrR;

    // 60
    m_operationMap[0x60] = &CPU::LDrR;
    m_operationMap[0x61] = &CPU::LDrR;
    m_operationMap[0x62] = &CPU::LDrR;
    m_operationMap[0x63] = &CPU::LDrR;
    m_operationMap[0x64] = &CPU::LDrR;
    m_operationMap[0x65] = &CPU::LDrR;
    m_operationMap[0x66] = &CPU::LDr_HL_;
    m_operationMap[0x67] = &CPU::LDrR;
    m_operationMap[0x68] = &CPU::LDrR;
    m_operationMap[0x69] = &CPU::LDrR;
    m_operationMap[0x6A] = &CPU::LDrR;
    m_operationMap[0x6B] = &CPU::LDrR;
    m_operationMap[0x6C] = &CPU::LDrR;
    m_operationMap[0x6D] = &CPU::LDrR;
    m_operationMap[0x6E] = &CPU::LDr_HL_;
    m_operationMap[0x6F] = &CPU::LDrR;

    // 70
    m_operationMap[0x70] = &CPU::LD_HL_r;
    m_operationMap[0x71] = &CPU::LD_HL_r;
    m_operationMap[0x72] = &CPU::LD_HL_r;
    m_operationMap[0x73] = &CPU::LD_HL_r;
    m_operationMap[0x74] = &CPU::LD_HL_r;
    m_operationMap[0x75] = &CPU::LD_HL_r;
    m_operationMap[0x76] = &CPU::HALT;
    m_operationMap[0x77] = &CPU::LD_HL_r;
    m_operationMap[0x78] = &CPU::LDrR;
    m_operationMap[0x79] = &CPU::LDrR;
    m_operationMap[0x7A] = &CPU::LDrR;
    m_operationMap[0x7B] = &CPU::LDrR;
    m_operationMap[0x7C] = &CPU::LDrR;
    m_operationMap[0x7D] = &CPU::LDrR;
    m_operationMap[0x7E] = &CPU::LDr_HL_;
    m_operationMap[0x7F] = &CPU::LDrR;

    // 80
    m_operationMap[0x80] = &CPU::ADDAr;
    m_operationMap[0x81] = &CPU::ADDAr;
    m_operationMap[0x82] = &CPU::ADDAr;
    m_operationMap[0x83] = &CPU::ADDAr;
    m_operationMap[0x84] = &CPU::ADDAr;
    m_operationMap[0x85] = &CPU::ADDAr;
    m_operationMap[0x86] = &CPU::ADDA_HL_;
    m_operationMap[0x87] = &CPU::ADDAr;
    m_operationMap[0x88] = &CPU::ADCAr;
    m_operationMap[0x89] = &CPU::ADCAr;
    m_operationMap[0x8A] = &CPU::ADCAr;
    m_operationMap[0x8B] = &CPU::ADCAr;
    m_operationMap[0x8C] = &CPU::ADCAr;
    m_operationMap[0x8D] = &CPU::ADCAr;
    m_operationMap[0x8E] = &CPU::ADCAr;
    m_operationMap[0x8F] = &CPU::ADCAr;

    // 90
    m_operationMap[0x90] = &CPU::SUBr;
    m_operationMap[0x91] = &CPU::SUBr;
    m_operationMap[0x92] = &CPU::SUBr;
    m_operationMap[0x93] = &CPU::SUBr;
    m_operationMap[0x94] = &CPU::SUBr;
    m_operationMap[0x95] = &CPU::SUBr;
    m_operationMap[0x96] = &CPU::SUB_HL_;
    m_operationMap[0x97] = &CPU::SUBr;
    //m_operationMap[0x98] TODO
    //m_operationMap[0x99] TODO
    //m_operationMap[0x9A] TODO
    //m_operationMap[0x9B] TODO
    //m_operationMap[0x9C] TODO
    //m_operationMap[0x9D] TODO
    //m_operationMap[0x9E] TODO
    //m_operationMap[0x9F] TODO

    // A0
    m_operationMap[0xA0] = &CPU::ANDr;
    m_operationMap[0xA1] = &CPU::ANDr;
    m_operationMap[0xA2] = &CPU::ANDr;
    m_operationMap[0xA3] = &CPU::ANDr;
    m_operationMap[0xA4] = &CPU::ANDr;
    m_operationMap[0xA5] = &CPU::ANDr;
    m_operationMap[0xA6] = &CPU::AND_HL_;
    m_operationMap[0xA7] = &CPU::ANDr;
    m_operationMap[0xA8] = &CPU::XORr;
    m_operationMap[0xA9] = &CPU::XORr;
    m_operationMap[0xAA] = &CPU::XORr;
    m_operationMap[0xAB] = &CPU::XORr;
    m_operationMap[0xAC] = &CPU::XORr;
    m_operationMap[0xAD] = &CPU::XORr;
    m_operationMap[0xAE] = &CPU::XOR_HL_;
    m_operationMap[0xAF] = &CPU::XORr;

    // B0
    m_operationMap[0xB0] = &CPU::ORr;
    m_operationMap[0xB1] = &CPU::ORr;
    m_operationMap[0xB2] = &CPU::ORr;
    m_operationMap[0xB3] = &CPU::ORr;
    m_operationMap[0xB4] = &CPU::ORr;
    m_operationMap[0xB5] = &CPU::ORr;
    m_operationMap[0xB6] = &CPU::OR_HL_;
    m_operationMap[0xB7] = &CPU::ORr;
    m_operationMap[0xB8] = &CPU::CPr;
    m_operationMap[0xB9] = &CPU::CPr;
    m_operationMap[0xBA] = &CPU::CPr;
    m_operationMap[0xBB] = &CPU::CPr;
    m_operationMap[0xBC] = &CPU::CPr;
    m_operationMap[0xBD] = &CPU::CPr;
    m_operationMap[0xBE] = &CPU::CP_HL_;
    m_operationMap[0xBF] = &CPU::CPr;

    // C0
    m_operationMap[0xC0] = &CPU::RETcc;
    m_operationMap[0xC1] = &CPU::POPrr;
    m_operationMap[0xC2] = &CPU::JPccnn;
    m_operationMap[0xC3] = &CPU::JPnn;
    m_operationMap[0xC4] = &CPU::CALLccnn;
    m_operationMap[0xC5] = &CPU::PUSHrr;
    m_operationMap[0xC6] = &CPU::ADDAn;
    m_operationMap[0xC7] = &CPU::RSTn;
    m_operationMap[0xC8] = &CPU::RETcc;
    m_operationMap[0xC9] = &CPU::RET;
    m_operationMap[0xCA] = &CPU::JPccnn;
    //m_operationMap[0xCB] MAPPED TO 0xCB MAP
    m_operationMap[0xCC] = &CPU::CALLccnn;
    m_operationMap[0xCD] = &CPU::CALLnn;
    m_operationMap[0xCE] = &CPU::ADCAn;
    m_operationMap[0xCF] = &CPU::RSTn;

    // D0
    m_operationMap[0xD0] = &CPU::RETcc;
    m_operationMap[0xD1] = &CPU::POPrr;
    m_operationMap[0xD2] = &CPU::JPccnn;
    //m_operationMap[0xD3] UNUSED
    m_operationMap[0xD4] = &CPU::CALLccnn;
    m_operationMap[0xD5] = &CPU::PUSHrr;
    m_operationMap[0xD6] = &CPU::SUBn;
    m_operationMap[0xD7] = &CPU::RSTn;
    m_operationMap[0xD8] = &CPU::RETcc;
    m_operationMap[0xD9] = &CPU::RETI;
    m_operationMap[0xDA] = &CPU::JPccnn;
    //m_operationMap[0xDB] UNUSED
    m_operationMap[0xDC] = &CPU::CALLccnn;
    //m_operationMap[0xDD] UNUSED
    m_operationMap[0xDE] = &CPU::SBCAn;
    m_operationMap[0xDF] = &CPU::RSTn;

    // E0
    m_operationMap[0xE0] = &CPU::LD_0xFF00n_A;
    m_operationMap[0xE1] = &CPU::POPrr;
    m_operationMap[0xE2] = &CPU::LD_0xFF00C_A;
    //m_operationMap[0xE3] UNUSED
    //m_operationMap[0xE4] UNUSED
    m_operationMap[0xE5] = &CPU::PUSHrr;
    m_operationMap[0xE6] = &CPU::ANDn;
    m_operationMap[0xE7] = &CPU::RSTn;
    m_operationMap[0xE8] = &CPU::ADDSPdd;
    m_operationMap[0xE9] = &CPU::JP_HL_;
    m_operationMap[0xEA] = &CPU::LD_nn_A;
    //m_operationMap[0xEB] UNUSED
    //m_operationMap[0xEC] UNUSED
    //m_operationMap[0xED] UNUSED
    m_operationMap[0xEE] = &CPU::XORn;
    m_operationMap[0xEF] = &CPU::RSTn;

    // F0
    m_operationMap[0xF0] = &CPU::LDA_0xFF00n_;
    m_operationMap[0xF1] = &CPU::POPrr;
    m_operationMap[0xF2] = &CPU::LDA_0xFF00C_;
    m_operationMap[0xF3] = &CPU::DI;
    //m_operationMap[0xF4] UNUSED
    m_operationMap[0xF5] = &CPU::PUSHrr;
    m_operationMap[0xF6] = &CPU::ORn;
    m_operationMap[0xF7] = &CPU::RSTn;
    m_operationMap[0xF8] = &CPU::LDHLSPe;
    m_operationMap[0xF9] = &CPU::LDSPHL;
    m_operationMap[0xFA] = &CPU::LDA_nn_;
    m_operationMap[0xFB] = &CPU::EI;
    //m_operationMap[0xFC] UNUSED
    //m_operationMap[0xFD] UNUSED
    m_operationMap[0xFE] = &CPU::CPn;
    m_operationMap[0xFF] = &CPU::RSTn;

    /*
        Z80 Command Set - CB
    */

    // 00
    m_operationMapCB[0x00] = &CPU::RLCr;
    m_operationMapCB[0x01] = &CPU::RLCr;
    m_operationMapCB[0x02] = &CPU::RLCr;
    m_operationMapCB[0x03] = &CPU::RLCr;
    m_operationMapCB[0x04] = &CPU::RLCr;
    m_operationMapCB[0x05] = &CPU::RLCr;
    m_operationMapCB[0x06] = &CPU::RLC_HL_;
    m_operationMapCB[0x07] = &CPU::RLCr;
    m_operationMapCB[0x08] = &CPU::RRCr;
    m_operationMapCB[0x09] = &CPU::RRCr;
    m_operationMapCB[0x0A] = &CPU::RRCr;
    m_operationMapCB[0x0B] = &CPU::RRCr;
    m_operationMapCB[0x0C] = &CPU::RRCr;
    m_operationMapCB[0x0D] = &CPU::RRCr;
    m_operationMapCB[0x0E] = &CPU::RRC_HL_;
    m_operationMapCB[0x0F] = &CPU::RRCr;

    // 10
    m_operationMapCB[0x10] = &CPU::RLr;
    m_operationMapCB[0x11] = &CPU::RLr;
    m_operationMapCB[0x12] = &CPU::RLr;
    m_operationMapCB[0x13] = &CPU::RLr;
    m_operationMapCB[0x14] = &CPU::RLr;
    m_operationMapCB[0x15] = &CPU::RLr;
    m_operationMapCB[0x16] = &CPU::RL_HL_;
    m_operationMapCB[0x17] = &CPU::RLr;
    m_operationMapCB[0x18] = &CPU::RRr;
    m_operationMapCB[0x19] = &CPU::RRr;
    m_operationMapCB[0x1A] = &CPU::RRr;
    m_operationMapCB[0x1B] = &CPU::RRr;
    m_operationMapCB[0x1C] = &CPU::RRr;
    m_operationMapCB[0x1D] = &CPU::RRr;
    m_operationMapCB[0x1E] = &CPU::RR_HL_;
    m_operationMapCB[0x1F] = &CPU::RRr;

    // 20
    m_operationMapCB[0x20] = &CPU::SLAr;
    m_operationMapCB[0x21] = &CPU::SLAr;
    m_operationMapCB[0x22] = &CPU::SLAr;
    m_operationMapCB[0x23] = &CPU::SLAr;
    m_operationMapCB[0x24] = &CPU::SLAr;
    m_operationMapCB[0x25] = &CPU::SLAr;
    m_operationMapCB[0x26] = &CPU::SLA_HL_;
    m_operationMapCB[0x27] = &CPU::SLAr;
    m_operationMapCB[0x28] = &CPU::SRAr;
    m_operationMapCB[0x29] = &CPU::SRAr;
    m_operationMapCB[0x2A] = &CPU::SRAr;
    m_operationMapCB[0x2B] = &CPU::SRAr;
    m_operationMapCB[0x2C] = &CPU::SRAr;
    m_operationMapCB[0x2D] = &CPU::SRAr;
    m_operationMapCB[0x2E] = &CPU::SRA_HL_;
    m_operationMapCB[0x2F] = &CPU::SRAr;

    // 30
    m_operationMapCB[0x30] = &CPU::SWAPr;
    m_operationMapCB[0x31] = &CPU::SWAPr;
    m_operationMapCB[0x32] = &CPU::SWAPr;
    m_operationMapCB[0x33] = &CPU::SWAPr;
    m_operationMapCB[0x34] = &CPU::SWAPr;
    m_operationMapCB[0x35] = &CPU::SWAPr;
    m_operationMapCB[0x36] = &CPU::SWAP_HL_;
    m_operationMapCB[0x37] = &CPU::SWAPr;
    m_operationMapCB[0x38] = &CPU::SRLr;
    m_operationMapCB[0x39] = &CPU::SRLr;
    m_operationMapCB[0x3A] = &CPU::SRLr;
    m_operationMapCB[0x3B] = &CPU::SRLr;
    m_operationMapCB[0x3C] = &CPU::SRLr;
    m_operationMapCB[0x3D] = &CPU::SRLr;
    m_operationMapCB[0x3E] = &CPU::SRL_HL_;
    m_operationMapCB[0x3F] = &CPU::SRLr;

    // 40
    m_operationMapCB[0x40] = &CPU::BITbr;
    m_operationMapCB[0x41] = &CPU::BITbr;
    m_operationMapCB[0x42] = &CPU::BITbr;
    m_operationMapCB[0x43] = &CPU::BITbr;
    m_operationMapCB[0x44] = &CPU::BITbr;
    m_operationMapCB[0x45] = &CPU::BITbr;
    m_operationMapCB[0x46] = &CPU::BITb_HL_;
    m_operationMapCB[0x47] = &CPU::BITbr;
    m_operationMapCB[0x48] = &CPU::BITbr;
    m_operationMapCB[0x49] = &CPU::BITbr;
    m_operationMapCB[0x4A] = &CPU::BITbr;
    m_operationMapCB[0x4B] = &CPU::BITbr;
    m_operationMapCB[0x4C] = &CPU::BITbr;
    m_operationMapCB[0x4D] = &CPU::BITbr;
    m_operationMapCB[0x4E] = &CPU::BITb_HL_;
    m_operationMapCB[0x4F] = &CPU::BITbr;

    // 50
    m_operationMapCB[0x50] = &CPU::BITbr;
    m_operationMapCB[0x51] = &CPU::BITbr;
    m_operationMapCB[0x52] = &CPU::BITbr;
    m_operationMapCB[0x53] = &CPU::BITbr;
    m_operationMapCB[0x54] = &CPU::BITbr;
    m_operationMapCB[0x55] = &CPU::BITbr;
    m_operationMapCB[0x56] = &CPU::BITb_HL_;
    m_operationMapCB[0x57] = &CPU::BITbr;
    m_operationMapCB[0x58] = &CPU::BITbr;
    m_operationMapCB[0x59] = &CPU::BITbr;
    m_operationMapCB[0x5A] = &CPU::BITbr;
    m_operationMapCB[0x5B] = &CPU::BITbr;
    m_operationMapCB[0x5C] = &CPU::BITbr;
    m_operationMapCB[0x5D] = &CPU::BITbr;
    m_operationMapCB[0x5E] = &CPU::BITb_HL_;
    m_operationMapCB[0x5F] = &CPU::BITbr;

    // 60
    m_operationMapCB[0x60] = &CPU::BITbr;
    m_operationMapCB[0x61] = &CPU::BITbr;
    m_operationMapCB[0x62] = &CPU::BITbr;
    m_operationMapCB[0x63] = &CPU::BITbr;
    m_operationMapCB[0x64] = &CPU::BITbr;
    m_operationMapCB[0x65] = &CPU::BITbr;
    m_operationMapCB[0x66] = &CPU::BITb_HL_;
    m_operationMapCB[0x67] = &CPU::BITbr;
    m_operationMapCB[0x68] = &CPU::BITbr;
    m_operationMapCB[0x69] = &CPU::BITbr;
    m_operationMapCB[0x6A] = &CPU::BITbr;
    m_operationMapCB[0x6B] = &CPU::BITbr;
    m_operationMapCB[0x6C] = &CPU::BITbr;
    m_operationMapCB[0x6D] = &CPU::BITbr;
    m_operationMapCB[0x6E] = &CPU::BITb_HL_;
    m_operationMapCB[0x6F] = &CPU::BITbr;

    // 70
    m_operationMapCB[0x70] = &CPU::BITbr;
    m_operationMapCB[0x71] = &CPU::BITbr;
    m_operationMapCB[0x72] = &CPU::BITbr;
    m_operationMapCB[0x73] = &CPU::BITbr;
    m_operationMapCB[0x74] = &CPU::BITbr;
    m_operationMapCB[0x75] = &CPU::BITbr;
    m_operationMapCB[0x76] = &CPU::BITb_HL_;
    m_operationMapCB[0x77] = &CPU::BITbr;
    m_operationMapCB[0x78] = &CPU::BITbr;
    m_operationMapCB[0x79] = &CPU::BITbr;
    m_operationMapCB[0x7A] = &CPU::BITbr;
    m_operationMapCB[0x7B] = &CPU::BITbr;
    m_operationMapCB[0x7C] = &CPU::BITbr;
    m_operationMapCB[0x7D] = &CPU::BITbr;
    m_operationMapCB[0x7E] = &CPU::BITb_HL_;
    m_operationMapCB[0x7F] = &CPU::BITbr;

    // 80
    m_operationMapCB[0x80] = &CPU::RESbr;
    m_operationMapCB[0x81] = &CPU::RESbr;
    m_operationMapCB[0x82] = &CPU::RESbr;
    m_operationMapCB[0x83] = &CPU::RESbr;
    m_operationMapCB[0x84] = &CPU::RESbr;
    m_operationMapCB[0x85] = &CPU::RESbr;
    m_operationMapCB[0x86] = &CPU::RESb_HL_;
    m_operationMapCB[0x87] = &CPU::RESbr;
    m_operationMapCB[0x88] = &CPU::RESbr;
    m_operationMapCB[0x89] = &CPU::RESbr;
    m_operationMapCB[0x8A] = &CPU::RESbr;
    m_operationMapCB[0x8B] = &CPU::RESbr;
    m_operationMapCB[0x8C] = &CPU::RESbr;
    m_operationMapCB[0x8D] = &CPU::RESbr;
    m_operationMapCB[0x8E] = &CPU::RESb_HL_;
    m_operationMapCB[0x8F] = &CPU::RESbr;

    // 90
    m_operationMapCB[0x90] = &CPU::RESbr;
    m_operationMapCB[0x91] = &CPU::RESbr;
    m_operationMapCB[0x92] = &CPU::RESbr;
    m_operationMapCB[0x93] = &CPU::RESbr;
    m_operationMapCB[0x94] = &CPU::RESbr;
    m_operationMapCB[0x95] = &CPU::RESbr;
    m_operationMapCB[0x96] = &CPU::RESb_HL_;
    m_operationMapCB[0x97] = &CPU::RESbr;
    m_operationMapCB[0x98] = &CPU::RESbr;
    m_operationMapCB[0x99] = &CPU::RESbr;
    m_operationMapCB[0x9A] = &CPU::RESbr;
    m_operationMapCB[0x9B] = &CPU::RESbr;
    m_operationMapCB[0x9C] = &CPU::RESbr;
    m_operationMapCB[0x9D] = &CPU::RESbr;
    m_operationMapCB[0x9E] = &CPU::RESb_HL_;
    m_operationMapCB[0x9F] = &CPU::RESbr;

    // A0
    m_operationMapCB[0xA0] = &CPU::RESbr;
    m_operationMapCB[0xA1] = &CPU::RESbr;
    m_operationMapCB[0xA2] = &CPU::RESbr;
    m_operationMapCB[0xA3] = &CPU::RESbr;
    m_operationMapCB[0xA4] = &CPU::RESbr;
    m_operationMapCB[0xA5] = &CPU::RESbr;
    m_operationMapCB[0xA6] = &CPU::RESb_HL_;
    m_operationMapCB[0xA7] = &CPU::RESbr;
    m_operationMapCB[0xA8] = &CPU::RESbr;
    m_operationMapCB[0xA9] = &CPU::RESbr;
    m_operationMapCB[0xAA] = &CPU::RESbr;
    m_operationMapCB[0xAB] = &CPU::RESbr;
    m_operationMapCB[0xAC] = &CPU::RESbr;
    m_operationMapCB[0xAD] = &CPU::RESbr;
    m_operationMapCB[0xAE] = &CPU::RESb_HL_;
    m_operationMapCB[0xAF] = &CPU::RESbr;

    // B0
    m_operationMapCB[0xB0] = &CPU::RESbr;
    m_operationMapCB[0xB1] = &CPU::RESbr;
    m_operationMapCB[0xB2] = &CPU::RESbr;
    m_operationMapCB[0xB3] = &CPU::RESbr;
    m_operationMapCB[0xB4] = &CPU::RESbr;
    m_operationMapCB[0xB5] = &CPU::RESbr;
    m_operationMapCB[0xB6] = &CPU::RESb_HL_;
    m_operationMapCB[0xB7] = &CPU::RESbr;
    m_operationMapCB[0xB8] = &CPU::RESbr;
    m_operationMapCB[0xB9] = &CPU::RESbr;
    m_operationMapCB[0xBA] = &CPU::RESbr;
    m_operationMapCB[0xBB] = &CPU::RESbr;
    m_operationMapCB[0xBC] = &CPU::RESbr;
    m_operationMapCB[0xBD] = &CPU::RESbr;
    m_operationMapCB[0xBE] = &CPU::RESb_HL_;
    m_operationMapCB[0xBF] = &CPU::RESbr;

    // C0
    m_operationMapCB[0xC0] = &CPU::SETbr;
    m_operationMapCB[0xC1] = &CPU::SETbr;
    m_operationMapCB[0xC2] = &CPU::SETbr;
    m_operationMapCB[0xC3] = &CPU::SETbr;
    m_operationMapCB[0xC4] = &CPU::SETbr;
    m_operationMapCB[0xC5] = &CPU::SETbr;
    m_operationMapCB[0xC6] = &CPU::SETb_HL_;
    m_operationMapCB[0xC7] = &CPU::SETbr;
    m_operationMapCB[0xC8] = &CPU::SETbr;
    m_operationMapCB[0xC9] = &CPU::SETbr;
    m_operationMapCB[0xCA] = &CPU::SETbr;
    m_operationMapCB[0xCB] = &CPU::SETbr;
    m_operationMapCB[0xCC] = &CPU::SETbr;
    m_operationMapCB[0xCD] = &CPU::SETbr;
    m_operationMapCB[0xCE] = &CPU::SETb_HL_;
    m_operationMapCB[0xCF] = &CPU::SETbr;

    // D0
    m_operationMapCB[0xD0] = &CPU::SETbr;
    m_operationMapCB[0xD1] = &CPU::SETbr;
    m_operationMapCB[0xD2] = &CPU::SETbr;
    m_operationMapCB[0xD3] = &CPU::SETbr;
    m_operationMapCB[0xD4] = &CPU::SETbr;
    m_operationMapCB[0xD5] = &CPU::SETbr;
    m_operationMapCB[0xD6] = &CPU::SETb_HL_;
    m_operationMapCB[0xD7] = &CPU::SETbr;
    m_operationMapCB[0xD8] = &CPU::SETbr;
    m_operationMapCB[0xD9] = &CPU::SETbr;
    m_operationMapCB[0xDA] = &CPU::SETbr;
    m_operationMapCB[0xDB] = &CPU::SETbr;
    m_operationMapCB[0xDC] = &CPU::SETbr;
    m_operationMapCB[0xDD] = &CPU::SETbr;
    m_operationMapCB[0xDE] = &CPU::SETb_HL_;
    m_operationMapCB[0xDF] = &CPU::SETbr;

    // E0
    m_operationMapCB[0xE0] = &CPU::SETbr;
    m_operationMapCB[0xE1] = &CPU::SETbr;
    m_operationMapCB[0xE2] = &CPU::SETbr;
    m_operationMapCB[0xE3] = &CPU::SETbr;
    m_operationMapCB[0xE4] = &CPU::SETbr;
    m_operationMapCB[0xE5] = &CPU::SETbr;
    m_operationMapCB[0xE6] = &CPU::SETb_HL_;
    m_operationMapCB[0xE7] = &CPU::SETbr;
    m_operationMapCB[0xE8] = &CPU::SETbr;
    m_operationMapCB[0xE9] = &CPU::SETbr;
    m_operationMapCB[0xEA] = &CPU::SETbr;
    m_operationMapCB[0xEB] = &CPU::SETbr;
    m_operationMapCB[0xEC] = &CPU::SETbr;
    m_operationMapCB[0xED] = &CPU::SETbr;
    m_operationMapCB[0xEE] = &CPU::SETb_HL_;
    m_operationMapCB[0xEF] = &CPU::SETbr;

    // F0
    m_operationMapCB[0xF0] = &CPU::SETbr;
    m_operationMapCB[0xF1] = &CPU::SETbr;
    m_operationMapCB[0xF2] = &CPU::SETbr;
    m_operationMapCB[0xF3] = &CPU::SETbr;
    m_operationMapCB[0xF4] = &CPU::SETbr;
    m_operationMapCB[0xF5] = &CPU::SETbr;
    m_operationMapCB[0xF6] = &CPU::SETb_HL_;
    m_operationMapCB[0xF7] = &CPU::SETbr;
    m_operationMapCB[0xF8] = &CPU::SETbr;
    m_operationMapCB[0xF9] = &CPU::SETbr;
    m_operationMapCB[0xFA] = &CPU::SETbr;
    m_operationMapCB[0xFB] = &CPU::SETbr;
    m_operationMapCB[0xFC] = &CPU::SETbr;
    m_operationMapCB[0xFD] = &CPU::SETbr;
    m_operationMapCB[0xFE] = &CPU::SETb_HL_;
    m_operationMapCB[0xFF] = &CPU::SETbr;

    // Initialize the register map
    /*
    I'm not 100% sure about this, so ensure the lookup is right for each opcode before using
    GetByteRegister.

    000 B
    001 C
    010 D
    011 E
    100 H
    101 L
    110 ? (F?)
    111 A
    */
    m_ByteRegisterMap[0x00] = reinterpret_cast<byte*>(&m_BC) + 1;   // ushort memory is [C][B]
    m_ByteRegisterMap[0x01] = reinterpret_cast<byte*>(&m_BC);
    m_ByteRegisterMap[0x02] = reinterpret_cast<byte*>(&m_DE) + 1;
    m_ByteRegisterMap[0x03] = reinterpret_cast<byte*>(&m_DE);
    m_ByteRegisterMap[0x04] = reinterpret_cast<byte*>(&m_HL) + 1;
    m_ByteRegisterMap[0x05] = reinterpret_cast<byte*>(&m_HL);
    m_ByteRegisterMap[0x06] = reinterpret_cast<byte*>(&m_AF);       // Should not be used (F)
    m_ByteRegisterMap[0x07] = reinterpret_cast<byte*>(&m_AF) + 1;

    /*
    I'm not 100% sure about this, so ensure the lookup is right for each opcode before using
    GetUShortRegister.

    00 = BC
    01 = DE
    10 = HL
    11 = SP
    */
    m_UShortRegisterMap[0x00] = &m_BC;
    m_UShortRegisterMap[0x01] = &m_DE;
    m_UShortRegisterMap[0x02] = &m_HL;
    m_UShortRegisterMap[0x03] = &m_SP;
}

CPU::~CPU()
{
    m_timer.reset();
    m_serial.reset();
    m_joypad.reset();
    m_APU.reset();
    m_GPU.reset();
    m_cartridge.reset();
    m_MMU.reset();
}

bool CPU::Initialize(IMMU* pMMU, bool isFromTest)
{
    // Create the MMU
    m_MMU = std::unique_ptr<IMMU>(pMMU);

    if (!isFromTest)
    {
        // Create the Cartridge
        m_cartridge = std::make_unique<Cartridge>();

        // Create the GPU
        m_GPU = std::unique_ptr<GPU>(new GPU(pMMU, this));

        // Create the APU
        m_APU = std::make_unique<APU>();

        // Create the Joypad
        m_joypad = std::make_unique<Joypad>(this);

        // Create the Serial
        m_serial = std::make_unique<Serial>();

        // Create the Timer
        m_timer = std::unique_ptr<Timer>(new Timer(this));

        m_MMU->RegisterMemoryUnit(0x0000, 0x7FFF, m_cartridge.get());
        m_MMU->RegisterMemoryUnit(0x8000, 0x9FFF, m_GPU.get());
        m_MMU->RegisterMemoryUnit(0xA000, 0xBFFF, m_cartridge.get());
        m_MMU->RegisterMemoryUnit(0xFE00, 0xFE9F, m_GPU.get());
        // 0xFEA0-0xFEFF - Unusable
        m_MMU->RegisterMemoryUnit(0xFF00, 0xFF00, m_joypad.get());
        m_MMU->RegisterMemoryUnit(0xFF01, 0xFF02, m_serial.get());
        m_MMU->RegisterMemoryUnit(0xFF04, 0xFF07, m_timer.get());
        m_MMU->RegisterMemoryUnit(0xFF10, 0xFF3F, m_APU.get());
        m_MMU->RegisterMemoryUnit(0xFF40, 0xFF4C, m_GPU.get());
        m_MMU->RegisterMemoryUnit(0xFF4E, 0xFF4F, m_GPU.get());
        // 0xFF50 - MMU
        m_MMU->RegisterMemoryUnit(0xFF51, 0xFF55, m_GPU.get());
        m_MMU->RegisterMemoryUnit(0xFF57, 0xFF6B, m_GPU.get());
        m_MMU->RegisterMemoryUnit(0xFF6D, 0xFF6F, m_GPU.get());
    }

    return true;
}

bool CPU::Initialize()
{
    return Initialize(new MMU(), false);
}

bool CPU::LoadROM(const char* path)
{
    return m_cartridge->LoadROM(path);
}

void CPU::StepFrame()
{
    while (m_cycles < CyclesPerFrame)
    {
        Step(); // Execute the current instruction
    }

    // Reset the cycles. If we went over our max cycles, the next frame will start a
    // few cycles ahead.
    m_cycles -= CyclesPerFrame;
}

void CPU::TriggerInterrupt(byte interrupt)
{
    byte IF = m_MMU->ReadByte(0xFF0F);
    if (interrupt == INT40) IF = SETBIT(IF, 0);
    else if (interrupt == INT48) IF = SETBIT(IF, 1);
    else if (interrupt == INT50) IF = SETBIT(IF, 2);
    else if (interrupt == INT58) IF = SETBIT(IF, 3);
    else if (interrupt == INT60) IF = SETBIT(IF, 4);

    m_MMU->WriteByte(0xFF0F, IF);
}

byte* CPU::GetCurrentFrame()
{
    return m_GPU->GetCurrentFrame();
}

void CPU::SetInput(byte input, byte buttons)
{
    m_joypad->SetInput(input, buttons);
}

void CPU::SetVSyncCallback(void(*pCallback)())
{
    m_GPU->SetVSyncCallback(pCallback);
}

void CPU::Step()
{
    unsigned long preCycles = m_cycles;

    // Let's handle interrupts before we do anything
    HandleInterrupts();

    if (m_isHalted)
    {
        NOP(0x00);

        if (m_IFWhenHalted != m_MMU->ReadByte(0xFF0F))
        {
            // We received an interrupt, resume
            m_isHalted = false;
        }
    }
    else
    {
        ushort addr = m_PC;
        // Read through the memory, starting at m_PC
        byte opCode = ReadBytePC();
        opCodeFunction instruction; // Execute the correct function for each OpCode

        if (opCode == 0xCB)
        {
            opCode = ReadBytePC();
            instruction = m_operationMapCB[opCode];
        }
        else
        {
            instruction = m_operationMap[opCode];
        }

        if (instruction != nullptr)
        {
            (this->*instruction)(opCode);
            Trace t = { opCode, m_AF, m_BC, m_DE, m_HL, m_SP, m_PC };
            trace.push_back(t);
            if (trace.size() > 10)
            {
                trace.erase(trace.begin());
            }
        }
        else
        {
            Logger::LogError("OpCode 0x%02X at address 0x%04X could not be interpreted.", opCode, addr);
            HALT(0x76);
        }
    }

    unsigned long elapsedCycles = m_cycles - preCycles;

    if (m_GPU != nullptr)
    {
        // Step GPU by # of elapsed cycles
        m_GPU->Step(elapsedCycles);
    }

    if (m_timer != nullptr)
    {
        // Step the timer by the # of elapsed cycles
        m_timer->Step(elapsedCycles);
    }

    if (m_APU != nullptr)
    {
        // Step the audio processing unit by the # of elapsed cycles
        m_APU->Step(elapsedCycles);
    }
}

byte CPU::GetHighByte(ushort dest)
{
    return ((dest >> 8) & 0xFF);
}

byte CPU::GetLowByte(ushort dest)
{
    return (dest & 0xFF);
}

byte* CPU::GetByteRegister(byte val)
{
    // Bottom 3 bits only
    return m_ByteRegisterMap[val & 0x07];
}

ushort* CPU::GetUShortRegister(byte val, bool useAF)
{
    // Some instructions (PUSH rr and POP rr) use a dumb alternate mapping,
    // which replaces 0x03 (normally SP) with AF.
    if ((val & 0x03) == 0x03)
    {
        return useAF ? &m_AF : m_UShortRegisterMap[0x03];
    }
    else
    {
        // Bottom 2 bits only
        return m_UShortRegisterMap[val & 0x03];
    }
}

void CPU::SetHighByte(ushort* dest, byte val)
{
    byte low = GetLowByte(*dest);
    *dest = (val << 8) | low;
}

void CPU::SetLowByte(ushort* dest, byte val)
{
    byte high = GetHighByte(*dest);
    *dest = (high << 8) | val;
}

void CPU::SetFlag(byte flag)
{
    // This shifts the bit to the left to where the flag is
    // Then ORs it with the Flag register.
    // Finally it filters out the lower 4 bits, as they aren't used on the Gameboy
    SetLowByte(&m_AF, SETBIT(GetLowByte(m_AF), flag) & 0xF0);
}

void CPU::ClearFlag(byte flag)
{
    // This shifts the bit to the left to where the flag is
    // Then it inverts all of the bits
    // Then ANDs it with the Flag register.
    // Finally it filters out the lower 4 bits, as they aren't used on the Gameboy
    SetLowByte(&m_AF, CLEARBIT(GetLowByte(m_AF), flag) & 0xF0);
}

bool CPU::IsFlagSet(byte flag)
{
    return ISBITSET(GetLowByte(m_AF), flag);
}

bool CPU::LookupAndCheckFlag(byte value)
{
    /*
    000 NZ
    001 Z
    010 NC
    011 C
    */
    switch (value)
    {
    case 0x00:
        return !IsFlagSet(ZeroFlag);
    case 0x01:
        return IsFlagSet(ZeroFlag);
    case 0x02:
        return !IsFlagSet(CarryFlag);
    case 0x03:
        return IsFlagSet(CarryFlag);
    }

    return false;
}

void CPU::PushByteToSP(byte val)
{
    m_SP--;
    m_MMU->WriteByte(m_SP, val);
}

void CPU::PushUShortToSP(ushort val)
{
    PushByteToSP(GetHighByte(val));
    PushByteToSP(GetLowByte(val));
}

ushort CPU::PopUShort()
{
    ushort val = m_MMU->ReadUShort(m_SP);
    m_SP += 2;
    return val;
}

byte CPU::PopByte()
{
    byte val = m_MMU->ReadByte(m_SP);
    m_SP++;
    return val;
}

byte CPU::ReadBytePC()
{
    byte val = m_MMU->ReadByte(m_PC);
    m_PC++;
    return val;
}

ushort CPU::ReadUShortPC()
{
    ushort val = m_MMU->ReadUShort(m_PC);
    m_PC += 2;
    return val;
}

void CPU::HandleInterrupts()
{
    // If the IME is enabled, some interrupts are enabled in IE, and
    // an interrupt flag is set, handle the interrupt.
    if (m_IME == 0x01)
    {
        byte IE = m_MMU->ReadByte(0xFFFF);
        byte IF = m_MMU->ReadByte(0xFF0F);

        // This will only match valid interrupts
        byte activeInterrupts = ((IE & IF) & 0x0F);
        if (activeInterrupts > 0x00)
        {
            m_IME = 0x00; // Disable further interrupts

            PushUShortToSP(m_PC); // Push current PC onto stack

            // Jump to the correct handler
            if (ISBITSET(activeInterrupts, 0))
            {
                // VBlank
                m_PC = INT40;
                IF = CLEARBIT(IF, 0);
            }
            else if (ISBITSET(activeInterrupts, 1))
            {
                // LCD status
                m_PC = INT48;
                IF = CLEARBIT(IF, 1);
            }
            else if (ISBITSET(activeInterrupts, 2))
            {
                // Timer
                m_PC = INT50;
                IF = CLEARBIT(IF, 2);
            }
            else if (ISBITSET(activeInterrupts, 3))
            {
                // Serial
                m_PC = INT58;
                IF = CLEARBIT(IF, 3);
            }
            else if (ISBITSET(activeInterrupts, 4))
            {
                // Joypad
                m_PC = INT60;
                IF = CLEARBIT(IF, 4);
            }

            m_MMU->WriteByte(0xFF0F, IF);
        }
    }
}

/*
    CPU INSTRUCTION MAP
*/

// 0x00 (NOP)
void CPU::NOP(const byte& opCode)
{
    m_cycles += 4;

    // No flags affected
}

/*
LD (bc), a
00000010

The contents of the accumulator are loaded to the memory location specified by
the contents of the register pair BC.

8 Cycles

Flags affected(znhc): ----
*/
void CPU::LD_BC_A(const byte& opCode)
{
    m_MMU->WriteByte(m_BC, GetHighByte(m_AF));
    m_cycles += 8;
}

/*
RLCA
00000111

The contents of the accumulator are rotated left 1-bit position. Bit 7
is copied to the carry flag and also to bit 0.

4 Cycles

Flags affected(znhc): 000c
*/
void CPU::RLCA(const byte& opCode)
{
    byte r = GetHighByte(m_AF);

    // Grab bit 7 and store it in the carryflag
    ISBITSET(r, 7) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r left
    r = r << 1;

    // Set bit 0 of r to the old CarryFlag
    r = IsFlagSet(CarryFlag) ? SETBIT(r, 0) : CLEARBIT(r, 0);

    SetHighByte(&m_AF, r);

    // Clear sZ, clears N, clears H, affects C
    ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 4;
}

/*
    LD r, n
    00rrr110 nnnnnnnn

    The 8-bit integer n is loaded to any register r, where r identifies register
    A, B, C, D, E, H, or L.

    8 Cycles

    Flags affected(znhc): ----
*/
void CPU::LDrn(const byte& opCode)
{
    byte n = ReadBytePC();
    byte* r = GetByteRegister(opCode >> 3);
    (*r) = n;

    m_cycles += 8;
}

/*
    LD r, R
    01rrrRRR

    The contents of any register R are loaded into another other register r, where
    R and r identify a register A, B, C, D, E, H, or L.

    4 Cycles

    Flags affected(znhc): ----
*/
void CPU::LDrR(const byte& opCode)
{
    byte* r = GetByteRegister(opCode >> 3);
    byte* R = GetByteRegister(opCode);
    (*r) = *R;

    m_cycles += 4;
}

/*
    LD r, (hl)
    01rrr110

    The contents of the memory location (HL) are loaded into register r, where
    r identifies a register A, B, C, D, E, H, or L.

    8 Cycles

    Flags affected(znhc): ----
*/
void CPU::LDr_HL_(const byte& opCode)
{
    byte* r = GetByteRegister(opCode >> 3);

    (*r) = m_MMU->ReadByte(m_HL);

    m_cycles += 8;
}

/*
    LD (HL), r
    01110rrr

    The contents of register r are loaded into the memory locoation specifed by the
    contents of the HL register pair. The operand r identifies register A, B, C, D, E,
    H, or L.

    8 Cycles

    Flags affected(znhc): ----
*/
void CPU::LD_HL_r(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);
    m_MMU->WriteByte(m_HL, (*r)); // Load r into the address pointed at by HL.

    m_cycles += 8;
}


/*
    LD rr, nn
    00rr0001 nnnnnnnn nnnnnnnn

    The 2-byte integer nn is loaded into the rr register pair, where rr defines
    the BC, DL, HL, or SP register pairs.

    12 Cycles

    Flags affected(znhc): ----
*/
void CPU::LDrrnn(const byte& opCode)
{
    ushort* rr = GetUShortRegister(opCode >> 4, false);
    ushort nn = ReadUShortPC(); // Read nn
    (*rr) = nn;

    m_cycles += 12;
}

/*
    INC r
    00rrr100

    Register r is incremented, where r identifies register A, B, C, D, E, H, or L.

    4 Cycles

    Flags affected(znhc): z0h-
    Affects Z, Clears N, affects H
*/
void CPU::INCr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode >> 3);
    bool isBit3Before = ISBITSET(*r, 3);
    *r += 1;
    bool isBit3After = ISBITSET(*r, 3);

    if (*r == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(SubtractFlag);

    if (isBit3Before && !isBit3After)
    {
        SetFlag(HalfCarryFlag);
    }
    else
    {
        ClearFlag(HalfCarryFlag);
    }

    m_cycles += 4;
}

/*
CALL cc, nn
11ccc100

000 NZ
001 Z
010 NC
011 C

24 Cycles if taken
12 Cycles if not taken

Flags affected(znhc): ----
*/
void CPU::CALLccnn(const byte& opCode)
{
    ushort nn = ReadUShortPC();

    bool check = false;
    switch ((opCode >> 3) & 0x03)
    {
    case 0x00:  // NZ
        check = !IsFlagSet(ZeroFlag);
        break;
    case 0x01:  // Z
        check = IsFlagSet(ZeroFlag);
        break;
    case 0x02:  // NC
        check = !IsFlagSet(CarryFlag);
        break;
    case 0x03:  // C
        check = IsFlagSet(CarryFlag);
        break;
    }

    if (check)
    {
        m_cycles += 24;
        PushUShortToSP(m_PC);
        m_PC = nn;
    }
    else
    {
        m_cycles += 12;
    }
}

/*
RET cc
11ccc000

000 NZ
001 Z
010 NC
011 C

20 Cycles if taken
8 Cycles if not taken

Flags affected(znhc): ----
*/
void CPU::RETcc(const byte& opCode)
{
    bool check = false;
    switch ((opCode >> 3) & 0x03)
    {
    case 0x00:  // NZ
        check = !IsFlagSet(ZeroFlag);
        break;
    case 0x01:  // Z
        check = IsFlagSet(ZeroFlag);
        break;
    case 0x02:  // NC
        check = !IsFlagSet(CarryFlag);
        break;
    case 0x03:  // C
        check = IsFlagSet(CarryFlag);
        break;
    }

    if (check)
    {
        m_cycles += 20;
        m_PC = PopUShort();
    }
    else
    {
        m_cycles += 8;
    }
}

/*
    LD (nn), SP - 0x08

    The contents of the stack pointer are loaded into the address specified by the
    operand nn.

    20 Cycles

    Flags affected(znhc): ----
*/
void CPU::LD_nn_SP(const byte& opCode)
{
    ushort nn = ReadUShortPC();

    // Load A into (nn)
    m_MMU->WriteByte(nn + 1, GetHighByte(m_SP));
    m_MMU->WriteByte(nn, GetLowByte(m_SP));

    m_cycles += 20;
}

/*
ADD HL, ss
00ss1001

The contents of the register pair ss (BC, DE, HL, SP) are added to the contents
of the register pair HL and the result is stored in HL.

8 Cycles

Flags affected(znhc): -0hc
Clears N, affects H, affects C
*/
void CPU::ADDHLss(const byte& opCode)
{
    ushort* ss = GetUShortRegister(opCode >> 4, false);

    ushort result = (m_HL + *ss);

    ClearFlag(SubtractFlag);
    ((ISBITSET(m_HL, 11))) && (!ISBITSET(result, 11)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    ((ISBITSET(m_HL, 15))) && (!ISBITSET(result, 15)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_HL = result;

    m_cycles += 8;
}

/*
    ADD SP, dd
    0xE8

    16 Cycles

    Flags affected(znhc): 00hc
*/
void CPU::ADDSPdd(const byte& opCode)
{
    sbyte arg = static_cast<sbyte>(ReadBytePC());
    ushort result = (m_SP + arg);

    ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ((result & 0xF) < (m_SP & 0xF)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    ((result & 0xFF) < (m_SP & 0xFF)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_SP = result;

    m_cycles += 16;
}

/*
JP cc, nn
11ccc010

000 NZ
001 Z
010 NC
011 C

16 Cycles if taken
12 Cycles if not taken

Flags affected(znhc): ----
*/
void CPU::JPccnn(const byte& opCode)
{
    ushort nn = ReadUShortPC();

    bool check = false;
    switch ((opCode >> 3) & 0x03)
    {
    case 0x00:  // NZ
        check = !IsFlagSet(ZeroFlag);
        break;
    case 0x01:  // Z
        check = IsFlagSet(ZeroFlag);
        break;
    case 0x02:  // NC
        check = !IsFlagSet(CarryFlag);
        break;
    case 0x03:  // C
        check = IsFlagSet(CarryFlag);
        break;
    }

    if (check)
    {
        m_cycles += 16;
        m_PC = nn;
    }
    else
    {
        m_cycles += 12;
    }
}

/*
    ADD A, r
    10000rrr

    The contents of the r register are added to the contents of the accumulator,
    and the result is stored in the accumulator. The operand r identifies the registers
    A, B, C, D, E, H, or L.

    4 Cycles

    Flags affected(znhc): z0hc
*/
void CPU::ADDAr(const byte& opCode)
{
    byte A = GetHighByte(m_AF);
    byte* r = GetByteRegister(opCode);
    byte result = A + (*r);
    SetHighByte(&m_AF, result);

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ((ISBITSET(A, 3))) && (!ISBITSET(result, 3)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    ((ISBITSET(A, 7))) && (!ISBITSET(result, 7)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_cycles += 4;
}

/*
    ADC A, r
    10001rrr

    The contents of the r register, and the contents of the carry flag,
    are added to the contents of the accumulator, and the result is stored
    in the accumulator. The operand r identifies the registers A, B, C, D, E, H, or L.

    4 Cycles

    Flags affected(znhc): z0hc
*/
void CPU::ADCAr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);
    byte A = GetHighByte(m_AF);
    byte C = (IsFlagSet(CarryFlag)) ? 0x01 : 0x00;
    byte result = A + (*r) + C;
    SetHighByte(&m_AF, result);

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    (ISBITSET(A, 3) && !ISBITSET(result, 3)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    (ISBITSET(A, 7) && !ISBITSET(result, 7)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_cycles += 4;
}

/*
    JR cc, nn
    001cc000

    00 NZ
    01 Z
    10 NC
    11 C

    12 Cycles if taken
    8 Cycles if not taken

    Flags affected(znhc): ----
*/
void CPU::JRcce(const byte& opCode)
{
    sbyte arg = static_cast<sbyte>(ReadBytePC());

    bool check = false;
    switch ((opCode >> 3) & 0x03)
    {
    case 0x00:  // NZ
        check = !IsFlagSet(ZeroFlag);
        break;
    case 0x01:  // Z
        check = IsFlagSet(ZeroFlag);
        break;
    case 0x02:  // NC
        check = !IsFlagSet(CarryFlag);
        break;
    case 0x03:  // C
        check = IsFlagSet(CarryFlag);
        break;
    }

    if (check)
    {
        m_PC += arg;
        m_cycles += 12;
    }
    else
    {
        m_cycles += 8;
    }
}

/*
RST
11ttt111

000 0x00
001 0x08
010 0x10
011 0x18
100 0x20
101 0x28
110 0x30
111 0x38

16 Cycles if taken

Flags affected(znhc): ----
*/
void CPU::RSTn(const byte& opCode)
{
    byte t = ((opCode >> 3) & 0x07);

    m_PC = (ushort)(t * 0x08);
    m_cycles += 16;
}

/*
    AND r
    10100rrr

    The logical AND operation is performed between the register specified in the r
    operand and the byte contained in the accumulator. The result is stored in the accumulator.
    Register r can be A, B, C, D, E, H, or L.

    4 Cycles

    Flags affected(znhc): z010
*/
void CPU::ANDr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);
    byte result = (*r) & GetHighByte(m_AF);
    SetHighByte(&m_AF, result);

    if (result == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(SubtractFlag);
    SetFlag(HalfCarryFlag);
    ClearFlag(CarryFlag);

    m_cycles += 4;
}

/*
    CP r
    10111rrr

    The contents of 8-bit register r are compared with the contents of the accumulator.
    If there is a true compare, the Z flag is set. The execution of this instruction
    does not affect the contents of the accumulator.

    4 Cycles

    Flags affected(znhc): z1hc
*/
void CPU::CPr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);
    byte A = GetHighByte(m_AF);
    byte result = A - (*r);

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    SetFlag(SubtractFlag);
    ((A & 0xFF) < ((*r) & 0xFF)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);
    ((A & 0x0F) < ((*r) & 0x0F)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);

    m_cycles += 4;
}

/*
    INC rr
    00rr0011

    16-bit register rr is incremented, where rr identifies register pairs BC, DE, HL, or SP.

    8 Cycles

    Flags affected(znhc): ----
*/
void CPU::INCrr(const byte& opCode)
{
    ushort* rr = GetUShortRegister(opCode >> 4, false);
    *rr += 1;

    m_cycles += 8;
}

/*
DEC rr
00rr1011

16-bit register rr is decremented, where rr identifies register pairs BC, DE, HL, or SP.

8 Cycles

Flags affected(znhc): ----
*/
void CPU::DECrr(const byte& opCode)
{
    ushort* rr = GetUShortRegister(opCode >> 4, false);
    *rr -= 1;

    m_cycles += 8;
}

/*
    XOR r
    10101rrr

    The logical exclusive-OR operation is performed between the register specified in the r
    operand and the byte contained in the accumulator. The result is stored in the accumulator.
    Register r can be A, B, C, D, E, H, or L.

    4 Cycles

    Flags affected(znhc): z000
    Affects Z, clears n, clears h, clears c
*/
void CPU::XORr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);
    SetHighByte(&m_AF, *r ^ GetHighByte(m_AF));

    // Affects Z and clears NHC
    if (GetHighByte(m_AF) == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);
    ClearFlag(CarryFlag);

    m_cycles += 4;
}

/*
XOR (HL) - 0xAE

The logical exclusive-OR operation is performed between the byte pointed to by the HL register
and the byte contained in the accumulator. The result is stored in the accumulator.

8 Cycles

Flags affected(znhc): z000
Affects Z, clears n, clears h, clears c
*/
void CPU::XOR_HL_(const byte& opCode)
{
    byte r = m_MMU->ReadByte(m_HL);
    SetHighByte(&m_AF, r ^ GetHighByte(m_AF));

    // Affects Z and clears NHC
    if (GetHighByte(m_AF) == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);
    ClearFlag(CarryFlag);

    m_cycles += 8;
}

/*
OR r
10110rrr

The logical OR operation is performed between the register specified in the r
operand and the byte contained in the accumulator. The result is stored in the accumulator.
Register r can be A, B, C, D, E, H, or L.

4 Cycles

Flags affected(znhc): z000
Affects Z, clears n, clears h, clears c
*/
void CPU::ORr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);
    SetHighByte(&m_AF, *r | GetHighByte(m_AF));

    // Affects Z and clears NHC
    if (GetHighByte(m_AF) == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);
    ClearFlag(CarryFlag);

    m_cycles += 4;
}

/*
OR (HL) - 0xB6

The logical OR operation is performed between the value at memory location (HL)
and the byte contained in the accumulator. The result is stored in the accumulator.

8 Cycles

Flags affected(znhc): z000
Affects Z, clears n, clears h, clears c
*/
void CPU::OR_HL_(const byte& opCode)
{
    byte r = m_MMU->ReadByte(m_HL);
    SetHighByte(&m_AF, r | GetHighByte(m_AF));

    // Affects Z and clears NHC
    if (GetHighByte(m_AF) == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);
    ClearFlag(CarryFlag);

    m_cycles += 8;
}

/*
    PUSH rr
    11qq0101

    The contents of the register pair rr are pushed to the external memory stack.
    The stack pointer holds the 16-bit address of the current top of the stack.
    This instruction first decrements SP and loads the high order byte of register
    pair rr to the memory address specified by the SP. The SP is decremented again,
    and then the low order byte is then loaded to the new memory address. The operand
    rr identifies register pair BC, DE, HL, or AF.

    16 Cycles

    Flags affected(znhc): ----
*/
void CPU::PUSHrr(const byte& opCode)
{
    ushort* rr = GetUShortRegister(opCode >> 4, true);
    PushUShortToSP(*rr);

    m_cycles += 16;
}

/*
AND n
11100110 (0xE6)

The logical AND operation is performed between the byte specified in n and the byte contained
in the accumulator. The resutl is stored in the accumulator.

8 Cycles

Flags affected(znhc): z010
Affects Z, clears n, sets h, clears c
*/
void CPU::ANDn(const byte& opCode)
{
    byte n = ReadBytePC();

    byte result = GetHighByte(m_AF) & n;
    SetHighByte(&m_AF, result);

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    SetFlag(HalfCarryFlag);
    ClearFlag(CarryFlag);

    m_cycles += 8;
}

/*
JP HL - 0xE9

The PC is loaded with the value of HL.

4 Cycles

Flags affected(znhc): ----
*/
void CPU::JP_HL_(const byte& opCode)
{
    m_PC = m_HL;
    m_cycles += 4;
}

/*
    POP rr
    11qq0001

    The top two bytes of the external memory stack are popped into register pair qq.
    The stack pointer holds the 16-bit address of the current top of the stack. This
    instruction first loads to the low order portion of rr. The SP is incremented and
    the contents of the corresponding adjacent memory ocation are loaded into the high
    order portion of rr. The SP is then incremented again. The operand rr identifies
    register pair BC, DE, HL, or AF.

    12 Cycles

    Flags affected(znhc): ----
*/
void CPU::POPrr(const byte& opCode)
{
    ushort* rr = GetUShortRegister(opCode >> 4, true);
    (*rr) = PopUShort();

    if (((opCode >> 4) & 0x03) == 0x03)
    {
        (*rr) &= 0xFFF0;
    }

    m_cycles += 12;
}

/*
    DEC r
    00rrr101

    Register r is decremented, where r identifies register A, B, C, D, E, H, or L.

    4 Cycles

    Flags affected(znhc): z1h-
*/
void CPU::DECr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode >> 3);
    bool isBit4Before = ISBITSET(*r, 4);
    *r -= 1;
    bool isBit4After = ISBITSET(*r, 4);

    if (*r == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    SetFlag(SubtractFlag);

    if (!isBit4Before && isBit4After)
    {
        SetFlag(HalfCarryFlag);
    }
    else
    {
        ClearFlag(HalfCarryFlag);
    }

    m_cycles += 4;
}

/*
    INC (HL) - 0x34

    The byte at the address (HL) is incremented.

    12 Cycles

    Flags affected(znhc): z0h-
*/
void CPU::INC_HL_(const byte& opCode)
{
    byte HL = m_MMU->ReadByte(m_HL);
    bool isBit3Before = ISBITSET(HL, 3);
    HL += 1;
    bool isBit3After = ISBITSET(HL, 3);

    m_MMU->WriteByte(m_HL, HL);

    if (HL == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(SubtractFlag);

    if (isBit3Before && !isBit3After)
    {
        SetFlag(HalfCarryFlag);
    }
    else
    {
        ClearFlag(HalfCarryFlag);
    }

    m_cycles += 12;
}

/*
DEC (hl) - 0x35

The byte at the address (HL) is decremented.

12 Cycles

Flags affected(znhc): z1h-
*/
void CPU::DEC_HL_(const byte& opCode)
{
    byte r = m_MMU->ReadByte(m_HL);
    bool isBit4Before = ISBITSET(r, 4);
    r -= 1;
    bool isBit4After = ISBITSET(r, 4);

    m_MMU->WriteByte(m_HL, r);

    if (r == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    SetFlag(SubtractFlag);

    if (!isBit4Before && isBit4After)
    {
        SetFlag(HalfCarryFlag);
    }
    else
    {
        ClearFlag(HalfCarryFlag);
    }

    m_cycles += 12;
}

/*
    LD (HL), n
    0x36

    The contents of n are loaded into the memory location specifed by the
    contents of the HL register pair.

    12 Cycles

    Flags affected(znhc): ----
*/
void CPU::LD_HL_n(const byte& opCode)
{
    byte n = ReadBytePC();
    m_MMU->WriteByte(m_HL, n); // Load n into the address pointed at by HL.

    m_cycles += 12;
}

/*
SCF - 0x37

Sets the carry flag.

4 Cycles

Flags affected(znhc): -001
Clears N, Clears h, Sets c
*/
void CPU::SCF(const byte& opCode)
{
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);
    SetFlag(CarryFlag);

    m_cycles += 4;
}

/*
    CCF - 0x3F

    Toggle the carry flag.

    4 Cycles

    Flags affected(znhc): -00c
*/
void CPU::CCF(const byte& opCode)
{
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);
    IsFlagSet(CarryFlag) ? ClearFlag(CarryFlag) : SetFlag(CarryFlag);

    m_cycles += 4;
}

/*
    SUB r
    10010rrr

    The 8-bit register r is subtracted from the contents of the Accumulator, and the
    result is stored in the accumulator.

    4 Cycles

    Flags affected(znhc): z1hc
*/
void CPU::SUBr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);
    byte A = GetHighByte(m_AF);
    byte result = A - (*r);
    SetHighByte(&m_AF, result);

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    SetFlag(SubtractFlag);
    ((A & 0x0F) < ((*r) & 0x0F)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    ((A & 0xFF) < ((*r) & 0xFF)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_cycles += 4;
}

// 0x10 (STOP)
void CPU::STOP(const byte& opCode)
{
    // For the emulator, these are effectively the same thing
    HALT(opCode);
}

/*
LD (de), a
00010010

The contents of the accumulator are loaded to the memory location specified by
the contents of the register pair DE.

8 Cycles

Flags affected(znhc): ----
*/
void CPU::LD_DE_A(const byte& opCode)
{
    m_MMU->WriteByte(m_DE, GetHighByte(m_AF));
    m_cycles += 8;
}

// 0x17 (RL A)
void CPU::RLA(const byte& opCode)
{
    // Grab the current CarryFlag val
    bool carry = IsFlagSet(CarryFlag);

    // Grab bit 7 and store it in the carryflag
    ISBITSET(GetHighByte(m_AF), 7) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift A left
    SetHighByte(&m_AF, GetHighByte(m_AF) << 1);

    // Set bit 0 of A to the old CarryFlag
    SetHighByte(&m_AF, carry ? SETBIT(GetHighByte(m_AF), 0) : CLEARBIT(GetHighByte(m_AF), 0));

    // Affects Z, clears N, clears H, affects C
    (GetHighByte(m_AF) == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 4;
}

/*
    JR Z, e
    0x18

    Jump relative, to the offset e.

    12 cycles.

    Flags affected(znhc): ----
*/
void CPU::JRe(const byte& opCode)
{
    sbyte e = static_cast<sbyte>(ReadBytePC());

    m_PC += e;
    m_cycles += 12;
}

// 0x1A (LD A, (DE))
void CPU::LDA_DE_(const byte& opCode)
{
    // loads the value stored at the address pointed to by DE
    // (currently 0x0104) and stores in the A register
    byte val = m_MMU->ReadByte(m_DE);
    SetHighByte(&m_AF, val);
    m_cycles += 8;

    // No flags affected
}

/*
    LD A, (BC) - 0x0A

    Loads the value stored at the address pointed to by BC into the accumulator.

    8 Cycles

    Flags affected(znhc): ----
*/
void CPU::LDA_BC_(const byte& opCode)
{
    byte val = m_MMU->ReadByte(m_BC);
    SetHighByte(&m_AF, val);

    m_cycles += 8;
}

// 0x17 (RR A)
void CPU::RRA(const byte& opCode)
{
    // Grab the current CarryFlag val
    bool carry = IsFlagSet(CarryFlag);

    // Grab bit 0 and store it in the carryflag
    ISBITSET(GetHighByte(m_AF), 0) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift A right
    SetHighByte(&m_AF, GetHighByte(m_AF) >> 1);

    // Set bit 7 of A to the old CarryFlag
    SetHighByte(&m_AF, carry ? SETBIT(GetHighByte(m_AF), 7) : CLEARBIT(GetHighByte(m_AF), 7));

    // Affects Z, clears N, clears H, affects C
    (GetHighByte(m_AF) == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 4;
}

/*
    LDI (HL), A
    0x22

    Loads A into the address pointed at by HL, then increment HL.

    8 Cycles

    Flags affected(znhc): ----
*/
void CPU::LDI_HL_A(const byte& opCode)
{
    m_MMU->WriteByte(m_HL, GetHighByte(m_AF)); // Load A into the address pointed at by HL.

    m_HL++;

    m_cycles += 8;
}

/*
DAA
0x27

This instruction conditionally adjust the accumulator for BCD addition and
subtraction operations. For addition (ADD, ADC, INC) or subtraction (SUB, SBC, DEC, NEG),
the following table indicates the operation performed:

OP      C Before    U       H Before    L       Add     C After
        0           0-9     0           0-9     00      0
ADD     0           0-8     0           A-F     06      0
ADC     0           A-F     0           0-9     60      1
INC     0           9-F     0           A-F     66      1
        0           0-9     1           0-3     06      0*
        0           A-F     1           0-3     66      1*
        1           0-2     0           0-9     60      1*
        1           0-2     0           A-F     66      1*
        1           0-3     1           0-3     66      1*
----------------------
SUB     0           0-9     0           0-9     00      0*
SBC     0           0-8     1           6-F     FA      0*
DEC     1           7-F     0           0-9     A0      1*
NEG     1           6-7     1           6-F     9A      1*

4 Cycles

Flags affected(znhc): z-0x
Affected Z, Cleared H, Affected C
*/
void CPU::DAA(const byte& opCode)
{
    int aVal = GetHighByte(m_AF);

    if (!IsFlagSet(SubtractFlag))
    {
        if (IsFlagSet(HalfCarryFlag) || (aVal & 0xF) > 9)
        {
            aVal += 0x06;
        }

        if (IsFlagSet(CarryFlag) || (aVal > 0x9F))
        {
            aVal += 0x60;
        }
    }
    else
    {
        if (IsFlagSet(HalfCarryFlag))
        {
            aVal = (aVal - 0x06) & 0xFF;
        }

        if (IsFlagSet(CarryFlag))
        {
            aVal -= 0x60;
        }
    }

    ClearFlag(HalfCarryFlag);

    if ((aVal & 0x100) == 0x100)
    {
        SetFlag(CarryFlag);
    }

    aVal &= 0xFF;

    (aVal == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    SetHighByte(&m_AF, (byte)aVal);

    m_cycles += 4;
}

/*
    LDI A, (HL)
    0x2A

    Loads the address pointed at by HL into A, then increment HL.

    8 Cycles

    Flags affected(znhc): ----
*/
void CPU::LDIA_HL_(const byte& opCode)
{
    SetHighByte(&m_AF, m_MMU->ReadByte(m_HL));
    m_HL++;

    m_cycles += 8;
}

/*
    CPL - 0x2F

    The contents of the accumulator are inverted.

    4 Cycles

    Flags affected(znhc): -11-
*/
void CPU::CPL(const byte& opCode)
{
    byte A = GetHighByte(m_AF);
    byte result = A ^ 0xFF;
    SetHighByte(&m_AF, result);

    SetFlag(SubtractFlag);
    SetFlag(HalfCarryFlag);

    m_cycles += 4;
}

// 0x32 (LDD (HL), A)
void CPU::LDD_HL_A(const byte& opCode)
{
    m_MMU->WriteByte(m_HL, GetHighByte(m_AF)); // Load A into the address pointed at by HL.

    m_HL--;
    m_cycles += 8;

    // No flags affected
}

/*
    LDD A, (HL)
    0x3A

    Load the byte at the address specified in the HL register into A, and decrement HL.

    8 Cycles

    Flags affected(znhc): ----
*/
void CPU::LDDA_HL_(const byte& opCode)
{
    byte HL = m_MMU->ReadByte(m_HL);
    SetHighByte(&m_AF, HL);

    m_HL--;
    m_cycles += 8;
}

// 0x76 (HALT)
void CPU::HALT(const byte& opCode)
{
    m_isHalted = true;
    m_IFWhenHalted = m_MMU->ReadByte(0xFF0F);
    m_cycles += 4;
}

/*
    ADD A, (HL)
    0x86

    The byte at the memory address specified by the contents of the HL register pair
    is added to the contents of the accumulator, and the result is stored in the
    accumulator.

    8 Cycles

    Flags affected(znhc): z0hc
*/
void CPU::ADDA_HL_(const byte& opCode)
{
    byte A = GetHighByte(m_AF);
    byte HL = m_MMU->ReadByte(m_HL);
    byte result = A + HL;
    SetHighByte(&m_AF, result);

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ((ISBITSET(A, 3))) && (!ISBITSET(result, 3)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    ((ISBITSET(A, 7))) && (!ISBITSET(result, 7)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_cycles += 8;
}

/*
    SUB (HL)
    0x96

    The byte at the memory address specified by the contents of the HL register pair
    is subtracted to the contents of the accumulator, and the result is stored in the
    accumulator.

    8 Cycles

    Flags affected(znhc): z1hc
*/
void CPU::SUB_HL_(const byte& opCode)
{
    byte A = GetHighByte(m_AF);
    byte HL = m_MMU->ReadByte(m_HL);
    byte result = A - HL;
    SetHighByte(&m_AF, result);

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    SetFlag(SubtractFlag);
    ((A & 0x0F) < (HL & 0x0F)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    ((A & 0xFF) < (HL & 0xFF)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_cycles += 8;
}

/*
    AND (HL)
    0xA6

    The logical AND operation is performed between the byte contained at
    the memory address specified by the HL register  and the byte contained
    in the accumulator. The result is stored in the accumulator.

    8 Cycles

    Flags affected(znhc): z010
*/
void CPU::AND_HL_(const byte& opCode)
{
    byte HL = m_MMU->ReadByte(m_HL);
    byte result = HL & GetHighByte(m_AF);
    SetHighByte(&m_AF, result);

    if (result == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(SubtractFlag);
    SetFlag(HalfCarryFlag);
    ClearFlag(CarryFlag);

    m_cycles += 8;
}

/*
    CP (HL)
    0xBE

    The contents of 8-bit register HL are compared with the contents of the accumulator.
    If there is a true compare, the Z flag is set. The execution of this instruction
    does not affect the contents of the accumulator.

    8 Cycles

    Flags affected(znhc): z1hc
*/
void CPU::CP_HL_(const byte& opCode)
{
    byte HL = m_MMU->ReadByte(m_HL);
    byte A = GetHighByte(m_AF);
    byte result = A - HL;

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    SetFlag(SubtractFlag);
    ((A & 0x0F) < (HL & 0x0F)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    ((A & 0xFF) < (HL & 0xFF)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_cycles += 8;
}

/*
    JP nn
    0xC3

    Jump to nn

    16 Cycles

    Flags affected(znhc): ----
*/
void CPU::JPnn(const byte& opCode)
{
    ushort nn = ReadUShortPC();
    m_PC = nn;

    m_cycles += 16;
}

/*
ADD A, n
0xC6

The integer n is added to the contents of the accumulator, and the reults are
stored in the accumulator.

8 Cycles

Flags affected(znhc): z0hc
Affects Z, clears n, affects h, affects c
*/
void CPU::ADDAn(const byte& opCode)
{
    ushort n = ReadBytePC();

    byte A = GetHighByte(m_AF);
    byte result = A + n;

    SetHighByte(&m_AF, result);

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ((ISBITSET(A, 3))) && (!ISBITSET(result, 3)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    ((ISBITSET(A, 7))) && (!ISBITSET(result, 7)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_cycles += 8;
}

/*
    RET
    0xC9

    return, PC=(SP), SP=SP+2

    16 Cycles

    Flags affected(znhc): ----
*/
void CPU::RET(const byte& opCode)
{
    m_PC = PopUShort();

    m_cycles += 16;
}

// 0xCD (CALL nn)
void CPU::CALLnn(const byte& opCode)
{
    // This instruction pushes the PC to the SP, then sets the PC to the target address(nn).
    ushort nn = ReadUShortPC(); // Read nn
    PushUShortToSP(m_PC); // Push PC to SP
    m_PC = nn; // Set the PC to the target address

    m_cycles += 24;

    // No flags affected
}

/*
    ADC A, n
    0xCE

    The 8-bit n operand, along with the carry flag, is added to the contents of the
    accumulator, and the result is stored in the accumulator.

    8 Cycles

    Flags affected(znhc): z0hc
*/
void CPU::ADCAn(const byte& opCode)
{
    byte n = ReadBytePC();
    byte A = GetHighByte(m_AF);
    byte C = (IsFlagSet(CarryFlag)) ? 0x01 : 0x00;
    byte result = A + n + C;
    SetHighByte(&m_AF, result);

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    (ISBITSET(A, 3) && !ISBITSET(result, 3)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    (ISBITSET(A, 7) && !ISBITSET(result, 7)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_cycles += 8;
}

/*
    SUB n (0xD6)

    The 8-bit value n is subtracted from the contents of the Accumulator, and the
    result is stored in the accumulator.

    8 Cycles

    Flags affected(znhc): z1hc
*/
void CPU::SUBn(const byte& opCode)
{
    byte n = ReadBytePC();
    byte A = GetHighByte(m_AF);
    byte result = A - n;
    SetHighByte(&m_AF, result);

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    SetFlag(SubtractFlag);
    ((A & 0x0F) < (n & 0x0F)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    ((A & 0xFF) < (n & 0xFF)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_cycles += 4;
}

/*
    RETI
    0xD9

    Return and enable interrupts.

    16 Cycles

    Flags affected(znhc): ----
*/
void CPU::RETI(const byte& opCode)
{
    m_IME = 0x01; // Restore interrupts
    m_PC = PopUShort(); // Return

    m_cycles += 16;
}

/*
    SBC A, n (0xDE)

    The 8-bit value n, along with the carry flag, is subtracted from the contents
    of the Accumulator, and the result is stored in the accumulator.

    8 Cycles

    Flags affected(znhc): z1hc
*/
void CPU::SBCAn(const byte& opCode)
{
    byte n = ReadBytePC();
    byte A = GetHighByte(m_AF);
    byte C = (IsFlagSet(CarryFlag)) ? 0x01 : 0x00;
    byte result = A - n - C;
    SetHighByte(&m_AF, result);

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    SetFlag(SubtractFlag);
    ((A & 0x0F) < (result & 0x0F)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    ((A & 0xFF) < (result & 0xFF)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_cycles += 8;
}

// 0xE0 (LD(0xFF00 + n), A)
void CPU::LD_0xFF00n_A(const byte& opCode)
{
    byte n = ReadBytePC(); // Read n

    m_MMU->WriteByte(0xFF00 + n, GetHighByte(m_AF)); // Load A into 0xFF00 + n

    m_cycles += 8;

    // No flags affected
}

// 0xE2 (LD(0xFF00 + C), A)
void CPU::LD_0xFF00C_A(const byte& opCode)
{
    m_MMU->WriteByte(0xFF00 + GetLowByte(m_BC), GetHighByte(m_AF)); // Load A into 0xFF00 + C

    m_cycles += 8;

    // No flags affected
}

/*
    LD (nn), A
    0xEA

    The contents of the accumulator are loaded into the address specified by the
    operand nn.

    16 Cycles

    Flags affected(znhc): ----
*/
void CPU::LD_nn_A(const byte& opCode)
{
    ushort nn = ReadUShortPC();

    m_MMU->WriteByte(nn, GetHighByte(m_AF)); // Load A into (nn)

    m_cycles += 16;
}

/*
    XOR n
    0xEE

    The logical exclusive-OR operation is performed between the 8-bit operand and
    the byte contained in the accumulator. The result is stored in the accumulator.

    8 Cycles

    Flags affected(znhc): z000
    Affects Z, clears n, clears h, clears c
*/
void CPU::XORn(const byte& opCode)
{
    byte n = ReadBytePC();
    SetHighByte(&m_AF, n ^ GetHighByte(m_AF));

    // Affects Z and clears NHC
    if (GetHighByte(m_AF) == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);
    ClearFlag(CarryFlag);

    m_cycles += 8;
}

/*
    LD A, (0xFF00 + n)
    0xF0

    Read from io-port n

    12 Cycles

    Flags affected(znhc): ----
*/
void CPU::LDA_0xFF00n_(const byte& opCode)
{
    byte n = ReadBytePC(); // Read n
    SetHighByte(&m_AF, m_MMU->ReadByte(0xFF00 + n));

    m_cycles += 12;
}

/*
    LD A, (0xFF00 + C)
    0xF2

    Read from io-port C

    8 Cycles

    Flags affected(znhc): ----
*/
void CPU::LDA_0xFF00C_(const byte& opCode)
{
    SetHighByte(&m_AF, m_MMU->ReadByte(0xFF00 + GetLowByte(m_BC)));

    m_cycles += 8;
}

/*
    DI
    0xF3

    Disable interrupts

    4 Cycles

    Flags affected(znhc): ----
*/
void CPU::DI(const byte& opCode)
{
    m_IME = 0x00;

    m_cycles += 4;
}

/*
OR n - 0xF6

The logical OR operation is performed between the byte in n and the byte contained in the
accumulator. The result is stored in the accumulator.

8 Cycles

Flags affected(znhc): z000
Affects Z, clears n, clears h, clears c
*/
void CPU::ORn(const byte& opCode)
{
    byte n = ReadBytePC();
    SetHighByte(&m_AF, n | GetHighByte(m_AF));

    // Affects Z and clears NHC
    if (GetHighByte(m_AF) == 0x00)
    {
        SetFlag(ZeroFlag);
    }
    else
    {
        ClearFlag(ZeroFlag);
    }

    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);
    ClearFlag(CarryFlag);

    m_cycles += 8;
}

/*
    LD SP, HL
    0xF9

    Load HL into SP.

    8 Cycles

    Flags affected(znhc): ----
*/
void CPU::LDSPHL(const byte& opCode)
{
    m_SP = m_HL;

    m_cycles += 8;
}

/*
LD HL, SP+e - 0xF8

ld   HL,SP+dd  F8          12 00hc HL = SP +/- dd ;dd is 8bit signed number

12 Cycles

Flags affected(znhc): 00hc
Clears Z, clears n, affects h, affects c
*/
void CPU::LDHLSPe(const byte& opCode)
{
    sbyte e = static_cast<sbyte>(ReadBytePC());

    ushort result = m_SP + e;

    ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);

    ((ISBITSET(m_HL, 11))) && (!ISBITSET(result, 11)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    ((ISBITSET(m_HL, 15))) && (!ISBITSET(result, 15)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    m_HL = result;

    m_cycles += 12;
}

/*
LD A, (nn)
0xFA

The contents of the address specified by the operand nn are loaded into the accumulator.

16 Cycles

Flags affected(znhc): ----
*/
void CPU::LDA_nn_(const byte& opCode)
{
    ushort nn = ReadUShortPC();
    SetHighByte(&m_AF, m_MMU->ReadByte(nn));

    m_cycles += 16;
}

/*
    EI
    0xFB

    Enable interrupts

    4 Cycles

    Flags affected(znhc): ----
*/
void CPU::EI(const byte& opCode)
{
    m_IME = 0x01;

    m_cycles += 4;
}

/*
    CP n
    0xFE

    The contents of 8-bit operand n are compared with the contents of the accumulator.
    If there is a true compare, the Z flag is set. The execution of this instruction
    does not affect the contents of the accumulator.

    8 Cycles

    Flags affected(znhc): z1hc
*/
void CPU::CPn(const byte& opCode)
{
    byte n = ReadBytePC();
    byte A = GetHighByte(m_AF);
    byte result = A - n;

    (result == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ((A & 0xFF) < (n & 0xFF)) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);
    ((A & 0x0F) < (n & 0x0F)) ? SetFlag(HalfCarryFlag) : ClearFlag(HalfCarryFlag);
    SetFlag(SubtractFlag);

    m_cycles += 8;
}

/*
    CPU 0xCB INSTRUCTION MAP
*/

/*
    RLC r
    11001011(CB) 00000rrr

    The contents of 8-bit register r are rotated left 1-bit position. The content of bit 7
    is copied to the carry flag and also to bit 0. Operand r identifies register
    B, C, D, E, H, L, or A.

    8 Cycles

    Flags affected(znhc): z00c
*/
void CPU::RLCr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);

    // Grab bit 7 and store it in the carryflag
    ISBITSET(*r, 7) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r left
    (*r) = *r << 1;

    // Set bit 0 of r to the old CarryFlag
    (*r) = IsFlagSet(CarryFlag) ? SETBIT((*r), 0) : CLEARBIT((*r), 0);

    // Affects Z, clears N, clears H, affects C
    SetFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 8;
}

void CPU::RLC_HL_(const byte& opCode)
{
    byte r = m_MMU->ReadByte(m_HL);

    // Grab bit 7 and store it in the carryflag
    ISBITSET(r, 7) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r left
    r <<= 1;

    // Set bit 0 of r to the old CarryFlag
    r = IsFlagSet(CarryFlag) ? SETBIT((r), 0) : CLEARBIT((r), 0);

    m_MMU->WriteByte(m_HL, r);

    // Affects Z, clears N, clears H, affects C
    SetFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 16;
}

/*
    RRC r
    11001011(CB) 00001rrr

    The contents of 8-bit register r are rotated right 1-bit position. The content of bit 0
    is copied to the carry flag and also to bit 7. Operand r identifies register
    B, C, D, E, H, L, or A.

    8 Cycles

    Flags affected(znhc): z00c
*/
void CPU::RRCr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);

    // Grab bit 0 and store it in the carryflag
    ISBITSET(*r, 0) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r right
    (*r) = *r >> 1;

    // Set bit 0 of r to the old CarryFlag
    (*r) = IsFlagSet(CarryFlag) ? SETBIT((*r), 7) : CLEARBIT((*r), 7);

    // Affects Z, clears N, clears H, affects C
    (*r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 8;
}

void CPU::RRC_HL_(const byte& opCode)
{
    byte r = m_MMU->ReadByte(m_HL);

    // Grab bit 0 and store it in the carryflag
    ISBITSET(r, 0) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r right
    r >>= 1;

    // Set bit 0 of r to the old CarryFlag
    r = IsFlagSet(CarryFlag) ? SETBIT((r), 7) : CLEARBIT((r), 7);

    m_MMU->WriteByte(m_HL, r);

    // Affects Z, clears N, clears H, affects C
    (r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 16;
}

/*
    RL r
    11001011(CB) 00010rrr

    The contents of 8-bit register r are rotated left 1-bit position. The content of bit 7
    is copied to the carry flag and the previous content of the carry flag is copied to bit 0.
    Operand r identifies register: B, C, D, E, H, L, or A.

    8 Cycles

    Flags affected(znhc): z00c
*/
void CPU::RLr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);

    // Grab the current CarryFlag val
    bool carry = IsFlagSet(CarryFlag);

    // Grab bit 7 and store it in the carryflag
    ISBITSET(*r, 7) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r left
    (*r) = *r << 1;

    // Set bit 0 of r to the old CarryFlag
    (*r) = carry ? SETBIT((*r), 0) : CLEARBIT((*r), 0);

    // Affects Z, clears N, clears H, affects C
    (*r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 8;
}

void CPU::RL_HL_(const byte& opCode)
{
    byte r = m_MMU->ReadByte(m_HL);

    // Grab the current CarryFlag val
    bool carry = IsFlagSet(CarryFlag);

    // Grab bit 7 and store it in the carryflag
    ISBITSET(r, 7) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r left
    r <<= 1;

    // Set bit 0 of r to the old CarryFlag
    r = carry ? SETBIT((r), 0) : CLEARBIT((r), 0);

    m_MMU->WriteByte(m_HL, r);

    // Affects Z, clears N, clears H, affects C
    (r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 16;
}

/*
    RR r
    11001011(CB) 00001rrr

    The contents of 8-bit register r are rotated right 1-bit position. The content of bit 0
    is copied to the carry flag and the previous content of the carry flag is copied to bit 7.
    Operand r identifies register: B, C, D, E, H, L, or A.

    8 Cycles

    Flags affected(znhc): z00c
*/
void CPU::RRr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);

    // Grab the current CarryFlag val
    bool carry = IsFlagSet(CarryFlag);

    // Grab bit 0 and store it in the carryflag
    ISBITSET(*r, 0) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r right
    (*r) = *r >> 1;

    // Set bit 7 of r to the old CarryFlag
    (*r) = carry ? SETBIT((*r), 7) : CLEARBIT((*r), 7);

    // Affects Z, clears N, clears H, affects C
    (*r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 8;
}

void CPU::RR_HL_(const byte& opCode)
{
    byte r = m_MMU->ReadByte(m_HL);

    // Grab the current CarryFlag val
    bool carry = IsFlagSet(CarryFlag);

    // Grab bit 0 and store it in the carryflag
    ISBITSET(r, 0) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r right
    r >>= 1;

    // Set bit 7 of r to the old CarryFlag
    r = carry ? SETBIT((r), 7) : CLEARBIT((r), 7);

    m_MMU->WriteByte(m_HL, r);

    // Affects Z, clears N, clears H, affects C
    (r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 16;
}

/*
    SLA r
    11001011 00100rrrr

    An arithmetic shift left 1-bit position is performed on the contents of the operand r. The content
    of bit 7 is copied to the carry flag.

    8 Cycles

    Flags affected(znhc): z00c
    Affects Z, clears n, clears h, affects c
*/
void CPU::SLAr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);

    // Grab bit 7 and store it in the carryflag
    ISBITSET(*r, 7) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r left
    (*r) = *r << 1;

    // Affects Z, clears N, clears H, affects C
    (*r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 8;
}

void CPU::SLA_HL_(const byte& opCode)
{
    byte r = m_MMU->ReadByte(m_HL);

    // Grab bit 7 and store it in the carryflag
    ISBITSET(r, 7) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r left
    r = r << 1;
    m_MMU->WriteByte(m_HL, r);

    // Affects Z, clears N, clears H, affects C
    (r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 16;
}

/*
    SRA r
    11001011 00101rrr

    An arithmetic shift right 1-bit position is performed on the contents of the operand r. The content
    of bit 0 is copied to the carry flag.

    8 Cycles

    Flags affected(znhc): z00c
    Affects Z, clears n, clears h, affects c
*/
void CPU::SRAr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);

    // Grab bit 0 and store it in the carryflag
    ISBITSET(*r, 0) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r right
    (*r) = *r >> 1;

    // Affects Z, clears N, clears H, affects C
    (*r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 8;
}

void CPU::SRA_HL_(const byte& opCode)
{
    byte r = m_MMU->ReadByte(m_HL);

    // Grab bit 0 and store it in the carryflag
    ISBITSET(r, 0) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r right
    r = r >> 1;
    m_MMU->WriteByte(m_HL, r);

    // Affects Z, clears N, clears H, affects C
    (r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 16;
}

/*
SRL r
11001011 00011rrr

The contents of the operand "r" are shifted right 1 -bit.  The content of
bit 0 is copied to the carry flag and bit 7 is reset.

8 Cycles

Flags affected(znhc): z00c
Affects Z, clears n, clears h, affects c
*/
void CPU::SRLr(const byte& opCode)
{
    byte* r = GetByteRegister(opCode);

    // Grab bit 0 and store it in the carryflag
    ISBITSET(*r, 0) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r right
    (*r) = *r >> 1;
    (*r) = CLEARBIT(*r, 7);

    // Affects Z, clears N, clears H, affects C
    (*r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 8;
}

void CPU::SRL_HL_(const byte& opCode)
{
    byte r = m_MMU->ReadByte(m_HL);

    // Grab bit 0 and store it in the carryflag
    ISBITSET(r, 0) ? SetFlag(CarryFlag) : ClearFlag(CarryFlag);

    // Shift r right
    r = r >> 1;
    r = CLEARBIT(r, 7);
    m_MMU->WriteByte(m_HL, r);

    // Affects Z, clears N, clears H, affects C
    (r == 0x00) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);
    ClearFlag(SubtractFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 16;
}

/*
    BIT b, r
    11001011 01bbbrrr

    This instruction tests bit b in register r and sets the Z flag accordingly.

    8 Cycles

    Flags affected(znhc): z01-
    Affects Z, clears n, sets h
*/
void CPU::BITbr(const byte& opCode)
{
    m_cycles += 8;

    byte bit = (opCode >> 3) & 0x07;
    byte* r = GetByteRegister(opCode);

    // Test bit b in r
    (!ISBITSET(*r, bit)) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);

    SetFlag(HalfCarryFlag); // H is set
    ClearFlag(SubtractFlag); // N is reset
}

void CPU::BITb_HL_(const byte& opCode)
{
    m_cycles += 12;

    byte bit = (opCode >> 3) & 0x07;
    byte r = m_MMU->ReadByte(m_HL);

    // Test bit b in r
    (!ISBITSET(r, bit)) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);

    SetFlag(HalfCarryFlag); // H is set
    ClearFlag(SubtractFlag); // N is reset
}

/*
    RES b, r
    11001011 10bbbrrr

    Bit b in operand r is reset.

    8 Cycles

    No flags affected.
*/
void CPU::RESbr(const byte& opCode)
{
    m_cycles += 8;

    byte bit = (opCode >> 3) & 0x07;
    byte* r = GetByteRegister(opCode);
    *r = CLEARBIT(*r, bit);
}

void CPU::RESb_HL_(const byte& opCode)
{
    m_cycles += 16;

    byte bit = (opCode >> 3) & 0x07;
    byte r = m_MMU->ReadByte(m_HL);
    m_MMU->WriteByte(m_HL, CLEARBIT(r, bit));
}

/*
    SET b, r
    11001011 11bbbrrr

    Bit b in operand r is set.

    8 Cycles

    No flags affected.
*/
void CPU::SETbr(const byte& opCode)
{
    m_cycles += 8;

    byte bit = (opCode >> 3) & 0x07;
    byte* r = GetByteRegister(opCode);
    *r = SETBIT(*r, bit);
}

void CPU::SETb_HL_(const byte& opCode)
{
    m_cycles += 16;

    byte bit = (opCode >> 3) & 0x07;
    byte r = m_MMU->ReadByte(m_HL);
    m_MMU->WriteByte(m_HL, SETBIT(r, bit));
}

/*
    SWAP r
    11001011 00111rrr
    swap r         CB 3x        8 z000 exchange low/hi-nibble
    swap (HL)      CB 36       16 z000 exchange low/hi-nibble

    Exchange the low and hi nibble (a nibble is 4 bits)

    No flags affected.
*/
void CPU::SWAPr(const byte& opCode)
{
    m_cycles += 8;

    byte* r = GetByteRegister(opCode);
    byte lowNibble = (*r & 0x0F);
    byte highNibble = (*r & 0xF0);

    *r = (lowNibble << 4) | (highNibble >> 4);
}

void CPU::SWAP_HL_(const byte& opCode)
{
    m_cycles += 16;

    byte r = m_MMU->ReadByte(m_HL);
    byte lowNibble = (r & 0x0F);
    byte highNibble = (r & 0xF0);

    m_MMU->WriteByte(m_HL, (lowNibble << 4) | (highNibble >> 4));
}
