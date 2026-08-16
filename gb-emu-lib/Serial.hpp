#pragma once

class Serial : public IMemoryUnit
{
public:
    Serial(ICPU* pCPU = nullptr);
    ~Serial();

    void PreBoot();
    void Step(unsigned long cycles);
    void ResetDivider();
    void AcknowledgeInterrupt(bool doubleSpeed = false);
    bool IsWaitingForExternalClock() const;
    bool ClockExternalBit(bool incomingBit, bool& outgoingBit);

    void SetGameBoyMode(GameBoyMode mode) { m_mode = mode; }

    // IMemoryUnit
    byte ReadByte(const ushort& address);
    bool WriteByte(const ushort& address, const byte val);

private:
    void ClockMasterEdge();
    void ShiftBit(bool incomingBit);
    void CompleteTransfer();
    unsigned long ClockMask() const;
    byte ControlMask() const;

    ICPU* m_CPU;
    GameBoyMode m_mode;
    byte m_Data;
    byte m_Control;
    byte m_BitsTransferred;
    bool m_MasterClock;
    unsigned long m_ClockCycles;
    byte m_InterruptSuppressCycles;
};
