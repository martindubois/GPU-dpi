
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved
// Product    GPU_dpi
// File       FPU_dpi/EventProcessor.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Import/Includes ====================================================
#include <OpenNet/Adapter.h>
#include <OpenNet/Buffer.h>

// Class
/////////////////////////////////////////////////////////////////////////////

class EventProcessor
{

public:

    virtual ~EventProcessor();

    unsigned int                     GetErrorCount   ();
    OpenNet::Adapter::Event_Callback GetEventCallback();

    virtual void DisplayStatistics(FILE * aOut) const;

// internal:

    void ProcessEvent(OpenNetK::Event_Type aType, uint64_t aTimestamp_us, uint32_t aData0, void * aData1);

protected:

    EventProcessor();

    virtual void ProcessBufferEvent_End(OpenNet::Buffer * aBuffer) = 0;
    virtual void ProcessPacketEvent    (OpenNet::Buffer * aBuffer, unsigned int aPacket, uint64_t aTimestamp_us) = 0;

    unsigned int mErrors ;

private:

    // ===== Statistics =====================================================
    unsigned int mBufferEvents     ;
    unsigned int mBytes            ;
    unsigned int mData0_Last       ;
    unsigned int mData0_Max        ;
    unsigned int mData0_Min        ;
    unsigned int mData0_Sum        ;
    unsigned int mDebugs           ;
    unsigned int mEvents           ;
    unsigned int mFalseBufferEvents;
    unsigned int mPackets          ;

};
