
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved
// Product    GPU_dpi
// File       FPU_dpi/FileWriter_PCAP.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== GPU_dpi ============================================================
#include "FileWriter.h"

// Class
/////////////////////////////////////////////////////////////////////////////

class FileWriter_PCAP : public FileWriter
{

public:

    FileWriter_PCAP(const char * aFileName);

    // ===== FileWriter =====================================================
    virtual ~FileWriter_PCAP();

    virtual unsigned int WritePacketHeader(void * aOut, unsigned int aSize_byte, uint64_t aTimestamp_us);

protected:

private:

};
