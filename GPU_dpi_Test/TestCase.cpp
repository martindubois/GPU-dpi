
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) KMS 2019. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi_Test/TestCase.cpp

#define __CLASS__ "TestCase::"

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KmsLib/ThreadBase.h>

// ===== GPU_dpi_Test =======================================================
#include "TestCase.h"

// Public
/////////////////////////////////////////////////////////////////////////////

// aContext [---;RW-]
// aData    [---;R--]
// aSize_byte
void TestCase::Callback(void * aContext, const void * aData, unsigned int aSize_byte)
{
    assert(NULL != aContext );
    assert(NULL != aData    );
    assert(  64 < aSize_byte);

    TestCase * lThis = reinterpret_cast<TestCase *>(aContext);

    lThis->ProcessPacket(aData, aSize_byte);
}

// aInfo [---;R--]
TestCase::TestCase(const Info & aInfo) : mInfo(aInfo), mRepeat(0)
{
    assert(NULL != (&aInfo));
}

void TestCase::DisplayStatistics() const
{
    printf("Repeat = %u packets\n", mRepeat);
}

// Return
//  false  Test failed
//  true   Test passed
bool TestCase::Verify()
{
    bool lResult = true;

    for ( unsigned int i = 0; i < (sizeof(mInfo.mPackets) / sizeof(mInfo.mPackets[0])); i ++)
    {
        unsigned short lSize_byte = mInfo.mPackets[i];
        if (0 == lSize_byte)
        {
            break;
        }

        if (1 != mResult.erase(lSize_byte))
        {
            printf("%u was expected but has not been received\n", lSize_byte);
            lResult = false;
        }
    }

    for (SizeSet::iterator lIt = mResult.begin(); lIt != mResult.end(); lIt ++)
    {
        printf("%u received but not expected\n", (*lIt));
        lResult = false;
    }

    mResult.clear();

    return lResult;
}

// Return
//  false  Timeout
//  true   OK
bool TestCase::Wait()
{
    for (unsigned int i = 0; i < 500; i ++)
    {
        if (0 < mRepeat)
        {
            return true;
        }

        KmsLib::ThreadBase::Sleep_ms(10);
    }

    return false;
}

// Private
/////////////////////////////////////////////////////////////////////////////

// aData [---;R--;
// aSize_byte
void TestCase::ProcessPacket(const void * aData, unsigned int aSize_byte)
{
    // printf(__CLASS__ "ProcessPacket( , %u bytes )\n", aSize_byte);

    assert(NULL != aData     );
    assert(64   <= aSize_byte);

    SizeSet::iterator lIt = mResult.find(aSize_byte);
    if (mResult.end() == lIt)
    {
        mResult.insert(aSize_byte);
    }
    else
    {
        mRepeat++;
    }
}
