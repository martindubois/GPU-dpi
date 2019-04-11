
// Author   KMS - Martin Dubois, ing.
// Product  GPU-dpi.h
// File     GPU_dpi_Extcap/PCap.h

#pragma once

// Constants
/////////////////////////////////////////////////////////////////////////////

#define PCAP_MAGIC_NUMBER          (0xa1b2c3d4)
#define PCAP_MAX_SIZE_DEFAULT      (65535)
#define PCAP_NETWORK_TYPE_ETHERNET (1)
#define PCAP_VERSION_MAJOR         (2)
#define PCAP_VERSION_MINOR         (4)

// Data types
/////////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint32_t mMagicNumber  ;
    uint16_t mVersion_Major;
    uint16_t mVersion_Minor;
    int32_t  mTimeZone_s   ;
    uint32_t mReserved0    ;
    uint32_t mMaxSize_byte ;
    uint32_t mNetworkType  ;
}
PCap_Header;

typedef struct
{
    uint32_t mTimestamp_s      ;
    uint32_t mTimestamp_us     ;
    uint32_t mIncludedSize_byte;
    uint32_t mSize_byte        ;
}
PCap_PacketHeader;
