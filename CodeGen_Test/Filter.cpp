
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
    { GPU_dpi::OUTPUT_TYPE_FILE  , GPU_dpi::FORWARD_TYPE_NEVER       , "portrange 1-2"                                        , NULL, "Filter_00" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_FILTERED    , "ether[0] & 1 != 0"                                    , NULL, "Filter_01" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NEVER       , "host localhost"                                       , NULL, "Filter_02" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip host localhost"                                    , NULL, "Filter_03" },
    { GPU_dpi::OUTPUT_TYPE_FILE  , GPU_dpi::FORWARD_TYPE_ALWAYS      , "ether proto 0x0800 and host www.kms-quebec.com"       , NULL, "Filter_04" },
    { GPU_dpi::OUTPUT_TYPE_FILE  , GPU_dpi::FORWARD_TYPE_FILTERED    , "ether host 01:02:03:04:05:06 and not host 192.168.0.1", NULL, "Filter_05" },
    { GPU_dpi::OUTPUT_TYPE_FILE  , GPU_dpi::FORWARD_TYPE_NEVER       , "tcp src port 0x0a0a"                                  , NULL, "Filter_06" },
    { GPU_dpi::OUTPUT_TYPE_FILE  , GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "greater 1000"                                         , NULL, "Filter_07" },
    { GPU_dpi::OUTPUT_TYPE_NONE  , GPU_dpi::FORWARD_TYPE_ALWAYS      , "len <= 1000"                                          , NULL, "Filter_08" },
    { GPU_dpi::OUTPUT_TYPE_NONE  , GPU_dpi::FORWARD_TYPE_FILTERED    , "port 1"                                               , NULL, "Filter_09" },
    { GPU_dpi::OUTPUT_TYPE_NONE  , GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "less 1000"                                            , NULL, "Filter_10" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_FILTERED    , "ether[0:4] != 0x00000000"                             , NULL, "Filter_11" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NEVER       , "in_ether { TEXT \"Totot\" }"                          , NULL, "Filter_12" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "arp dst host 192.158.0.1"                             , NULL, "Filter_13" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "arp host localhost"                                   , NULL, "Filter_14" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "arp src host localhost"                               , NULL, "Filter_15" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "dst net 192.158.0"                                    , NULL, "Filter_16" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "dst port 1"                                           , NULL, "Filter_17" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "dst portrange 1-2"                                    , NULL, "Filter_18" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ether dst 01-02-03-04-05-06"                          , NULL, "Filter_19" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ether src 01.02.03.04.05.0f"                          , NULL, "Filter_20" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip dst host 01.02.03.04"                              , NULL, "Filter_21" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip multicast"                                         , NULL, "Filter_22" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip proto 110"                                         , NULL, "Filter_23" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip src host 1.2.3.4"                                  , NULL, "Filter_24" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 dst host 1:2:3:4:5:6:7:8"                         , NULL, "Filter_25" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 host localhost"                                   , NULL, "Filter_26" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 multicast"                                        , NULL, "Filter_27" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 proto 0x10"                                       , NULL, "Filter_28" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 src host 1:2:3:4:5:6:7:8"                         , NULL, "Filter_29" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "proto ah"                                             , NULL, "Filter_30" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "proto esp"                                            , NULL, "Filter_31" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "proto icmp6"                                          , NULL, "Filter_32" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "proto igmp"                                           , NULL, "Filter_33" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "proto pim"                                            , NULL, "Filter_34" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "proto vrrp"                                           , NULL, "Filter_35" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "src net 10"                                           , NULL, "Filter_36" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "src portrange 1-2"                                    , NULL, "Filter_37" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "tcp dst port 1"                                       , NULL, "Filter_38" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "tcp dst portrange 1-2"                                , NULL, "Filter_39" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "tcp port 1"                                           , NULL, "Filter_40" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "tcp portrange 1-2"                                    , NULL, "Filter_41" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "tcp src portrange 1-2"                                , NULL, "Filter_42" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "udp dst port 1"                                       , NULL, "Filter_43" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "udp dst portrange 1-2"                                , NULL, "Filter_44" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "udp port 1"                                           , NULL, "Filter_45" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "udp portrange 1-2"                                    , NULL, "Filter_46" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "udp src port 1"                                       , NULL, "Filter_47" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "udp src portrange 1-2"                                , NULL, "Filter_48" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ether broadcast"                                      , NULL, "Filter_49" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "vlan 100"                                             , NULL, "Filter_50" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ether[len - 2] != 0"                                  , NULL, "Filter_51" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ether[len - 2 : 2] != 0"                              , NULL, "Filter_52" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "udp"                                                  , NULL, "Filter_53" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "net 192.168.0.1"                                      , NULL, "Filter_54" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "net 192.168"                                          , NULL, "Filter_55" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08"                      , NULL, "Filter_56" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/127"                  , NULL, "Filter_57" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/112"                  , NULL, "Filter_58" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/110"                  , NULL, "Filter_59" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/96"                   , NULL, "Filter_60" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/93"                   , NULL, "Filter_61" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/80"                   , NULL, "Filter_62" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/76"                   , NULL, "Filter_63" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/64"                   , NULL, "Filter_64" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/59"                   , NULL, "Filter_65" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/48"                   , NULL, "Filter_66" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/42"                   , NULL, "Filter_67" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/32"                   , NULL, "Filter_68" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/25"                   , NULL, "Filter_69" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/16"                   , NULL, "Filter_70" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip6 net 01:02:03:04:05:06:07:08/8"                    , NULL, "Filter_71" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "dst host localhost"                                   , NULL, "Filter_72" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "inbound"                                              , NULL, "Filter_73" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "multicast"                                            , NULL, "Filter_74" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "port 1 or port 3"                                     , NULL, "Filter_75" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "src host localhost"                                   , NULL, "Filter_76" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "src port 1"                                           , NULL, "Filter_77" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "vlan"                                                 , NULL, "Filter_78" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip net 192.168.0.0/28"                                , NULL, "Filter_79" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip net 192.168.0/22"                                  , NULL, "Filter_80" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip net 192.168/15"                                    , NULL, "Filter_81" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "ip net 192/7"                                         , NULL, "Filter_82" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "in_ip { TEXT \"Totot\"; BIN \"01 02 03\" 1 }"         , NULL, "Filter_83" },
    { GPU_dpi::OUTPUT_TYPE_FILE  , GPU_dpi::FORWARD_TYPE_NEVER       , "in_tcp { REGEX \"\\d{3}-\\d{3}-\\d{4}\" }"            , NULL, "Filter_84" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "in_udp { REGEX \"\\d{3,}\\D\\t\" }"                   , NULL, "Filter_85" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "in_udp { REGEX \"\\d{3,4}\\S\\$\" }"                  , NULL, "Filter_86" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "in_udp { REGEX \"(ab)+\\W.\" }"                       , NULL, "Filter_87" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "in_udp { REGEX \"(ab(cd)?ab)+\\n\" }"                 , NULL, "Filter_88" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "in_udp { REGEX \"a|bc\\r\" }"                         , NULL, "Filter_89" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "in_udp { REGEX \"[^a-d]*a\\s\" }"                     , NULL, "Filter_90" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "in_udp { REGEX \"^a-d\\w$\" }"                        , NULL, "Filter_91" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "in_ip { TEXT \"Totot\"; BIN \"a 0A 0\" 1 2 }"         , NULL, "Filter_92" },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_NOT_FILTERED, "in_udp { REGEX \"[a-d]*e\" }"                         , NULL, "Filter_93" },
    { GPU_dpi::OUTPUT_TYPE_FILE  , GPU_dpi::FORWARD_TYPE_NEVER       , "in_ether { BIN \"10 11 00 00 01 02 03 04 01 02 03\" }", NULL, "Filter_94" },

    { GPU_dpi::OUTPUT_TYPE_FILE  , GPU_dpi::FORWARD_TYPE_NEVER, "( portrange 1-2 )", NULL, "Filter_00" },

    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, ""                               , "ERROR 1 - Empty filter"                    , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, " "                              , "ERROR 1 - Empty filter"                    , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "a{a}"                           , "ERROR 2 - Invalid filter"                  , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "in_ether { TEXT"                , "ERROR 0.1 - Invalid list"                  , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "in_ether { TEXT"                , "ERROR 0.1 - Invalid list"                  , NULL },
    // { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "arp dst host 1"                 , "ERROR 1.4 - Invalid dst host expression"   , NULL }, // Take long time
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "ether host 1-"                  , "ERROR 1.8 - Invalid ether host expression" , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "ether host 01.02.03.04.05.0g"   , "ERROR 1.8 - Invalid ether host expression" , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "ether host 01.02.03.04.05.06.07", "ERROR 1.8 - Invalid ether host expression" , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "ether proto a"                  , "ERROR 1.9 - Invalid ether proto expression", NULL },
    // { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "host 0:1:2:3:4:5:6:10000"       , "ERROR 1.12 - Invalid host expression"      , NULL }, // Take long time
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "ip proto a"                     , "ERROR 1.16 - Invalid ip proto expression"  , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "less a"                         , "ERROR 1.23 - Invalid less expression"      , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "net a"                          , "ERROR 1.24 - Invalid net expression"       , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "port a"                         , "ERROR 1.25 - Invalid port number"          , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "port 0x10000"                   , "ERROR 1.25 - Invalid port number"          , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "portrange 1-"                   , "ERROR 1.26 - Invalid port range expression", NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "portrange 0xffff-0x10000"       , "ERROR 1.26 - Invalid port range expression", NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "vlan a"                         , "ERROR 1.37 - Invalid vlan expression"      , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "ether[a] = 0"                   , "ERROR 2.1 - Invalid brackets"              , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "ether[a:2] = 0"                 , "ERROR 2.2 - Invalid access expression"     , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "ether[0:a] = 0"                 , "ERROR 2.2 - Invalid access expression"     , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "ether[len-2:a] != 0"            , "ERROR 2.2 - Invalid access expression"     , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "a"                              , "ERROR 3.2 - Invalid filter"                , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "in_ether { }"                   , "ERROR S.1 - Invalid search element"        , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "in_ip { BIN \"g\" }"            , "ERROR S.2 - Invalid binary search string"  , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "in_ether { REGEX \"a{a}\" }"    , "ERROR S.3 - Invalid regular expression"    , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "in_ether { REGEX \"a]\" }"      , "ERROR S.3 - Invalid regular expression"    , NULL },
    { GPU_dpi::OUTPUT_TYPE_DIRECT, GPU_dpi::FORWARD_TYPE_ALWAYS, "in_ether { RTX \"Invalid\" }"   , "ERROR S.4 - Invalid search element type"   , NULL },
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
