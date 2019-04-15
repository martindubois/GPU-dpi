
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) KMS 2019. All rights reserved.
// Product    GPU-dpi
// File       CodeGen_Test/Filter.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== CodeGen_Test =======================================================
#include "TestCase.h"

// Test cases
/////////////////////////////////////////////////////////////////////////////

static const TestCase TEST_CASES[] =
{
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, ""                     , "ERROR 1 - Empty filter", NULL        }, //  0
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, " "                    , "ERROR 1 - Empty filter", NULL        },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "port 1"               , NULL                    , "Filter_01" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "( port 1 )"           , NULL                    , "Filter_01" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "(port 1)"             , NULL                    , "Filter_01" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "not port 1"           , NULL                    , "Filter_02" }, //  5
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "not ( port 1 )"       , NULL                    , "Filter_02" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "not (port 1)"         , NULL                    , "Filter_02" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "( not port 1 )"       , NULL                    , "Filter_02" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "(not port 1)"         , NULL                    , "Filter_02" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "! port 1"             , NULL                    , "Filter_02" }, // 10
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "!port 1"              , NULL                    , "Filter_02" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "! ( port 1 )"         , NULL                    , "Filter_02" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "!(port 1)"            , NULL                    , "Filter_02" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "( ! port 1 )"         , NULL                    , "Filter_02" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "(!port 1)"            , NULL                    , "Filter_02" }, // 15
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "port 1 and port 2"    , NULL                    , "Filter_03" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "port 1 && port 2"     , NULL                    , "Filter_03" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "port 1 or port 2"     , NULL                    , "Filter_04" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "port 1 || port 2"     , NULL                    , "Filter_04" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "dst port 1"           , NULL                    , "Filter_05" }, // 20
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "src port 1"           , NULL                    , "Filter_06" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "portrange 1 - 2"      , NULL                    , "Filter_07" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "portrange 1-2"        , NULL                    , "Filter_07" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "dst portrange 1-2"    , NULL                    , "Filter_08" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "src portrange 1-2"    , NULL                    , "Filter_09" }, // 25
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "udp dst port 1"       , NULL                    , "Filter_10" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "udp dst portrange 1-2", NULL                    , "Filter_11" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "udp port 1"           , NULL                    , "Filter_12" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "udp portrange 1-2"    , NULL                    , "Filter_13" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "udp src port 1"       , NULL                    , "Filter_14" }, // 30
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "udp src portrange 1-2", NULL                    , "Filter_15" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "tcp dst port 1"       , NULL                    , "Filter_16" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "tcp dst portrange 1-2", NULL                    , "Filter_17" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "tcp port 1"           , NULL                    , "Filter_18" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "tcp portrange 1-2"    , NULL                    , "Filter_19" }, // 35
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "tcp src port 1"       , NULL                    , "Filter_20" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "tcp src portrange 1-2", NULL                    , "Filter_21" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "less 1"               , NULL                    , "Filter_22" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "greater 1"            , NULL                    , "Filter_23" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "ip proto 1"           , NULL                    , "Filter_24" }, // 40
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "ip6 proto 1"          , NULL                    , "Filter_25" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "proto 6"              , NULL                    , "Filter_26" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "tcp"                  , NULL                    , "Filter_26" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "proto 17"             , NULL                    , "Filter_27" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "udp"                  , NULL                    , "Filter_27" }, // 45
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "proto 1"              , NULL                    , "Filter_28" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "icmp"                 , NULL                    , "Filter_28" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "host localhost"       , NULL                    , "Filter_29" },
};

#define TEST_CASE_COUNT ( sizeof( TEST_CASES ) / sizeof( TEST_CASES[ 0 ] ) )

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_BEGIN(Filter_Base)
{
    WSADATA lData;

    int lRet = WSAStartup(MAKEWORD(2, 2), &lData);
    assert(0 == lRet);

    for (unsigned int i = 0; i < TEST_CASE_COUNT; i++)
    {
        printf("========== Test case %u ==========\n", i);

        const TestCase & lTC = TEST_CASES[i];

        Filter * lF = Filter::Create(lTC.mOutputType, lTC.mForwardType, lTC.mCode);

        KMS_TEST_ASSERT(TestCase_Verify(lTC, lF));
    }

    lRet = WSACleanup();
    assert(0 == lRet);
}
KMS_TEST_END
