
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi_Test/OpenNet_Test.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_GROUP_LIST_BEGIN
    KMS_TEST_GROUP_LIST_ENTRY("Base"   )
    KMS_TEST_GROUP_LIST_ENTRY("Display")
KMS_TEST_GROUP_LIST_END

extern int SetupTool_Base();

extern int System_Base   ();
extern int System_Display();

KMS_TEST_LIST_BEGIN
    KMS_TEST_LIST_ENTRY(SetupTool_Base, "SetupTool - Base", 0, 0)
    KMS_TEST_LIST_ENTRY(System_Base   , "System - Base"   , 0, 0)
    KMS_TEST_LIST_ENTRY(System_Display, "System - Display", 1, KMS_TEST_FLAG_INTERACTION_NEEDED)
KMS_TEST_LIST_END

KMS_TEST_MAIN
