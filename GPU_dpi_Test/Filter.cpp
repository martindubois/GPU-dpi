
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) KMS 2019. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi_Test/Filter.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== Includes/OpenNet ===================================================
#include <GPU_dpi/System.h>

// ===== GPU_dpi_Test =======================================================
#include "TestCase.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

static const TestCase::Info TESTS[]
{
    { "arp dst host 192.168.0.2"                             , { 67,             0 } }, //  0
    { "arp host 192.168.0.1"                                 , { 67,             0 } },
    { "arp src host 192.168.0.1"                             , { 67,             0 } },
    { "dst host 1.2.3.4"                                     , { 65, 66,         0 } },
    { "dst net 1.2.3"                                        , { 65, 66,         0 } },
    { "dst port 1"                                           , { 65,             0 } }, //  5  Filter_17
    { "dst portrange 1-2"                                    , { 65,             0 } }, //  6  Filter_18
    { "ether host 00:01:02:03:04:05 and not host 192.168.0.1", { 65, 66,         0 } }, //  7  Filter_05 <== 68 received but not expected
    { "ether proto 0x0800 and host 1.2.3.4"                  , { 65, 66,         0 } },
    { "ether src 00.01.02.03.04.05"                          , { 65, 66, 67, 68, 0 } },
    { "ether[6:4] = 0x00010203"                              , { 65, 66, 67, 68, 0 } }, // 10
    { "ether[11] = 5"                                        , { 65, 66, 67, 68, 0 } },
    { "greater 1024"                                         , { 1024,           0 } },
    { "host 1.2.3.4"                                         , { 65, 66,         0 } },
    { "in_ether { BIN \"10 11 00 00 01 02 03 04 01 02 03\" }", { 65,             0 } }, // 14  Filter_94
    { "in_ip { TEXT \"KMS\" }"                               , { 65,             0 } },
    { "in_tcp { REGEX \"\\d{3}-\\d{3}-\\d{4}\" }"            , { 66,             0 } }, // 16  Filter_84
    { "ip dst host 01.02.03.04"                              , { 65, 66,         0 } },
    { "ip host 1.2.3.4"                                      , { 65, 66,         0 } },
    { "ip multicast"                                         , { 69,             0 } },
    { "ip net 0/7"                                           , { 65, 66,         0 } }, // 20
    { "ip net 1.2/15"                                        , { 65, 66,         0 } },
    { "ip net 1.2.0/22"                                      , { 65, 66,         0 } },
    { "ip net 1.2.3.0/28"                                    , { 65, 66,         0 } },
    { "ip proto 17"                                          , { 65, 68, 69,     0 } },
    { "ip src host 1.2.3.4"                                  , { 65, 66,         0 } }, // 25
    { "ip6 dst host f0e:d0c:b0a:908:706:504:302:100"         , { 70,             0 } },
    { "ip6 host 1:203:405:607:809:a0b:c0d:e0f"               , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f"                , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/16"             , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/110"            , { 70,             0 } }, // 30
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/112"            , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/127"            , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/25"             , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/32"             , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/42"             , { 70,             0 } }, // 35
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/48"             , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/59"             , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/64"             , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/76"             , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/8"              , { 70,             0 } }, // 40
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/80"             , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/93"             , { 70,             0 } },
    { "ip6 net 1:203:405:607:809:a0b:c0d:e0f/96"             , { 70,             0 } },
    { "ip6 proto 0x11"                                       , { 70,             0 } },
    { "ip6 src host 1:203:405:607:809:a0b:c0d:e0f"           , { 70,             0 } }, // 45
    { "len <= 65"                                            , { 65,             0 } },
    { "multicast"                                            , { 69,             0 } },
    { "net 1.2"                                              , { 65, 66,         0 } },
    { "net 1.2.3.4"                                          , { 65, 66,         0 } },
    { "port 1"                                               , { 65,             0 } }, // 50  Filter_09
    { "port 1 or port 3"                                     , { 65, 66,         0 } },
    { "portrange 1-2"                                        , { 65, 66,         0 } }, // 52  Filter_00
    { "src host 1.2.3.4"                                     , { 65, 66,         0 } },
    { "src net 1"                                            , { 65, 66,         0 } },
    { "src port 0"                                           , { 65,             0 } }, // 55
    { "src portrange 0-1"                                    , { 65,             0 } },
    { "tcp dst port 3"                                       , { 66,             0 } },
    { "tcp dst portrange 3-4"                                , { 66,             0 } },
    { "tcp port 2"                                           , { 66,             0 } },
    { "tcp portrange 2-3"                                    , { 66,             0 } }, // 60
    { "tcp src port 0x0002"                                  , { 66,             0 } },
    { "tcp src portrange 1-2"                                , { 66,             0 } },
    { "udp"                                                  , { 65, 68, 69, 70, 0 } }, // 63 Filter_53
    { "udp dst port 1"                                       , { 65,             0 } },
    { "udp dst portrange 1-2"                                , { 65,             0 } }, // 65
    { "udp port 1"                                           , { 65,             0 } },
    { "udp portrange 1-2"                                    , { 65,             0 } },
    { "udp src port 0"                                       , { 65,             0 } },
    { "udp src portrange 0-1"                                , { 65,             0 } },
    { "vlan"                                                 , { 68,             0 } },
    { "vlan 2"                                               , { 68,             0 } }, // 71

 // { "ip6 multicast"                                        , {         0 } }, |
 // { "proto ah"                                             , {         0 } }, TODO  ah packet
 // { "proto esp"                                            , {         0 } }, TODO  esp packet
 // { "proto icmp6"                                          , {         0 } }, TODO  icmp6 packet
 // { "proto igmp"                                           , {         0 } }, TODO  igmp packet
 // { "proto pim"                                            , {         0 } }, TODO  pim packet
 // { "proto vrrp"                                           , {         0 } }, TODO  vrrp packet
 // { "ether[len - 2] != 0"                                  , {         0 } }, Later
 // { "ether[len - 2 : 2] != 0"                              , {         0 } }, Later
};

#define TEST_COUNT  ( sizeof( TESTS ) / sizeof( TESTS[ 0 ] ) )

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_BEGIN(Filter_SetupB)
{
    for (unsigned int i = 0; i < TEST_COUNT; i++)
    {
        printf("========== %i ==========\n", i);

        GPU_dpi::System * lS0 = GPU_dpi::System::Create();
        KMS_TEST_ASSERT_RETURN(NULL != lS0);

        GPU_dpi::AdapterConfig lConfig  ;
        TestCase               lTestCase(TESTS[i]);

        KMS_TEST_COMPARE(GPU_dpi::STATUS_OK, lS0->Adapter_GetConfig(0, &lConfig));

        lConfig.mBufferQty              = 14;
        lConfig.mFilterCode             = TESTS[i].mFilter;
        lConfig.mFilterCodeSize_byte    = static_cast<unsigned int>(strlen(lConfig.mFilterCode));
        lConfig.mFilterType             = GPU_dpi::FILTER_TYPE_FILTER;
        lConfig.mFlags.mProfilingEnabled = true;
        lConfig.mForwardType            = GPU_dpi::FORWARD_TYPE_NEVER;
        lConfig.mOutputCallback         = TestCase::Callback;
        lConfig.mOutputCallback_Context = &lTestCase;
        lConfig.mOutputType             = GPU_dpi::OUTPUT_TYPE_CALLBACK;
        lConfig.mProcessorIndex         = 0;

        KMS_TEST_COMPARE(GPU_dpi::STATUS_OK, lS0->Adapter_SetConfig(0, lConfig));

        GPU_dpi::Status lStatus = lS0->Start();
        switch (lStatus)
        {
        case GPU_dpi::STATUS_INVALID_FILTER:
        case GPU_dpi::STATUS_OPEN_CL_ERROR :
            FILE * lFile;
            if (0 == fopen_s(&lFile, "C:\\Temp\\GPU_dpi_Test_BuildLog.txt", "w"))
            {
                GPU_dpi::Status lStatus = lS0->WriteBuildLog(lFile);
                assert(GPU_dpi::STATUS_OK == lStatus);

                int lRet = fclose(lFile);
                assert(0 == lRet);
            }
            break;
        }

        KMS_TEST_COMPARE_RETURN(GPU_dpi::STATUS_OK, lStatus);

        KMS_TEST_ASSERT(lTestCase.Wait());

        KMS_TEST_COMPARE(GPU_dpi::STATUS_OK, lS0->Stop());

        KMS_TEST_ASSERT(lTestCase.Verify());

        lTestCase.DisplayStatistics();

        KMS_TEST_COMPARE(GPU_dpi::STATUS_OK, lS0->Adapter_DisplayStatistics(0, stdout));
        KMS_TEST_COMPARE(GPU_dpi::STATUS_OK, lS0->DisplayStatistics        (   stdout));

        lS0->Delete();
    }
}
KMS_TEST_END
