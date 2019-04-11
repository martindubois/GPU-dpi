
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/Utils.cpp

// TEST COVERAGE  2019-04-09  KMS - Martin Dubois, ing.

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== GPU_dpi ============================================================
#include "Utils.h"

// Functions
/////////////////////////////////////////////////////////////////////////////

// NOT TESTED  GPU_dpi.Utils.Error
//             Unexpected OpenNet status
GPU_dpi::Status Utl_ConvertOpenNetStatus(OpenNet::Status aStatus)
{
    switch (aStatus)
    {
    case OpenNet::STATUS_OK: return GPU_dpi::STATUS_OK;

    case OpenNet::STATUS_NOT_ADMINISTRATOR : return GPU_dpi::STATUS_NOT_ADMINISTRATOR ;
    case OpenNet::STATUS_OPEN_CL_ERROR     : return GPU_dpi::STATUS_OPEN_CL_ERROR     ;
    case OpenNet::STATUS_SYSTEM_NOT_STARTED: return GPU_dpi::STATUS_SYSTEM_NOT_STARTED;
    }

    fprintf(stderr, "%s (0x%08x) ==> GPU_dpi::STATUS_UNEXPECTED_OPEN_NET_STATUS\n", OpenNet::Status_GetName(aStatus), aStatus);

    return GPU_dpi::STATUS_UNEXPECTED_OPEN_NET_STATUS;
}
