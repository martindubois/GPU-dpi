
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved
// Product    GPU-dpi
// File       FPU_dpi/CallbackCaller.cpp

#define __CLASS__ "CallbackCaller::"

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <stdint.h>

#ifdef _KMS_WINDOWS_
    // ===== Windows ========================================================
    #include <Windows.h>
#endif

// ===== Import/Includes ====================================================
#include <KmsLib/Exception.h>
#include <OpenNet/Buffer.h>

// ===== GPU_dpi ============================================================
#include "CallbackCaller.h"

// Public
/////////////////////////////////////////////////////////////////////////////

// aCallback [-K-;--X]
// aContext  [-KO;---]
CallbackCaller::CallbackCaller(GPU_dpi::OutputCallback aCallback, void * aContext)
    : mCallback(aCallback)
    , mContext (aContext )
{
    assert(NULL != aCallback);
}

CallbackCaller::~CallbackCaller()
{
}

// Protected
/////////////////////////////////////////////////////////////////////////////

// ===== EventProcessor =================================================

// aBuffer [---;RW-] The buffer for which an event has been raised
void CallbackCaller::ProcessBufferEvent_End(OpenNet::Buffer * aBuffer)
{
    assert(NULL != aBuffer);

    unsigned int lIndex = 0;

    while (0xffffffff != (lIndex = aBuffer->GetPacketEvent(lIndex)))
    {
        mCallback(mContext, mBuffer[lIndex], aBuffer->GetPacketSize(lIndex));

        lIndex++;
    }
}

// aBuffer [---;RW-] The buffer for which an event has been raised
// aPacket           The index of the packet for which and event has been raised
// aTimestamp_us
void CallbackCaller::ProcessPacketEvent(OpenNet::Buffer * aBuffer, unsigned int aPacket, uint64_t aTimestamp_us)
{
    assert(NULL != aBuffer);

    unsigned int lPacketSize_byte = aBuffer->GetPacketSize(aPacket);

    OpenNet::Status lStatus = aBuffer->ReadPacket(aPacket, mBuffer[aPacket], lPacketSize_byte);
    assert(OpenNet::STATUS_OK == lStatus);
}
