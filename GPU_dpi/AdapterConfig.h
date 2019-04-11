
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/AdapterConfig.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Includes ===========================================================
#include <GPU_dpi/Status.h>
#include <GPU_dpi/Types.h>

// Functions
/////////////////////////////////////////////////////////////////////////////

extern bool            AdapterConfig_IsCapturing(const GPU_dpi::AdapterConfig & aConfig);
extern bool            AdapterConfig_IsSendingTo(const GPU_dpi::AdapterConfig & aConfig, unsigned int aTo);
extern void            AdapterConfig_Reset      (      GPU_dpi::AdapterConfig * aConfig);
extern GPU_dpi::Status AdapterConfig_Verify     (const GPU_dpi::AdapterConfig & aConfig);

