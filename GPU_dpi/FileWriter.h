
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved
// Product    GPU_dpi
// File       FPU_dpi/FileWriter.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Import/Includes ====================================================
#include <OpenNet/Adapter.h>

// Class
/////////////////////////////////////////////////////////////////////////////

class FileWriter
{

public:

    virtual ~FileWriter();

    OpenNet::Adapter::Event_Callback GetEventCallback();

    virtual unsigned int WritePacketHeader(void * aOut, unsigned int aSize_byte, uint64_t aTimestamp_us) = 0;

// internal:

    void ProcessEvent(OpenNetK::Event_Type aType, uint64_t aTimestamp_us, uint32_t aData0, void * aData1);

protected:

    FileWriter(const char * aFileName);

    void Write(const void * aIn, unsigned int aInSize_byte);

private:

    uint8_t mBuffer[16 * 1024 * 64];

    OpenNet::Adapter::Event_Callback mEventCallback;

    #ifdef _KMS_WINDOWS_
        HANDLE mHandle;
    #endif

};
