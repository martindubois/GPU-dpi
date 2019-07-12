
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU_dpi
// File       GPU_dpi/Component.h

// CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

#pragma once

#define GPU_DPI_EXPORTS

// Includes
/////////////////////////////////////////////////////////////////////////////

#include <KmsBase.h>

// ===== C ==================================================================
#include <assert.h>

// Macros
/////////////////////////////////////////////////////////////////////////////

#ifdef _KMS_LINUX_
    #define _unlink   unlink
    #define fprintf_s fprintf
#endif
