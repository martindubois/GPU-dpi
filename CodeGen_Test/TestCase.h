
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) KMS 2019. All rights reserved.
// Product    GPU-dpi
// File       CodeGen_Test/TestCase.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Common =============================================================
#include "../Common/Filter.h"

// Data types
/////////////////////////////////////////////////////////////////////////////

typedef struct
{
    GPU_dpi::OutputType  mOutputType  ;
    GPU_dpi::ForwardType mForwardType ;
    const char         * mCode        ;
    const char         * mErrorMessage;
    const char         * mResultFile  ;
}
TestCase;

// Functions
/////////////////////////////////////////////////////////////////////////////

extern bool TestCase_Verify(const TestCase & aCases, Filter * aFilter);
