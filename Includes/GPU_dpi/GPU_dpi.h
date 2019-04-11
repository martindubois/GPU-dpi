
// Product  GPU-dpi

/// \author     KMS - Martin Dubois, ing.
/// \copyright  Copyright &copy; 2019 KMS. All rights reserved.
/// \file       Includes/GPU_dpi/GPU_dpi.h

#pragma once

// Macros
/////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
    #ifdef GPU_DPI_EXPORTS
        #define GPU_DPI_PUBLIC __declspec( dllexport )
    #else
        #define GPU_DPI_PUBLIC __declspec( dllimport )
    #endif
#else
    // TODO  GPU-dpi
    //       Normal - On Linux, Do not export all symbols
    #define GPU_DPI_PUBLIC
#endif
