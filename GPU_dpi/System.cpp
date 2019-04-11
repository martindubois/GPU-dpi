
// Author     KMS - Martin Dubois. ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/System.cpp

// TEST COVERAGE  2019-04-09  KMS - Martin Dubois, ing.

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

// ===== GPU_dpi ============================================================
#include "System_Internal.h"

namespace GPU_dpi
{

    // Public
    /////////////////////////////////////////////////////////////////////////

    // NOT TESTED  GPU_dpi.System.Error
    //             System_Internal contructor throw an exception
    System * System::Create()
    {
        System * lResult;

        try
        {
            lResult = new System_Internal();
        }
        catch (KmsLib::Exception * eE)
        {
            eE->Write(stderr);
            lResult = NULL;
        }

        return lResult;
    }

    void System::Delete()
    {
        assert(NULL != this);

        delete this;
    }

    // Protected
    /////////////////////////////////////////////////////////////////////////

    System::System()
    {
    }

    System::~System()
    {
    }

}
