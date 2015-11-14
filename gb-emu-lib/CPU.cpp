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
    for (unsigned int index = 0;index < ARRAYSIZE(m_operationMap);index++)
    {
        m_operationMap[index] = nullptr;
    }
    for (unsigned int index = 0;index < ARRAYSIZE(m_operationMapCB);index++)
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
    //m_operationMap[0x03] TODO
    m_operationMap[0x04] = &CPU::INCr;
    //m_operationMap[0x05] TODO
    m_operationMap[0x06] = &CPU::LDrn;
    //m_operationMap[0x07] TODO
    //m_operationMap[0x08] TODO
    //m_operationMap[0x09] TODO
    //m_operationMap[0x0A] TODO
    //m_operationMap[0x0B] TODO
    m_operationMap[0x0C] = &CPU::INCr;
    //m_operationMap[0x0D] TODO
    m_operationMap[0x0E] = &CPU::LDrn;
    //m_operationMap[0x0F] TODO

    // 10
    //m_operationMap[0x10] TODO
    m_operationMap[0x11] = &CPU::LDrrnn;
    //m_operationMap[0x12] TODO
    //m_operationMap[0x13] TODO
    m_operationMap[0x14] = &CPU::INCr;
    //m_operationMap[0x15] TODO
    m_operationMap[0x16] = &CPU::LDrn;
    m_operationMap[0x17] = &CPU::RLA;
    //m_operationMap[0x18] TODO
    //m_operationMap[0x19] TODO
    m_operationMap[0x1A] = &CPU::LDA_DE_;
    //m_operationMap[0x1B] TODO
    m_operationMap[0x1C] = &CPU::INCr;
    //m_operationMap[0x1D] TODO
    m_operationMap[0x1E] = &CPU::LDrn;
    //m_operationMap[0x1F] TODO

    // 20
    m_operationMap[0x20] = &CPU::JRNZe;
    m_operationMap[0x21] = &CPU::LDrrnn;
    //m_operationMap[0x22] TODO
    //m_operationMap[0x23] TODO
    m_operationMap[0x24] = &CPU::INCr;
    //m_operationMap[0x25] TODO
    m_operationMap[0x26] = &CPU::LDrn;
    //m_operationMap[0x27] TODO
    //m_operationMap[0x28] TODO
    //m_operationMap[0x29] TODO
    //m_operationMap[0x2A] TODO
    //m_operationMap[0x2B] TODO
    m_operationMap[0x2C] = &CPU::INCr;
    //m_operationMap[0x2D] TODO
    m_operationMap[0x2E] = &CPU::LDrn;
    //m_operationMap[0x2F] TODO

    // 30
    //m_operationMap[0x30] TODO
    m_operationMap[0x31] = &CPU::LDrrnn;
    m_operationMap[0x32] = &CPU::LDD_HL_A;
    //m_operationMap[0x33] TODO
    //m_operationMap[0x34] TODO
    //m_operationMap[0x35] TODO
    //m_operationMap[0x36] TODO
    //m_operationMap[0x37] TODO
    //m_operationMap[0x38] TODO
    //m_operationMap[0x39] TODO
    //m_operationMap[0x3A] TODO
    //m_operationMap[0x3B] TODO
    m_operationMap[0x3C] = &CPU::INCr;
    //m_operationMap[0x3D] TODO
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
    m_operationMap[0xC1] = &CPU::POPBC;
    //m_operationMap[0xC2] TODO
    //m_operationMap[0xC3] TODO
    //m_operationMap[0xC4] TODO
    m_operationMap[0xC5] = &CPU::PUSHBC;
    //m_operationMap[0xC6] TODO
    //m_operationMap[0xC7] TODO
    //m_operationMap[0xC8] TODO
    //m_operationMap[0xC9] TODO
    //m_operationMap[0xCA] TODO
    //m_operationMap[0xCB] TODO
    //m_operationMap[0xCC] TODO
    m_operationMap[0xCD] = &CPU::CALLnn;
    //m_operationMap[0xCE] TODO
    //m_operationMap[0xCF] TODO

    // D0
    //m_operationMap[0xD0] TODO
    //m_operationMap[0xD1] TODO
    //m_operationMap[0xD2] TODO
    //m_operationMap[0xD3] TODO
    //m_operationMap[0xD4] TODO
    //m_operationMap[0xD5] TODO
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
    //m_operationMap[0xE1] TODO
    m_operationMap[0xE2] = &CPU::LD_0xFF00C_A;
    //m_operationMap[0xE3] TODO
    //m_operationMap[0xE4] TODO
    //m_operationMap[0xE5] TODO
    //m_operationMap[0xE6] TODO
    //m_operationMap[0xE7] TODO
    //m_operationMap[0xE8] TODO
    //m_operationMap[0xE9] TODO
    //m_operationMap[0xEA] TODO
    //m_operationMap[0xEB] TODO
    //m_operationMap[0xEC] TODO
    //m_operationMap[0xED] TODO
    //m_operationMap[0xEE] TODO
    //m_operationMap[0xEF] TODO

    // F0
    //m_operationMap[0xF0] TODO
    //m_operationMap[0xF1] TODO
    //m_operationMap[0xF2] TODO
    //m_operationMap[0xF3] TODO
    //m_operationMap[0xF4] TODO
    //m_operationMap[0xF5] TODO
    //m_operationMap[0xF6] TODO
    //m_operationMap[0xF7] TODO
    //m_operationMap[0xF8] TODO
    //m_operationMap[0xF9] TODO
    //m_operationMap[0xFA] TODO
    //m_operationMap[0xFB] TODO
    //m_operationMap[0xFC] TODO
    //m_operationMap[0xFD] TODO
    //m_operationMap[0xFE] TODO
    //m_operationMap[0xFF] TODO

    /*
        Z80 Command Set - CB
    */

    // 00
    //m_operationMapCB[0x00] TODO
    //m_operationMapCB[0x01] TODO
    //m_operationMapCB[0x02] TODO
    //m_operationMapCB[0x03] TODO
    //m_operationMapCB[0x04] TODO
    //m_operationMapCB[0x05] TODO
    //m_operationMapCB[0x06] TODO
    //m_operationMapCB[0x07] TODO
    //m_operationMapCB[0x08] TODO
    //m_operationMapCB[0x09] TODO
    //m_operationMapCB[0x0A] TODO
    //m_operationMapCB[0x0B] TODO
    //m_operationMapCB[0x0C] TODO
    //m_operationMapCB[0x0D] TODO
    //m_operationMapCB[0x0E] TODO
    //m_operationMapCB[0x0F] TODO

    // 10
    //m_operationMapCB[0x10] TODO
    m_operationMapCB[0x11] = &CPU::RLC;
    //m_operationMapCB[0x12] TODO
    //m_operationMapCB[0x13] TODO
    //m_operationMapCB[0x14] TODO
    //m_operationMapCB[0x15] TODO
    //m_operationMapCB[0x16] TODO
    //m_operationMapCB[0x17] TODO
    //m_operationMapCB[0x18] TODO
    //m_operationMapCB[0x19] TODO
    //m_operationMapCB[0x1A] TODO
    //m_operationMapCB[0x1B] TODO
    //m_operationMapCB[0x1C] TODO
    //m_operationMapCB[0x1D] TODO
    //m_operationMapCB[0x1E] TODO
    //m_operationMapCB[0x1F] TODO

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
    //m_operationMapCB[0x38] TODO
    //m_operationMapCB[0x39] TODO
    //m_operationMapCB[0x3A] TODO
    //m_operationMapCB[0x3B] TODO
    //m_operationMapCB[0x3C] TODO
    //m_operationMapCB[0x3D] TODO
    //m_operationMapCB[0x3E] TODO
    //m_operationMapCB[0x3F] TODO

    // 40
    m_operationMapCB[0x40] = &CPU::BITbr;
    m_operationMapCB[0x41] = &CPU::BITbr;
    m_operationMapCB[0x42] = &CPU::BITbr;
    m_operationMapCB[0x43] = &CPU::BITbr;
    m_operationMapCB[0x44] = &CPU::BITbr;
    m_operationMapCB[0x45] = &CPU::BITbr;
    //m_operationMapCB[0x46] TODO
    m_operationMapCB[0x47] = &CPU::BITbr;
    m_operationMapCB[0x48] = &CPU::BITbr;
    m_operationMapCB[0x49] = &CPU::BITbr;
    m_operationMapCB[0x4A] = &CPU::BITbr;
    m_operationMapCB[0x4B] = &CPU::BITbr;
    m_operationMapCB[0x4C] = &CPU::BITbr;
    m_operationMapCB[0x4D] = &CPU::BITbr;
    //m_operationMapCB[0x4E] TODO
    m_operationMapCB[0x4F] = &CPU::BITbr;

    // 50
    m_operationMapCB[0x50] = &CPU::BITbr;
    m_operationMapCB[0x51] = &CPU::BITbr;
    m_operationMapCB[0x52] = &CPU::BITbr;
    m_operationMapCB[0x53] = &CPU::BITbr;
    m_operationMapCB[0x54] = &CPU::BITbr;
    m_operationMapCB[0x55] = &CPU::BITbr;
    //m_operationMapCB[0x56] TODO
    m_operationMapCB[0x57] = &CPU::BITbr;
    m_operationMapCB[0x58] = &CPU::BITbr;
    m_operationMapCB[0x59] = &CPU::BITbr;
    m_operationMapCB[0x5A] = &CPU::BITbr;
    m_operationMapCB[0x5B] = &CPU::BITbr;
    m_operationMapCB[0x5C] = &CPU::BITbr;
    m_operationMapCB[0x5D] = &CPU::BITbr;
    //m_operationMapCB[0x5E] TODO
    m_operationMapCB[0x5F] = &CPU::BITbr;

    // 60
    m_operationMapCB[0x60] = &CPU::BITbr;
    m_operationMapCB[0x61] = &CPU::BITbr;
    m_operationMapCB[0x62] = &CPU::BITbr;
    m_operationMapCB[0x63] = &CPU::BITbr;
    m_operationMapCB[0x64] = &CPU::BITbr;
    m_operationMapCB[0x65] = &CPU::BITbr;
    //m_operationMapCB[0x66] TODO
    m_operationMapCB[0x67] = &CPU::BITbr;
    m_operationMapCB[0x68] = &CPU::BITbr;
    m_operationMapCB[0x69] = &CPU::BITbr;
    m_operationMapCB[0x6A] = &CPU::BITbr;
    m_operationMapCB[0x6B] = &CPU::BITbr;
    m_operationMapCB[0x6C] = &CPU::BITbr;
    m_operationMapCB[0x6D] = &CPU::BITbr;
    //m_operationMapCB[0x6E] TODO
    m_operationMapCB[0x6F] = &CPU::BITbr;

    // 70
    m_operationMapCB[0x70] = &CPU::BITbr;
    m_operationMapCB[0x71] = &CPU::BITbr;
    m_operationMapCB[0x72] = &CPU::BITbr;
    m_operationMapCB[0x73] = &CPU::BITbr;
    m_operationMapCB[0x74] = &CPU::BITbr;
    m_operationMapCB[0x75] = &CPU::BITbr;
    //m_operationMapCB[0x76] TODO
    m_operationMapCB[0x77] = &CPU::BITbr;
    m_operationMapCB[0x78] = &CPU::BITbr;
    m_operationMapCB[0x79] = &CPU::BITbr;
    m_operationMapCB[0x7A] = &CPU::BITbr;
    m_operationMapCB[0x7B] = &CPU::BITbr;
    m_operationMapCB[0x7C] = &CPU::BITbr;
    m_operationMapCB[0x7D] = &CPU::BITbr;
    //m_operationMapCB[0x7E] TODO
    m_operationMapCB[0x7F] = &CPU::BITbr;

    // 80
    //m_operationMapCB[0x80] TODO
    //m_operationMapCB[0x81] TODO
    //m_operationMapCB[0x82] TODO
    //m_operationMapCB[0x83] TODO
    //m_operationMapCB[0x84] TODO
    //m_operationMapCB[0x85] TODO
    //m_operationMapCB[0x86] TODO
    //m_operationMapCB[0x87] TODO
    //m_operationMapCB[0x88] TODO
    //m_operationMapCB[0x89] TODO
    //m_operationMapCB[0x8A] TODO
    //m_operationMapCB[0x8B] TODO
    //m_operationMapCB[0x8C] TODO
    //m_operationMapCB[0x8D] TODO
    //m_operationMapCB[0x8E] TODO
    //m_operationMapCB[0x8F] TODO

    // 90
    //m_operationMapCB[0x90] TODO
    //m_operationMapCB[0x91] TODO
    //m_operationMapCB[0x92] TODO
    //m_operationMapCB[0x93] TODO
    //m_operationMapCB[0x94] TODO
    //m_operationMapCB[0x95] TODO
    //m_operationMapCB[0x96] TODO
    //m_operationMapCB[0x97] TODO
    //m_operationMapCB[0x98] TODO
    //m_operationMapCB[0x99] TODO
    //m_operationMapCB[0x9A] TODO
    //m_operationMapCB[0x9B] TODO
    //m_operationMapCB[0x9C] TODO
    //m_operationMapCB[0x9D] TODO
    //m_operationMapCB[0x9E] TODO
    //m_operationMapCB[0x9F] TODO

    // A0
    //m_operationMapCB[0xA0] TODO
    //m_operationMapCB[0xA1] TODO
    //m_operationMapCB[0xA2] TODO
    //m_operationMapCB[0xA3] TODO
    //m_operationMapCB[0xA4] TODO
    //m_operationMapCB[0xA5] TODO
    //m_operationMapCB[0xA6] TODO
    //m_operationMapCB[0xA7] TODO
    //m_operationMapCB[0xA8] TODO
    //m_operationMapCB[0xA9] TODO
    //m_operationMapCB[0xAA] TODO
    //m_operationMapCB[0xAB] TODO
    //m_operationMapCB[0xAC] TODO
    //m_operationMapCB[0xAD] TODO
    //m_operationMapCB[0xAE] TODO
    //m_operationMapCB[0xAF] TODO

    // B0
    //m_operationMapCB[0xB0] TODO
    //m_operationMapCB[0xB1] TODO
    //m_operationMapCB[0xB2] TODO
    //m_operationMapCB[0xB3] TODO
    //m_operationMapCB[0xB4] TODO
    //m_operationMapCB[0xB5] TODO
    //m_operationMapCB[0xB6] TODO
    //m_operationMapCB[0xB7] TODO
    //m_operationMapCB[0xB8] TODO
    //m_operationMapCB[0xB9] TODO
    //m_operationMapCB[0xBA] TODO
    //m_operationMapCB[0xBB] TODO
    //m_operationMapCB[0xBC] TODO
    //m_operationMapCB[0xBD] TODO
    //m_operationMapCB[0xBE] TODO
    //m_operationMapCB[0xBF] TODO

    // C0
    //m_operationMapCB[0xC0] TODO
    //m_operationMapCB[0xC1] TODO
    //m_operationMapCB[0xC2] TODO
    //m_operationMapCB[0xC3] TODO
    //m_operationMapCB[0xC4] TODO
    //m_operationMapCB[0xC5] TODO
    //m_operationMapCB[0xC6] TODO
    //m_operationMapCB[0xC7] TODO
    //m_operationMapCB[0xC8] TODO
    //m_operationMapCB[0xC9] TODO
    //m_operationMapCB[0xCA] TODO
    //m_operationMapCB[0xCB] TODO
    //m_operationMapCB[0xCC] TODO
    //m_operationMapCB[0xCD] TODO
    //m_operationMapCB[0xCE] TODO
    //m_operationMapCB[0xCF] TODO

    // D0
    //m_operationMapCB[0xD0] TODO
    //m_operationMapCB[0xD1] TODO
    //m_operationMapCB[0xD2] TODO
    //m_operationMapCB[0xD3] TODO
    //m_operationMapCB[0xD4] TODO
    //m_operationMapCB[0xD5] TODO
    //m_operationMapCB[0xD6] TODO
    //m_operationMapCB[0xD7] TODO
    //m_operationMapCB[0xD8] TODO
    //m_operationMapCB[0xD9] TODO
    //m_operationMapCB[0xDA] TODO
    //m_operationMapCB[0xDB] TODO
    //m_operationMapCB[0xDC] TODO
    //m_operationMapCB[0xDD] TODO
    //m_operationMapCB[0xDE] TODO
    //m_operationMapCB[0xDF] TODO

    // E0
    //m_operationMapCB[0xE0] TODO
    //m_operationMapCB[0xE1] TODO
    //m_operationMapCB[0xE2] TODO
    //m_operationMapCB[0xE3] TODO
    //m_operationMapCB[0xE4] TODO
    //m_operationMapCB[0xE5] TODO
    //m_operationMapCB[0xE6] TODO
    //m_operationMapCB[0xE7] TODO
    //m_operationMapCB[0xE8] TODO
    //m_operationMapCB[0xE9] TODO
    //m_operationMapCB[0xEA] TODO
    //m_operationMapCB[0xEB] TODO
    //m_operationMapCB[0xEC] TODO
    //m_operationMapCB[0xED] TODO
    //m_operationMapCB[0xEE] TODO
    //m_operationMapCB[0xEF] TODO

    // F0
    //m_operationMapCB[0xF0] TODO
    //m_operationMapCB[0xF1] TODO
    //m_operationMapCB[0xF2] TODO
    //m_operationMapCB[0xF3] TODO
    //m_operationMapCB[0xF4] TODO
    //m_operationMapCB[0xF5] TODO
    //m_operationMapCB[0xF6] TODO
    //m_operationMapCB[0xF7] TODO
    //m_operationMapCB[0xF8] TODO
    //m_operationMapCB[0xF9] TODO
    //m_operationMapCB[0xFA] TODO
    //m_operationMapCB[0xFB] TODO
    //m_operationMapCB[0xFC] TODO
    //m_operationMapCB[0xFD] TODO
    //m_operationMapCB[0xFE] TODO
    //m_operationMapCB[0xFF] TODO

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

ushort* CPU::GetUShortRegister(byte val)
{
    // Bottom 2 bits only
    return m_UShortRegisterMap[val & 0x03];
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

    No flags affected
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

    No flags affected
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

    No flags affected
*/
void CPU::LDrrnn(const byte& opCode)
{
    ushort* rr = GetUShortRegister(opCode >> 4);
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

// 0x17 (RL A)
void CPU::RLA(const byte& opCode)
{
    m_cycles += 8;

    // Grab the current CarryFlag val
    bool carry = IsFlagSet(CarryFlag);

    // Grab bit 7 and store it in the carryflag
    if (ISBITSET(GetHighByte(m_AF), 7))
    {
        SetFlag(CarryFlag);
    }

    // Shift C left
    SetHighByte(&m_AF, GetHighByte(m_AF) << 1);

    // Set bit 0 of C to the old CarryFlag
    SetHighByte(&m_AF, carry ? SETBIT(GetHighByte(m_AF), 0) : CLEARBIT(GetHighByte(m_AF), 0));

    // Affects Z, clears N, clears H, affects C
    SetFlag(ZeroFlag);
    ClearFlag(AddFlag);
    ClearFlag(HalfCarryFlag);
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

// 0x52 (LDD (HL), A)
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
// Identical to 0x52, except does not decrement
void CPU::LD_HL_A(const byte& opCode)
{
    if (!m_MMU->WriteByte(m_HL, GetHighByte(m_AF))) // Load A into the address pointed at by HL.
    {
        HALT();
    }

    m_cycles += 8;

    // No flags affected
}

// 0xAF (XOR A)
void CPU::XORA(const byte& opCode)
{
    SetHighByte(&m_AF, GetHighByte(m_AF) ^ GetHighByte(m_AF));
    m_cycles += 4;

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
}

// 0xC1
void CPU::POPBC(const byte& opCode)
{
    m_BC = PopUShort();
    m_cycles += 12;

    // No flags affected
}

// 0xC5
void CPU::PUSHBC(const byte& opCode)
{
    PushUShortToSP(m_BC);
    m_cycles += 16;

    // No flags affected
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
    CPU 0xCB INSTRUCTION MAP
*/

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
    if (!ISBITSET(*r, bit))
    {
        // Z is set if specified bit is 0
        SetFlag(ZeroFlag);
    }
    else
    {
        // Reset otherwise
        ClearFlag(ZeroFlag);
    }

    SetFlag(HalfCarryFlag); // H is set
    ClearFlag(AddFlag); // N is reset
}


// 0x11 (RL C)
void CPU::RLC(const byte& opCode)
{
    m_cycles += 8;

    // Grab the current CarryFlag val
    bool carry = IsFlagSet(CarryFlag);

    // Grab bit 7 and store it in the carryflag
    if (ISBITSET(GetLowByte(m_BC), 7))
    {
        SetFlag(CarryFlag);
    }

    // Shift C left
    SetLowByte(&m_BC, GetLowByte(m_BC) << 1);

    // Set bit 0 of C to the old CarryFlag
    SetLowByte(&m_BC, carry ? SETBIT(GetLowByte(m_BC), 0) : CLEARBIT(GetLowByte(m_BC), 0));

    // Affects Z, clears N, clears H, affects C
    SetFlag(ZeroFlag);
    ClearFlag(AddFlag);
    ClearFlag(HalfCarryFlag);
}
