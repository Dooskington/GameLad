#include "PCH.hpp"
#include "CPU.hpp"
#include "Logger.hpp"

CPU::CPU() :
    AF(0x0000),
    BC(0x0000),
    DE(0x0000),
    HL(0x0000),
    SP(0x0000),
    PC(0x0000)
{
    Logger::Log("CPU Created.");

    // Initialize memory to 0x0000
    for (int i = 0; i < 0xFFFF; i++)
    {
        memory[i] = 0x0000;
    }
    
    // Initialize operationMap
    operationMap[0x00] = &CPU::NOP;
}

CPU::~CPU()
{
    Logger::Log("CPU Destroyed.");
}

void CPU::StepFrame()
{
    //(this->*opPointer)();
}

void CPU::NOP()
{

}