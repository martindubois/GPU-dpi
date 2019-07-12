
// Product  GPU-dpi

/// \author     KMS - Martin Dubois, ing.
/// \copyright  Copyright &copy; 2019 KMS. All rights reserved.
/// \file       Includes/GPU_dpi/GPU_dpi.h

// CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

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
    #ifdef GPU_DPI_EXPORTS
        #define GPU_DPI_PUBLIC __attribute__ ( ( visibility ( "default" ) ) )
    #else
        #define GPU_DPI_PUBLIC
    #endif
#endif
