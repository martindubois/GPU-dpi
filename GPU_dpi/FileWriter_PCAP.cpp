
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved
// Product    GPU-dpi
// File       FPU_dpi/FileWriter_PCAP.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <stdint.h>

#ifdef _KMS_WINDOWS_
    // ===== Windows ========================================================
    #include <Windows.h>
#endif

// ===== GPU_dpi ============================================================
#include "PCap.h"

#include "FileWriter_PCAP.h"

// Public
/////////////////////////////////////////////////////////////////////////////

FileWriter_PCAP::FileWriter_PCAP(const char * aFileName) : FileWriter(aFileName)
{
    assert(NULL != aFileName);

    PCap_Header lH;

    memset(&lH, 0, sizeof(lH));

    lH.mMagicNumber   = PCAP_MAGIC_NUMBER;
    lH.mMaxSize_byte  = PCAP_MAX_SIZE_DEFAULT;
    lH.mNetworkType   = PCAP_NETWORK_TYPE_ETHERNET;
    lH.mVersion_Major = PCAP_VERSION_MAJOR;
    lH.mVersion_Minor = PCAP_VERSION_MINOR;

    Write(&lH, sizeof(lH));
}

// ===== FileWriter =========================================================

FileWriter_PCAP::~FileWriter_PCAP()
{
}

unsigned int FileWriter_PCAP::WritePacketHeader(void * aOut, unsigned int aSize_byte, uint64_t aTimestamp_us)
{
    assert(NULL != aOut);

    PCap_PacketHeader * lPH = reinterpret_cast<PCap_PacketHeader *>(aOut);

    lPH->mIncludedSize_byte = aSize_byte          ;
    lPH->mSize_byte         = aSize_byte          ;
    lPH->mTimestamp_s       = static_cast<uint32_t>(aTimestamp_us / 1000000);
    lPH->mTimestamp_us      = aTimestamp_us % 1000000;

    return sizeof(PCap_PacketHeader);
}
