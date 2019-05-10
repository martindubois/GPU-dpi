
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) KMS 2019. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi_Test/TestCase.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== C ++ ===============================================================
#include <set>

// Class
/////////////////////////////////////////////////////////////////////////////

class TestCase
{

public:

    typedef struct
    {
        const char   * mFilter ;
        unsigned short mPackets[ 2 ];
    }
    Info;

    static void Callback(void * aContext, const void * aData, unsigned int aSize_byte);

    TestCase( const Info & aInfo );

    void DisplayStatistics() const;

    bool Verify();

    bool Wait();

private:

    typedef std::set<unsigned short> SizeSet;

    void ProcessPacket(const void * aData, unsigned int aSize_byte);

    const Info & mInfo  ;
    unsigned int mRepeat;
    SizeSet      mResult;

};
