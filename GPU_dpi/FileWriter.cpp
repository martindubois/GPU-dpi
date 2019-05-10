
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

// ===== EventProcessor =====================================================

// aOut [---;RW-] The output stream
void FileWriter::DisplayStatistics(FILE * aOut) const
{
    assert(NULL != aOut);

    EventProcessor::DisplayStatistics(aOut);

    fprintf(aOut, "%8u writes\n", mWrites);
}

// Protected
/////////////////////////////////////////////////////////////////////////////

// aFileName [---;R--] The file to write data to
FileWriter::FileWriter(const char * aFileName)
    : mSize_byte(0)
    , mWrites   (0)
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

// aIn [---;R--] The data to write
// aInSize_byte
void FileWriter::Write(const void * aIn, unsigned int aInSize_byte)
{
    assert(NULL != aIn         );
    assert(   0 <  aInSize_byte);

    #ifdef _KMS_WINDOWS_

        DWORD lInfo_byte;

        if (WriteFile(mHandle, aIn, aInSize_byte, &lInfo_byte, NULL))
        {
            assert(aInSize_byte == lInfo_byte);

            mWrites++;
        }
        else
        {
            mErrors++;
        }

    #endif
}

// ===== EventProcessor =================================================

void FileWriter::ProcessBufferEvent_End(OpenNet::Buffer * aBuffer)
{
    if (0 < mSize_byte)
    {
        Write(mBuffer, mSize_byte);
        mSize_byte = 0;
    }
}

void FileWriter::ProcessPacketEvent(OpenNet::Buffer * aBuffer, unsigned int aPacket, uint64_t aTimestamp_us)
{
    assert(NULL != aBuffer);

    unsigned int lPacketSize_byte = aBuffer->GetPacketSize(aPacket);

    mSize_byte += WritePacketHeader(mBuffer + mSize_byte, aBuffer->GetPacketSize(aPacket), aTimestamp_us);

    OpenNet::Status lStatus = aBuffer->ReadPacket(aPacket, mBuffer + mSize_byte, lPacketSize_byte);
    assert(OpenNet::STATUS_OK == lStatus);

    mSize_byte += lPacketSize_byte;

    assert(sizeof(mBuffer) >= mSize_byte);
}
