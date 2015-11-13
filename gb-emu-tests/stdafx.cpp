// stdafx.cpp : source file that includes just the standard includes
// gb-emu-tests.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if !WINDOWS
bool Assert::m_Failed = false;
const char* Assert::m_Name = nullptr;

void Assert::Reset(const char* name)
{
    m_Name = name;
    m_Failed = false;

    std::cout << "Running Test: " << name;
}

bool Assert::HasFailed()
{
    if (m_Failed)
    {
        std::cout << " - FAILED !!!!";
    }
    else
    {
        std::cout << " Passed!";
    }

    std::cout << std::endl;

    return m_Failed;
}

void Assert::AreEqual(int expected, int actual)
{
    if (m_Failed)
        return;
    m_Failed = (expected != actual);

    if (m_Failed)
    {
        std::cout << std::endl;
        std::cout << "    - Assert::AreEqual - Expected=" << expected << "  Actual=" << actual << std::endl;
    }
}

void Assert::IsTrue(bool condition)
{
    if (m_Failed)
        return;
    m_Failed = (condition != true);
}

void Assert::IsFalse(bool condition)
{
    if (m_Failed)
        return;
    m_Failed = (condition != false);
}
#endif
