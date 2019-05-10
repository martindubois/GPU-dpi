
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved
// Product    GPU_dpi
// File       FPU_dpi/CallbackCaller.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Import/Includes ====================================================
#include <OpenNet/Adapter.h>

// ===== Includes ===========================================================
#include <GPU_dpi/Types.h>

// ===== GPU_dpi ============================================================
#include "EventProcessor.h"

// Class
/////////////////////////////////////////////////////////////////////////////

class CallbackCaller : public EventProcessor
{

public:

    CallbackCaller(GPU_dpi::OutputCallback aCallback, void * aContext);

    virtual ~CallbackCaller();

protected:

    // ===== EventProcessor =================================================
    virtual void ProcessBufferEvent_End(OpenNet::Buffer * aBuffer);
    virtual void ProcessPacketEvent    (OpenNet::Buffer * aBuffer, unsigned int aPacket, uint64_t aTimestamp_us);

private:

    uint8_t mBuffer[64][16 * 1024];

    GPU_dpi::OutputCallback mCallback;
    void                  * mContext ;

};
