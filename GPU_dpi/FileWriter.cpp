
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved
// Product    GPU_dpi
// File       FPU_dpi/FileWriter.cpp

#define __CLASS__ "FileWriter::"

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
#include "FileWriter.h"

// Static function declaration
/////////////////////////////////////////////////////////////////////////////

static void ProcessEvent(void * aContext, OpenNetK::Event_Type aType, uint64_t aTimestamp_us, uint32_t aData0, void * aData1);

// Public
/////////////////////////////////////////////////////////////////////////////

FileWriter::~FileWriter()
{
    #ifdef _KMS_WINDOWS_

        assert(INVALID_HANDLE_VALUE != mHandle);

        BOOL lRetB = CloseHandle(mHandle);
        assert(lRetB);

    #endif
}

OpenNet::Adapter::Event_Callback FileWriter::GetEventCallback()
{
    return ::ProcessEvent;
}

// Internal
/////////////////////////////////////////////////////////////////////////////

void FileWriter::ProcessEvent(OpenNetK::Event_Type aType, uint64_t aTimestamp_us, uint32_t aData0, void * aData1)
{
    switch (aType)
    {
    case OpenNetK::EVENT_TYPE_BUFFER:
        assert(NULL != aData1);

        OpenNet::Buffer * lBuffer    = reinterpret_cast<OpenNet::Buffer *>(aData1);
        unsigned int      lIndex     = 0;
        unsigned int      lSize_byte = 0;
        OpenNet::Status   lStatus;

        while (0xffffffff != (lIndex = lBuffer->GetPacketEvent(lIndex)))
        {
            unsigned int lPacketSize_byte = lBuffer->GetPacketSize(lIndex);

            lSize_byte += WritePacketHeader(mBuffer + lSize_byte, lBuffer->GetPacketSize(lIndex), aTimestamp_us);

            lStatus = lBuffer->ReadPacket(lIndex, mBuffer + lSize_byte, lPacketSize_byte);
            assert(OpenNet::STATUS_OK == lStatus);

            lIndex++;
            lSize_byte += lPacketSize_byte;

            assert(sizeof(mBuffer) >= lSize_byte);
        }

        lStatus = lBuffer->ClearEvent();
        assert(OpenNet::STATUS_OK == lStatus);

        lStatus = lBuffer->Wait();
        assert(OpenNet::STATUS_OK == lStatus);

        Write(mBuffer, lSize_byte);

        break;
    }
}

// Protected
/////////////////////////////////////////////////////////////////////////////

FileWriter::FileWriter(const char * aFileName)
{
    #ifdef _KMS_WINDOWS_

        mHandle = CreateFile(aFileName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
        if (INVALID_HANDLE_VALUE == mHandle)
        {
            char lMsg[1024];

            sprintf_s(lMsg, "CreateFile( \"%s\", , , , , ,  ) failed", aFileName);

            throw new KmsLib::Exception(KmsLib::Exception::CODE_IO_ERROR,
                "CreateFile( , , , , , ,  ) failed", lMsg, __FILE__, __CLASS__ "FilterWriter", __LINE__, GetLastError());
        }

    #endif
}

void FileWriter::Write(const void * aIn, unsigned int aInSize_byte)
{
    assert(NULL != aIn         );
    assert(   0 <  aInSize_byte);

    #ifdef _KMS_WINDOWS_

        DWORD lInfo_byte;

        BOOL lRetB = WriteFile(mHandle, aIn, aInSize_byte, &lInfo_byte, NULL);
        assert(lRetB                     );
        assert(aInSize_byte == lInfo_byte);

    #endif
}

// Static function
/////////////////////////////////////////////////////////////////////////////

void ProcessEvent(void * aContext, OpenNetK::Event_Type aType, uint64_t aTimestamp_us, uint32_t aData0, void * aData1)
{
    assert(NULL != aContext);

    FileWriter * lThis = reinterpret_cast<FileWriter *>(aContext);

    lThis->ProcessEvent(aType, aTimestamp_us, aData0, aData1);
}
