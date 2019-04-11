
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/SetupTool.cpp

// TEST COVERAGE  2019-04-09  KMS - Martin Dubois, ing.

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== Includes ===========================================================
#include <GPU_dpi/SetupTool.h>

// ===== OpenNet ============================================================
#include "SetupTool_Internal.h"

namespace GPU_dpi
{

    // Public
    /////////////////////////////////////////////////////////////////////////

    // NOT TESTED  GPU_dpi.SetupTool.Error
    //             SetupTool_Internal constructor throw an exception
    SetupTool * SetupTool::Create(bool aDebug)
    {
        SetupTool * lResult;

        try
        {
            lResult = new SetupTool_Internal(aDebug);
        }
        catch (...)
        {
            lResult = NULL;
        }

        return lResult;
    }

    void SetupTool::Delete()
    {
        assert(NULL != this);

        delete this;
    }

    // Protected
    /////////////////////////////////////////////////////////////////////////

    SetupTool::SetupTool()
    {
    }

    SetupTool::~SetupTool()
    {
    }

}
