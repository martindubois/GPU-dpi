
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved
// Product    GPU_dpi
// File       FPU_dpi/EventProcessor.cpp

#define __CLASS__ "EventProcessor::"

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
#include <OpenNet/Buffer.h>

// ===== GPU_dpi ============================================================
#include "EventProcessor.h"

// Static function declaration
/////////////////////////////////////////////////////////////////////////////

// ===== Entry point ========================================================
static void ProcessEvent(void * aContext, OpenNetK::Event_Type aType, uint64_t aTimestamp_us, uint32_t aData0, void * aData1);

// Public
/////////////////////////////////////////////////////////////////////////////

EventProcessor::~EventProcessor()
{

}

// Return  The error count
unsigned int EventProcessor::GetErrorCount()
{
    unsigned int lResult = mErrors;

    if (0 != lResult)
    {
        mErrors = 0;
    }

    return lResult;
}

// Return  The address of the OpenNet callback function
OpenNet::Adapter::Event_Callback EventProcessor::GetEventCallback()
{
    return ::ProcessEvent;
}

// aOut [---;RW-] The output stream
void EventProcessor::DisplayStatistics(FILE * aOut) const
{
    assert(NULL != aOut);

    fprintf(aOut, "%8u buffer events\n", mBufferEvents);
    fprintf(aOut, "%8u bytes\n"  , mBytes  );
    fprintf(aOut, "%8u events\n" , mEvents );
    fprintf(aOut, "%8u false buffer events\n", mFalseBufferEvents);
    fprintf(aOut, "%8u packets\n", mPackets);
    fprintf(aOut, "Data0 %u <= %u <= %u, Sum = %u\n", mData0_Min, mData0_Last, mData0_Max, mData0_Sum);
    fprintf(aOut, "Debugs = %u packets\n", mDebugs);
}

// Internal
/////////////////////////////////////////////////////////////////////////////

// aType
// aTimestamp_us
// aData0
// aData1
void EventProcessor::ProcessEvent(OpenNetK::Event_Type aType, uint64_t aTimestamp_us, uint32_t aData0, void * aData1)
{
    mEvents++;

    switch (aType)
    {
    case OpenNetK::EVENT_TYPE_BUFFER:
        assert(NULL != aData1);

        if (mData0_Max < aData0) { mData0_Max = aData0; }
        if (mData0_Min > aData0) { mData0_Min = aData0; }

        mData0_Last = aData0;
        mData0_Sum += aData0;

        mBufferEvents++;

        OpenNet::Buffer * lBuffer    = reinterpret_cast<OpenNet::Buffer *>(aData1);
        unsigned int      lIndex     = 0;
        unsigned int      lPackets   = 0;
        unsigned int      lSize_byte = 0;

        for (unsigned int i = 0; i < lBuffer->GetPacketCount(); i++)
        {
            if (65 == lBuffer->GetPacketSize(i))
            {
                mDebugs++;
            }
        }

        while (0xffffffff != (lIndex = lBuffer->GetPacketEvent(lIndex)))
        {
            unsigned int lPacketSize_byte = lBuffer->GetPacketSize(lIndex);

            mBytes += lPacketSize_byte;
            mPackets++;

            ProcessPacketEvent(lBuffer, lIndex, aTimestamp_us);

            lIndex  ++;
            lPackets++;
        }

        OpenNet::Status lStatus = lBuffer->ClearEvent();
        assert(OpenNet::STATUS_OK == lStatus);

        if (0 >= lPackets)
        {
            mFalseBufferEvents++;
        }

        lStatus = lBuffer->Wait();
        assert(OpenNet::STATUS_OK == lStatus);

        ProcessBufferEvent_End(lBuffer);

        // TODO  GPU_dpi.FileWriter
        //       Normal (Feature) - Ajouter un compteur de statistics pour
        //       les fauts BufferEvent

        break;
    }
}

// Protected
/////////////////////////////////////////////////////////////////////////////

EventProcessor::EventProcessor() : mBufferEvents(0), mBytes(0), mData0_Last(0), mData0_Max(0), mData0_Min(0xffffffff), mData0_Sum(0), mDebugs(0), mEvents(0), mFalseBufferEvents(0), mPackets(0)
{
}

// Static function
/////////////////////////////////////////////////////////////////////////////

// ===== Entry point ========================================================

void ProcessEvent(void * aContext, OpenNetK::Event_Type aType, uint64_t aTimestamp_us, uint32_t aData0, void * aData1)
{
    assert(NULL != aContext);

    EventProcessor * lThis = reinterpret_cast<EventProcessor *>(aContext);

    lThis->ProcessEvent(aType, aTimestamp_us, aData0, aData1);
}
