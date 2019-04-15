
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen_Test/CodeGen_Test.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_GROUP_LIST_BEGIN
    KMS_TEST_GROUP_LIST_ENTRY("Base")
KMS_TEST_GROUP_LIST_END

extern int Filter_Base();

KMS_TEST_LIST_BEGIN
    KMS_TEST_LIST_ENTRY(Filter_Base, "Filter - Base", 0, 0)
KMS_TEST_LIST_END

KMS_TEST_MAIN
