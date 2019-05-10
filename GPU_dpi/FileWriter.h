
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved
// Product    GPU_dpi
// File       FPU_dpi/FileWriter.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Import/Includes ====================================================
#include <OpenNet/Adapter.h>

// ===== GPU_dpi ============================================================
#include "EventProcessor.h"

// Class
/////////////////////////////////////////////////////////////////////////////

class FileWriter : public EventProcessor
{

public:

    virtual ~FileWriter();

    // ===== EventProcessor =================================================
    virtual void DisplayStatistics(FILE * aOut) const;

protected:

    FileWriter(const char * aFileName);

    void Write(const void * aIn, unsigned int aInSize_byte);

    virtual unsigned int WritePacketHeader(void * aOut, unsigned int aSize_byte, uint64_t aTimestamp_us) = 0;

    // ===== EventProcessor =================================================
    virtual void ProcessBufferEvent_End(OpenNet::Buffer * aBuffer);
    virtual void ProcessPacketEvent    (OpenNet::Buffer * aBuffer, unsigned int aPacket, uint64_t aTimestamp_us);

private:

    uint8_t mBuffer[16 * 1024 * 64];

    unsigned int mSize_byte;

    unsigned int mWrites;

    #ifdef _KMS_WINDOWS_
        HANDLE mHandle;
    #endif

};
