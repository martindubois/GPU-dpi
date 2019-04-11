
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) KMS 2019. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi_Test/System.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== Includes/OpenNet ===================================================
#include <GPU_dpi/System.h>

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_BEGIN(System_Base)
{
    GPU_dpi::AdapterConfig lConfig;
    GPU_dpi::AdapterInfo   lInfo  ;

    // ===== Create =========================================================
    GPU_dpi::System * lS0 = GPU_dpi::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    // ===== Adapter_GetCount ===============================================
    KMS_TEST_COMPARE(0, lS0->Adapter_GetCount());

    // ===== Adapter_GetConfig ==============================================
    KMS_TEST_COMPARE(GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT, lS0->Adapter_GetConfig(0, NULL));
    KMS_TEST_COMPARE(GPU_dpi::STATUS_INVALID_ADAPTER_INDEX, lS0->Adapter_GetConfig(0, &lConfig));

    // ===== Adapter_GetInfo ================================================
    KMS_TEST_COMPARE(GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT, lS0->Adapter_GetInfo(0, NULL));
    KMS_TEST_COMPARE(GPU_dpi::STATUS_INVALID_ADAPTER_INDEX, lS0->Adapter_GetInfo(0, &lInfo));

    // ===== Adapter_ResetConfig ============================================
    KMS_TEST_COMPARE(GPU_dpi::STATUS_INVALID_ADAPTER_INDEX, lS0->Adapter_ResetConfig(0));

    // ===== Adapter_SetConfig ==============================================
    KMS_TEST_COMPARE(GPU_dpi::STATUS_INVALID_REFERENCE    , lS0->Adapter_SetConfig(0, *reinterpret_cast<GPU_dpi::AdapterConfig *>(NULL)));
    KMS_TEST_COMPARE(GPU_dpi::STATUS_INVALID_ADAPTER_INDEX, lS0->Adapter_SetConfig(0, lConfig));

    // ===== Display ========================================================
    KMS_TEST_COMPARE(GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT, lS0->Display(NULL  ));
    KMS_TEST_COMPARE(GPU_dpi::STATUS_OK                       , lS0->Display(stdout));

    // ===== Processor_GetCount =============================================
    KMS_TEST_COMPARE(0, lS0->Processor_GetCount());

    // ===== Start ==========================================================
    KMS_TEST_COMPARE(GPU_dpi::STATUS_NO_ADAPTER, lS0->Start());

    // ===== Stop ===========================================================
    KMS_TEST_COMPARE(GPU_dpi::STATUS_SYSTEM_NOT_STARTED, lS0->Stop());

    // ===== Delete =========================================================
    lS0->Delete();
}
KMS_TEST_END

KMS_TEST_BEGIN(System_Display)
{
    printf("QUESTION  Is the output OK? (y/n)\n");
    char lLine[1024];
    KMS_TEST_ASSERT( NULL != fgets(lLine, sizeof(lLine), stdin) );
    KMS_TEST_COMPARE(0, strncmp("y", lLine, 1));
}
KMS_TEST_END
