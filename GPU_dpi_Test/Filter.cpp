
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
    { "portrange 1-2"                                        , { 65, 0 } }, // 0
    { "ether[11] = 5"                                        , { 65, 0 } }, // 1
    { "host 1.2.3.4"                                         , { 65, 0 } }, // 2
    { "ip host 1.2.3.4"                                      , { 65, 0 } }, // 3
    { "ether proto 0x0800 and host 1.2.3.4"                  , { 65, 0 } }, // 4
 // { "ether host 00:01:02:03:04:05 and not host 192.168.0.1", { 65, 0 } }, ??? (0)
 // { "tcp src port 0x0a0a"                                  , { 0 } },     TODO
 // { "greater 65"                                           , { 65, 0 } }, Later
    { "len <= 65"                                            , { 65, 0 } }, // 5
    { "port 1"                                               , { 65, 0 } }, // 6
    { "ether[6:4] = 0x00010203"                              , { 65, 0 } }, // 7
 // { "in_ether { BIN \"10 11 00 00 01 02 03 04 01 02 03\" }", { 65, 0 } }, ???
 /* { "arp dst host 192.158.0.1"                             , { 0 } },
    { "arp host localhost"                                   , { 0 } },
    { "arp src host localhost"                               , { 0 } }, */
    { "dst net 1.2.3"                                        , { 65, 0 } }, // 8
    { "dst port 1"                                           , { 65, 0 } }, // 9
    { "dst portrange 1-2"                                    , { 65, 0 } }, // 10
 // { "ether dst ff-ff-ff-ff-ff-ff"                          , { 65, 1024, 0 } }, Impossible
 // { "ether src 00.01.02.03.04.05"                          , { 65, 0 } }, ??? (0)
    { "ip dst host 01.02.03.04"                              , { 65, 0 } }, // 11
 // { "ip multicast"                                         , { 0 } }      Later
    { "ip proto 17"                                          , { 65, 0 } }, // 12
    { "ip src host 1.2.3.4"                                  , { 65, 0 } }, // 13
 // { "ip6 dst host 1:2:3:4:5:6:7:8"                         , { 0 } },     Later
 // { "ip6 host localhost"                                   , { 0 } },     Later
 // { "ip6 multicast"                                        , { 0 } },     Later
 // { "ip6 proto 0x10"                                       , { 0 } },     Later
 // { "ip6 src host 1:2:3:4:5:6:7:8"                         , { 0 } },     Later
 // { "proto ah"                                             , { 0 } },     Later
 // { "proto esp"                                            , { 0 } },     Later
 // { "proto icmp6"                                          , { 0 } },     Later
 // { "proto igmp"                                           , { 0 } },     Later
 // { "proto pim"                                            , { 0 } },     Later
 // { "proto vrrp"                                           , { 0 } },     Later
    { "src net 1"                                            , { 65, 0 } }, // 14
    { "src portrange 0-1"                                    , { 65, 0 } }, // 15
 // { "tcp dst port 1"                                       , { 0 } },     Later
 // { "tcp dst portrange 1-2"                                , { 0 } },     Later
 // { "tcp port 1"                                           , { 0 } },     Later
 // { "tcp portrange 1-2"                                    , { 0 } },     Later
 // { "tcp src portrange 1-2"                                , { 0 } },     Later
    { "udp dst port 1"                                       , { 65, 0 } }, // 16
    { "udp dst portrange 1-2"                                , { 65, 0 } }, // 17
    { "udp port 1"                                           , { 65, 0 } }, // 18
    { "udp portrange 1-2"                                    , { 65, 0 } }, // 19
    { "udp src port 0"                                       , { 65, 0 } }, // 20
    { "udp src portrange 0-1"                                , { 65, 0 } }, // 21
 // { "ether broadcast"                                      , { 65, 1024, 0 } }, Impossible
 // { "vlan 100"                                             , { 0 } },     Later
 // { "ether[len - 2] != 0"                                  , { 0 } },     Later
 // { "ether[len - 2 : 2] != 0"                              , { 0 } },     Later
    { "udp"                                                  , { 65, 0 } }, // 22
    { "net 1.2.3.4"                                          , { 65, 0 } }, // 23
    { "net 1.2"                                              , { 65, 0 } }, // 24
 // { "ip6 net 01:02:03:04:05:06:07:08"                      , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/127"                  , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/112"                  , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/110"                  , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/96"                   , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/93"                   , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/80"                   , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/76"                   , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/64"                   , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/59"                   , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/48"                   , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/42"                   , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/32"                   , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/25"                   , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/16"                   , { 0 } },     Later
 // { "ip6 net 01:02:03:04:05:06:07:08/8"                    , { 0 } },     Later
    { "dst host 1.2.3.4"                                     , { 65, 0 } }, // 25
 // { "multicast"                                            , { 0 } },     Later
    { "port 1 or port 3"                                     , { 65, 0 } }, // 26
    { "src host 1.2.3.4"                                     , { 65, 0 } }, // 27
    { "src port 0"                                           , { 65, 0 } }, // 28
 // { "vlan"                                                 , { 0 } },     Later
    { "ip net 1.2.3.0/28"                                    , { 65, 0 } }, // 29
    { "ip net 1.2.0/22"                                      , { 65, 0 } }, // 30
    { "ip net 1.2/15"                                        , { 65, 0 } }, // 31
    { "ip net 0/7"                                           , { 65, 0 } }, // 32
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

        lConfig.mBufferQty              = 56;
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
