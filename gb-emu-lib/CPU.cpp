#include "pch.hpp"
#include "CPU.hpp"

// The number of CPU cycles per frame
const unsigned int CyclesPerFrame = 70244;

CPU::CPU() :
    m_cycles(0),
    m_isHalted(false),
    m_AF(0x0000),
    m_BC(0x0000),
    m_DE(0x0000),
    m_HL(0x0000),
    m_SP(0x0000),
    m_PC(0x0000)
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
    //m_operationMap[0x02] TODO
    m_operationMap[0x03] = &CPU::INCrr;
    m_operationMap[0x04] = &CPU::INCr;
    m_operationMap[0x05] = &CPU::DECr;
    m_operationMap[0x06] = &CPU::LDrn;
    //m_operationMap[0x07] TODO
    //m_operationMap[0x08] TODO
    //m_operationMap[0x09] TODO
    //m_operationMap[0x0A] TODO
    //m_operationMap[0x0B] TODO
    m_operationMap[0x0C] = &CPU::INCr;
    m_operationMap[0x0D] = &CPU::DECr;
    m_operationMap[0x0E] = &CPU::LDrn;
    //m_operationMap[0x0F] TODO

    // 10
    //m_operationMap[0x10] TODO
    m_operationMap[0x11] = &CPU::LDrrnn;
    //m_operationMap[0x12] TODO
    m_operationMap[0x13] = &CPU::INCrr;
    m_operationMap[0x14] = &CPU::INCr;
    m_operationMap[0x15] = &CPU::DECr;
    m_operationMap[0x16] = &CPU::LDrn;
    m_operationMap[0x17] = &CPU::RLA;
    //m_operationMap[0x18] TODO
    //m_operationMap[0x19] TODO
    m_operationMap[0x1A] = &CPU::LDA_DE_;
    //m_operationMap[0x1B] TODO
    m_operationMap[0x1C] = &CPU::INCr;
    m_operationMap[0x1D] = &CPU::DECr;
    m_operationMap[0x1E] = &CPU::LDrn;
    //m_operationMap[0x1F] TODO

    // 20
    m_operationMap[0x20] = &CPU::JRNZe;
    m_operationMap[0x21] = &CPU::LDrrnn;
    m_operationMap[0x22] = &CPU::LDI_HL_A;
    m_operationMap[0x23] = &CPU::INCrr;
    m_operationMap[0x24] = &CPU::INCr;
    m_operationMap[0x25] = &CPU::DECr;
    m_operationMap[0x26] = &CPU::LDrn;
    //m_operationMap[0x27] TODO
    //m_operationMap[0x28] TODO
    //m_operationMap[0x29] TODO
    //m_operationMap[0x2A] TODO
    //m_operationMap[0x2B] TODO
    m_operationMap[0x2C] = &CPU::INCr;
    m_operationMap[0x2D] = &CPU::DECr;
    m_operationMap[0x2E] = &CPU::LDrn;
    //m_operationMap[0x2F] TODO

    // 30
    //m_operationMap[0x30] TODO
    m_operationMap[0x31] = &CPU::LDrrnn;
    m_operationMap[0x32] = &CPU::LDD_HL_A;
    m_operationMap[0x33] = &CPU::INCrr;
    //m_operationMap[0x34] TODO
    //m_operationMap[0x35] TODO
    //m_operationMap[0x36] TODO
    //m_operationMap[0x37] TODO
    //m_operationMap[0x38] TODO
    //m_operationMap[0x39] TODO
    //m_operationMap[0x3A] TODO
    //m_operationMap[0x3B] TODO
    m_operationMap[0x3C] = &CPU::INCr;
    m_operationMap[0x3D] = &CPU::DECr;
    m_operationMap[0x3E] = &CPU::LDrn;
    //m_operationMap[0x3F] TODO

    // 40
    m_operationMap[0x40] = &CPU::LDrR;
    m_operationMap[0x41] = &CPU::LDrR;
    m_operationMap[0x42] = &CPU::LDrR;
    m_operationMap[0x43] = &CPU::LDrR;
    m_operationMap[0x44] = &CPU::LDrR;
    m_operationMap[0x45] = &CPU::LDrR;
    //m_operationMap[0x46] TODO
    m_operationMap[0x47] = &CPU::LDrR;
    m_operationMap[0x48] = &CPU::LDrR;
    m_operationMap[0x49] = &CPU::LDrR;
    m_operationMap[0x4A] = &CPU::LDrR;
    m_operationMap[0x4B] = &CPU::LDrR;
    m_operationMap[0x4C] = &CPU::LDrR;
    m_operationMap[0x4D] = &CPU::LDrR;
    //m_operationMap[0x4E] TODO
    m_operationMap[0x4F] = &CPU::LDrR;

    // 50
    m_operationMap[0x50] = &CPU::LDrR;
    m_operationMap[0x51] = &CPU::LDrR;
    m_operationMap[0x52] = &CPU::LDrR;
    m_operationMap[0x53] = &CPU::LDrR;
    m_operationMap[0x54] = &CPU::LDrR;
    m_operationMap[0x55] = &CPU::LDrR;
    //m_operationMap[0x56] TODO
    m_operationMap[0x57] = &CPU::LDrR;
    m_operationMap[0x58] = &CPU::LDrR;
    m_operationMap[0x59] = &CPU::LDrR;
    m_operationMap[0x5A] = &CPU::LDrR;
    m_operationMap[0x5B] = &CPU::LDrR;
    m_operationMap[0x5C] = &CPU::LDrR;
    m_operationMap[0x5D] = &CPU::LDrR;
    //m_operationMap[0x5E] TODO
    m_operationMap[0x5F] = &CPU::LDrR;

    // 60
    m_operationMap[0x60] = &CPU::LDrR;
    m_operationMap[0x61] = &CPU::LDrR;
    m_operationMap[0x62] = &CPU::LDrR;
    m_operationMap[0x63] = &CPU::LDrR;
    m_operationMap[0x64] = &CPU::LDrR;
    m_operationMap[0x65] = &CPU::LDrR;
    //m_operationMap[0x66] TODO
    m_operationMap[0x67] = &CPU::LDrR;
    m_operationMap[0x68] = &CPU::LDrR;
    m_operationMap[0x69] = &CPU::LDrR;
    m_operationMap[0x6A] = &CPU::LDrR;
    m_operationMap[0x6B] = &CPU::LDrR;
    m_operationMap[0x6C] = &CPU::LDrR;
    m_operationMap[0x6D] = &CPU::LDrR;
    //m_operationMap[0x6E] TODO
    m_operationMap[0x6F] = &CPU::LDrR;

    // 70
    //m_operationMap[0x70] TODO
    //m_operationMap[0x71] TODO
    //m_operationMap[0x72] TODO
    //m_operationMap[0x73] TODO
    //m_operationMap[0x74] TODO
    //m_operationMap[0x75] TODO
    //m_operationMap[0x76] TODO
    m_operationMap[0x77] = &CPU::LD_HL_A;
    m_operationMap[0x78] = &CPU::LDrR;
    m_operationMap[0x79] = &CPU::LDrR;
    m_operationMap[0x7A] = &CPU::LDrR;
    m_operationMap[0x7B] = &CPU::LDrR;
    m_operationMap[0x7C] = &CPU::LDrR;
    m_operationMap[0x7D] = &CPU::LDrR;
    //m_operationMap[0x7E] TODO
    m_operationMap[0x7F] = &CPU::LDrR;

    // 80
    //m_operationMap[0x80] TODO
    //m_operationMap[0x81] TODO
    //m_operationMap[0x82] TODO
    //m_operationMap[0x83] TODO
    //m_operationMap[0x84] TODO
    //m_operationMap[0x85] TODO
    //m_operationMap[0x86] TODO
    //m_operationMap[0x87] TODO
    //m_operationMap[0x88] TODO
    //m_operationMap[0x89] TODO
    //m_operationMap[0x8A] TODO
    //m_operationMap[0x8B] TODO
    //m_operationMap[0x8C] TODO
    //m_operationMap[0x8D] TODO
    //m_operationMap[0x8E] TODO
    //m_operationMap[0x8F] TODO

    // 90
    //m_operationMap[0x90] TODO
    //m_operationMap[0x91] TODO
    //m_operationMap[0x92] TODO
    //m_operationMap[0x93] TODO
    //m_operationMap[0x94] TODO
    //m_operationMap[0x95] TODO
    //m_operationMap[0x96] TODO
    //m_operationMap[0x97] TODO
    //m_operationMap[0x98] TODO
    //m_operationMap[0x99] TODO
    //m_operationMap[0x9A] TODO
    //m_operationMap[0x9B] TODO
    //m_operationMap[0x9C] TODO
    //m_operationMap[0x9D] TODO
    //m_operationMap[0x9E] TODO
    //m_operationMap[0x9F] TODO

    // A0
    //m_operationMap[0xA0] TODO
    //m_operationMap[0xA1] TODO
    //m_operationMap[0xA2] TODO
    //m_operationMap[0xA3] TODO
    //m_operationMap[0xA4] TODO
    //m_operationMap[0xA5] TODO
    //m_operationMap[0xA6] TODO
    //m_operationMap[0xA7] TODO
    m_operationMap[0xA8] = &CPU::XORr;
    m_operationMap[0xA9] = &CPU::XORr;
    m_operationMap[0xAA] = &CPU::XORr;
    m_operationMap[0xAB] = &CPU::XORr;
    m_operationMap[0xAC] = &CPU::XORr;
    m_operationMap[0xAD] = &CPU::XORr;
    //m_operationMap[0xAE] TODO
    m_operationMap[0xAF] = &CPU::XORr;

    // B0
    //m_operationMap[0xB0] TODO
    //m_operationMap[0xB1] TODO
    //m_operationMap[0xB2] TODO
    //m_operationMap[0xB3] TODO
    //m_operationMap[0xB4] TODO
    //m_operationMap[0xB5] TODO
    //m_operationMap[0xB6] TODO
    //m_operationMap[0xB7] TODO
    //m_operationMap[0xB8] TODO
    //m_operationMap[0xB9] TODO
    //m_operationMap[0xBA] TODO
    //m_operationMap[0xBB] TODO
    //m_operationMap[0xBC] TODO
    //m_operationMap[0xBD] TODO
    //m_operationMap[0xBE] TODO
    //m_operationMap[0xBF] TODO

    // C0
    //m_operationMap[0xC0] TODO
    m_operationMap[0xC1] = &CPU::POPrr;
    //m_operationMap[0xC2] TODO
    //m_operationMap[0xC3] TODO
    //m_operationMap[0xC4] TODO
    m_operationMap[0xC5] = &CPU::PUSHrr;
    //m_operationMap[0xC6] TODO
    //m_operationMap[0xC7] TODO
    //m_operationMap[0xC8] TODO
    m_operationMap[0xC9] = &CPU::RET;
    //m_operationMap[0xCA] TODO
    //m_operationMap[0xCB] TODO
    //m_operationMap[0xCC] TODO
    m_operationMap[0xCD] = &CPU::CALLnn;
    //m_operationMap[0xCE] TODO
    //m_operationMap[0xCF] TODO

    // D0
    //m_operationMap[0xD0] TODO
    m_operationMap[0xD1] = &CPU::POPrr;
    //m_operationMap[0xD2] TODO
    //m_operationMap[0xD3] TODO
    //m_operationMap[0xD4] TODO
    m_operationMap[0xD5] = &CPU::PUSHrr;
    //m_operationMap[0xD6] TODO
    //m_operationMap[0xD7] TODO
    //m_operationMap[0xD8] TODO
    //m_operationMap[0xD9] TODO
    //m_operationMap[0xDA] TODO
    //m_operationMap[0xDB] TODO
    //m_operationMap[0xDC] TODO
    //m_operationMap[0xDD] TODO
    //m_operationMap[0xDE] TODO
    //m_operationMap[0xDF] TODO

    // E0
    m_operationMap[0xE0] = &CPU::LD_0xFF00n_A;
    m_operationMap[0xE1] = &CPU::POPrr;
    m_operationMap[0xE2] = &CPU::LD_0xFF00C_A;
    //m_operationMap[0xE3] TODO
    //m_operationMap[0xE4] TODO
    m_operationMap[0xE5] = &CPU::PUSHrr;
    //m_operationMap[0xE6] TODO
    //m_operationMap[0xE7] TODO
    //m_operationMap[0xE8] TODO
    //m_operationMap[0xE9] TODO
    m_operationMap[0xEA]= &CPU::LD_nn_A;
    //m_operationMap[0xEB] TODO
    //m_operationMap[0xEC] TODO
    //m_operationMap[0xED] TODO
    //m_operationMap[0xEE] TODO
    //m_operationMap[0xEF] TODO

    // F0
    //m_operationMap[0xF0] TODO
    m_operationMap[0xF1] = &CPU::POPrr;
    //m_operationMap[0xF2] TODO
    //m_operationMap[0xF3] TODO
    //m_operationMap[0xF4] TODO
    m_operationMap[0xF5] = &CPU::PUSHrr;
    //m_operationMap[0xF6] TODO
    //m_operationMap[0xF7] TODO
    //m_operationMap[0xF8] TODO
    //m_operationMap[0xF9] TODO
    //m_operationMap[0xFA] TODO
    //m_operationMap[0xFB] TODO
    //m_operationMap[0xFC] TODO
    //m_operationMap[0xFD] TODO
    m_operationMap[0xFE] = &CPU::CPn;
    //m_operationMap[0xFF] TODO

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
    //m_operationMapCB[0x20] TODO
    //m_operationMapCB[0x21] TODO
    //m_operationMapCB[0x22] TODO
    //m_operationMapCB[0x23] TODO
    //m_operationMapCB[0x24] TODO
    //m_operationMapCB[0x25] TODO
    //m_operationMapCB[0x26] TODO
    //m_operationMapCB[0x27] TODO
    //m_operationMapCB[0x28] TODO
    //m_operationMapCB[0x29] TODO
    //m_operationMapCB[0x2A] TODO
    //m_operationMapCB[0x2B] TODO
    //m_operationMapCB[0x2C] TODO
    //m_operationMapCB[0x2D] TODO
    //m_operationMapCB[0x2E] TODO
    //m_operationMapCB[0x2F] TODO

    // 30
    //m_operationMapCB[0x30] UNUSED!
    //m_operationMapCB[0x31] UNUSED!
    //m_operationMapCB[0x32] UNUSED!
    //m_operationMapCB[0x33] UNUSED!
    //m_operationMapCB[0x34] UNUSED!
    //m_operationMapCB[0x35] UNUSED!
    //m_operationMapCB[0x36] UNUSED!
    //m_operationMapCB[0x37] UNUSED!
    m_operationMapCB[0x38] = &CPU::SWAPr;
    m_operationMapCB[0x39] = &CPU::SWAPr;
    m_operationMapCB[0x3A] = &CPU::SWAPr;
    m_operationMapCB[0x3B] = &CPU::SWAPr;
    m_operationMapCB[0x3C] = &CPU::SWAPr;
    m_operationMapCB[0x3D] = &CPU::SWAPr;
    m_operationMapCB[0x3E] = &CPU::SWAP_HL_;
    m_operationMapCB[0x3F] = &CPU::SWAPr;

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
        m_MMU->RegisterMemoryUnit(0xFF4E, 0xFF55, m_GPU.get());
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
        Step();
    }

    // Reset the cycles. If we went over our max cycles, the next frame will start a
    // few cycles ahead.
    m_cycles -= CyclesPerFrame;
}

void CPU::TriggerInterrupt(byte interrupt)
{
    // TODO: Process interrupts here!
}

byte* CPU::GetCurrentFrame()
{
    return m_GPU->GetCurrentFrame();
}

void CPU::SetInput(byte input, byte buttons)
{
    m_joypad->SetInput(input, buttons);
}

void CPU::Step()
{
    unsigned long preCycles = m_cycles;

    if (m_isHalted)
    {
        NOP(0x00);
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
        }
        else
        {
            Logger::LogError("OpCode 0x%02X at address 0x%04X could not be interpreted.", opCode, addr);
            HALT();
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

void CPU::HALT()
{
    m_isHalted = true;
    Logger::Log("!!!! HALTED !!!!");
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

    ClearFlag(AddFlag);

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
    XOR r
    10110rrr

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

    ClearFlag(AddFlag);
    ClearFlag(HalfCarryFlag);
    ClearFlag(CarryFlag);

    m_cycles += 4;
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

    ClearFlag(AddFlag);

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
    ClearFlag(AddFlag);
    ClearFlag(HalfCarryFlag);

    m_cycles += 4;
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

// 0x20 0xFB (JR NZ, e)
void CPU::JRNZe(const byte& opCode)
{
    sbyte arg = static_cast<sbyte>(ReadBytePC());

    if (IsFlagSet(ZeroFlag))
    {
        m_cycles += 8;
    }
    else
    {
        m_PC += arg;
        m_cycles += 12;
    }

    // No flags affected
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
    if (!m_MMU->WriteByte(m_HL, GetHighByte(m_AF))) // Load A into the address pointed at by HL.
    {
        HALT();
        return;
    }

    m_HL++;

    m_cycles += 8;
}

// 0x32 (LDD (HL), A)
void CPU::LDD_HL_A(const byte& opCode)
{
    if (!m_MMU->WriteByte(m_HL, GetHighByte(m_AF))) // Load A into the address pointed at by HL.
    {
        HALT();
        return;
    }

    m_HL--;
    m_cycles += 8;

    // No flags affected
}

// 0x77 (LD (HL), A)
// Identical to 0x32, except does not decrement
void CPU::LD_HL_A(const byte& opCode)
{
    if (!m_MMU->WriteByte(m_HL, GetHighByte(m_AF))) // Load A into the address pointed at by HL.
    {
        HALT();
    }

    m_cycles += 8;

    // No flags affected
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

// 0xE0 (LD(0xFF00 + n), A)
void CPU::LD_0xFF00n_A(const byte& opCode)
{
    byte n = ReadBytePC(); // Read n

    if (!m_MMU->WriteByte(0xFF00 + n, GetHighByte(m_AF))) // Load A into 0xFF00 + n
    {
        HALT();
    }

    m_cycles += 8;

    // No flags affected
}

// 0xE2 (LD(0xFF00 + C), A)
void CPU::LD_0xFF00C_A(const byte& opCode)
{
    if (!m_MMU->WriteByte(0xFF00 + GetLowByte(m_BC), GetHighByte(m_AF))) // Load A into 0xFF00 + C
    {
        HALT();
    }

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

    if (!m_MMU->WriteByte(nn, GetHighByte(m_AF))) // Load A into (nn)
    {
        HALT();
    }

    m_cycles += 16;
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
    SetFlag(AddFlag);

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
    ClearFlag(AddFlag);
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
    ClearFlag(AddFlag);
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
    ClearFlag(AddFlag);
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
    ClearFlag(AddFlag);
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
    ClearFlag(AddFlag);
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
    ClearFlag(AddFlag);
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
    ClearFlag(AddFlag);
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
    ClearFlag(AddFlag);
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
    ClearFlag(AddFlag); // N is reset
}

void CPU::BITb_HL_(const byte& opCode)
{
    m_cycles += 12;

    byte bit = (opCode >> 3) & 0x07;
    byte r = m_MMU->ReadByte(m_HL);

    // Test bit b in r
    (!ISBITSET(r, bit)) ? SetFlag(ZeroFlag) : ClearFlag(ZeroFlag);

    SetFlag(HalfCarryFlag); // H is set
    ClearFlag(AddFlag); // N is reset
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
