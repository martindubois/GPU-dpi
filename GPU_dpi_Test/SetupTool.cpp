
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) KMS 2019. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi_Test/SetupTool.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== Includes ===========================================================
#include <GPU_dpi/SetupTool.h>

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_BEGIN(SetupTool_Base)
{
    GPU_dpi::SetupTool * lST0 = GPU_dpi::SetupTool::Create(true);

    KMS_TEST_ASSERT(NULL != lST0->GetBinaryFolder ());
    KMS_TEST_ASSERT(NULL != lST0->GetIncludeFolder());
    KMS_TEST_ASSERT(NULL != lST0->GetInstallFolder());
    KMS_TEST_ASSERT(NULL != lST0->GetLibraryFolder());

    KMS_TEST_ASSERT(lST0->IsDebugEnabled());

    #ifdef _KMS_LINUX_
        KMS_TEST_COMPARE(GPU_dpi::STATUS_OK               , lST0->Install  ());
        KMS_TEST_COMPARE(GPU_dpi::STATUS_REBOOT_REQUIRED  , lST0->Uninstall());
    #endif

    #ifdef _KMS_WINDOWS_
        KMS_TEST_COMPARE(GPU_dpi::STATUS_NOT_ADMINISTRATOR, lST0->Install  ());
        KMS_TEST_COMPARE(GPU_dpi::STATUS_OK               , lST0->Uninstall());
    #endif

    KMS_TEST_COMPARE(GPU_dpi::STATUS_INVALID_COMMAND_INDEX, lST0->Interactif_ExecuteCommand (0));
    KMS_TEST_COMPARE(                                    0, lST0->Interactif_GetCommandCount());

    KMS_TEST_ASSERT(NULL == lST0->Interactif_GetCommandText(0));

    unsigned int lPage = 0;

    KMS_TEST_COMPARE(GPU_dpi::STATUS_INVALID_PAGE_INDEX, lST0->Wizard_ExecutePage       (&lPage, 0));
    KMS_TEST_COMPARE(                                 0, lST0->Wizard_GetPageButtonCount(0));
    KMS_TEST_COMPARE(                                 0, lST0->Wizard_GetPageCount      ());

    KMS_TEST_ASSERT(NULL == lST0->Wizard_GetPageButtonText(0, 0));
    KMS_TEST_ASSERT(NULL == lST0->Wizard_GetPageText      (0));
    KMS_TEST_ASSERT(NULL == lST0->Wizard_GetPageTitle     (0));

    lST0->Delete();
}
KMS_TEST_END
