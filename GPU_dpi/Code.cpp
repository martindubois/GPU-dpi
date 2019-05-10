
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/Code.h

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== GPU_dpi ============================================================
#include "Code.h"

// Static constants
/////////////////////////////////////////////////////////////////////////////

#define EOL "\n"

static const char CODE_NP_A[] =
    "OPEN_NET_FUNCTION_DECLARE( NoPacket_Allways )"                                        EOL
    "{"                                                                                    EOL
    "    OPEN_NET_FUNCTION_BEGIN"                                                          EOL
                                                                                           EOL
    "        lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL
                                                                                           EOL
    "    OPEN_NET_FUNCTION_END"                                                            EOL
    "}"                                                                                    EOL;

static const char CODE_AP_N_D[] =
    "OPEN_NET_FUNCTION_DECLARE( AllPackets_Never_Direct )"                                EOL
    "{"                                                                                   EOL
    "    OPEN_NET_FUNCTION_BEGIN"                                                         EOL
                                                                                          EOL
    "        lPacketInfo->mSendTo = ( 1 << OUTPUT_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL
                                                                                          EOL
    "    OPEN_NET_FUNCTION_END"                                                           EOL
    "}"                                                                                   EOL;

static const char CODE_AP_N_F[] =
    "OPEN_NET_FUNCTION_DECLARE( AllPackets_Never_File )"                                EOL
    "{"                                                                                 EOL
    "    OPEN_NET_FUNCTION_BEGIN"                                                       EOL
                                                                                        EOL
    "        lPacketInfo->mSendTo = OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;" EOL
                                                                                        EOL
    "        if ( 0 == OPEN_NET_PACKET_INDEX )"                                         EOL
    "        {"                                                                         EOL
    "            lEvents |= OPEN_NET_BUFFER_EVENT;"                                     EOL
    "        }"                                                                         EOL
                                                                                        EOL
    "    OPEN_NET_FUNCTION_END"                                                         EOL
    "}"                                                                                 EOL;

static const char CODE_AP_A_D[] =
    "OPEN_NET_FUNCTION_DECLARE( AllPackets_Always_Direct )"                                                          EOL
    "{"                                                                                                              EOL
    "    OPEN_NET_FUNCTION_BEGIN"                                                                                    EOL
                                                                                                                     EOL
    "        lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | ( 1 << OUTPUT_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL
                                                                                                                     EOL
    "    OPEN_NET_FUNCTION_END"                                                                                      EOL
    "}"                                                                                                              EOL;

static const char CODE_AP_A_F[] =
    "OPEN_NET_FUNCTION_DECLARE( AllPackets_Always_File )"                                                          EOL
    "{"                                                                                                            EOL
    "    OPEN_NET_FUNCTION_BEGIN"                                                                                  EOL
                                                                                                                   EOL
    "        lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;" EOL
                                                                                                                   EOL
    "        if ( 0 == OPEN_NET_PACKET_INDEX )"                                                                    EOL
    "        {"                                                                                                    EOL
    "            lEvents |= OPEN_NET_BUFFER_EVENT;"                                                                EOL
    "        }"                                                                                                    EOL
                                                                                                                   EOL
    "    OPEN_NET_FUNCTION_END"                                                                                    EOL
    "}"                                                                                                            EOL;

// Code table
/////////////////////////////////////////////////////////////////////////////

const Code_Info CODES[GPU_dpi::FILTER_TYPE_QTY][GPU_dpi::FORWARD_TYPE_QTY][GPU_dpi::OUTPUT_TYPE_QTY] =
{
    //    Code Type       Code         Code Size (byte)         Function Name               Outp.  F.Re.  O.Re.           FILTER_TYPE_         FORWARD_TYPE  OUTPUT_TYPE
    //    ==============  ===========  =======================  ==========================  =====  =====  =====           =================    ============  ===========
    { { { CODE_INVALID                                                                                          }    , // NO_PACKET            NEVER         NONE
        { CODE_INVALID                                                                                          }    , //                                    CALLBACK
        { CODE_INVALID                                                                                          }    , //                                    DIRECT
        { CODE_INVALID                                                                                          } }  , //                                    FILE
      { { CODE_CONSTANT , CODE_NP_A  , sizeof(CODE_NP_A  ) - 1, "NoPacket_Always"         , false, true , false }    , //                      ALWAYS        NONE
        { CODE_CONSTANT , CODE_NP_A  , sizeof(CODE_NP_A  ) - 1, "NoPacket_Always"         , false, true , false }    , //                                    CALLBACK
        { CODE_CONSTANT , CODE_NP_A  , sizeof(CODE_NP_A  ) - 1, "NoPacket_Always"         , false, true , false }    , //                                    DIRECT      
        { CODE_CONSTANT , CODE_NP_A  , sizeof(CODE_NP_A  ) - 1, "NoPacket_Always"         , false, true , false } }  , //                                    FILE
      { { CODE_INVALID                                                                                          }    , //                      FILTERED      NONE
        { CODE_INVALID                                                                                          }    , //                                    CALLBACK
        { CODE_INVALID                                                                                          }    , //                                    DIRECT
        { CODE_INVALID                                                                                          } }  , //                                    FILE
      { { CODE_CONSTANT , CODE_NP_A  , sizeof(CODE_NP_A  ) - 1, "NoPacket_Always"         , false, true , false }    , //                      NOT_FILTERED  NONE
        { CODE_CONSTANT , CODE_NP_A  , sizeof(CODE_NP_A  ) - 1, "NoPacket_Always"         , false, true , false }    , //                                    CALLBACK
        { CODE_CONSTANT , CODE_NP_A  , sizeof(CODE_NP_A  ) - 1, "NoPacket_Always"         , false, true , false }    , //                                    DIRECT
        { CODE_CONSTANT , CODE_NP_A  , sizeof(CODE_NP_A  ) - 1, "NoPacket_Always"         , false, true , false } } }, //                                    FILE
    { { { CODE_INVALID                                                                                          }    , // ALL_PACKETS          NEVER         NONE
        { CODE_CONSTANT , CODE_AP_N_F, sizeof(CODE_AP_N_F) - 1, "AllPackets_Never_File"   , true , false, false }    , //                                    CALLBACK
        { CODE_CONSTANT , CODE_AP_N_D, sizeof(CODE_AP_N_D) - 1, "AllPackets_Never_Direct" , true , false, true  }    , //                                    DIRECT
        { CODE_CONSTANT , CODE_AP_N_F, sizeof(CODE_AP_N_F) - 1, "AllPackets_Never_File"   , true , false, false } }  , //                                    FILE
      { { CODE_CONSTANT , CODE_NP_A  , sizeof(CODE_NP_A  ) - 1, "NoPacket_Always"         , false, true , false }    , //                      ALWAYS        NONE
        { CODE_CONSTANT , CODE_AP_A_F, sizeof(CODE_AP_A_F) - 1, "AllPackets_Always_File"  , true , true , true  }    , //                                    CALLBACK
        { CODE_CONSTANT , CODE_AP_A_D, sizeof(CODE_AP_A_D) - 1, "AllPackets_Always_Direct", true , true , true  }    , //                                    DIRECT
        { CODE_CONSTANT , CODE_AP_A_F, sizeof(CODE_AP_A_F) - 1, "AllPackets_Always_File"  , true , true , true  } }  , //                                    FILE
      { { CODE_CONSTANT , CODE_NP_A  , sizeof(CODE_NP_A  ) - 1, "NoPacket_Always"         , false, true , false }    , //                      FILTERED      NONE
        { CODE_CONSTANT , CODE_AP_A_F, sizeof(CODE_AP_A_F) - 1, "AllPackets_Always_File"  , true , true , false }    , //                                    CALLBACK
        { CODE_CONSTANT , CODE_AP_A_D, sizeof(CODE_AP_A_D) - 1, "AllPackets_Always_Direct", true , true , true  }    , //                                    DIRECT
        { CODE_CONSTANT , CODE_AP_A_F, sizeof(CODE_AP_A_F) - 1, "AllPackets_Always_File"  , true , true , false } }  , //                                    FILE
      { { CODE_INVALID                                                                                          }    , //                      NOT_FILTERED  NONE
        { CODE_CONSTANT , CODE_AP_N_F, sizeof(CODE_AP_N_F) - 1, "AllPackets_Never_File"   , true , false, false }    , //                                    CALLBACK
        { CODE_CONSTANT , CODE_AP_N_D, sizeof(CODE_AP_N_D) - 1, "AllPackets_Never_Direct" , true , false, true  }    , //                                    DIRECT
        { CODE_CONSTANT , CODE_AP_N_F, sizeof(CODE_AP_N_F) - 1, "AllPackets_Never_File"   , true , false, false } } }, //                                    FILE
    { { { CODE_INVALID                                                                                          }    , // FILTER               NEVER         NONE
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , false, false }    , //                                    CALLBACK
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , false, true  }    , //                                    DIRECT
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , false, false } }  , //                                    FILE
      { { CODE_GENERATED, NULL       ,                       0, NULL                      , false, true , false }    , //                      ALWAYS        NONE
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , true , false }    , //                                    CALLBACK
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , true , true  }    , //                                    DIRECT
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , true , false } }  , //                                    FILE
      { { CODE_GENERATED, NULL       ,                       0, NULL                      , false, true , false }    , //                      FILTERED      NONE
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , true , false }    , //                                    CALLBACK
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , true , true  }    , //                                    DIRECT
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , true , false } }  , //                                    FILE
      { { CODE_GENERATED, NULL       ,                       0, NULL                      , false, true , false }    , //                      NOT_FILTERED  NONE
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , true , false }    , //                                    CALLBACK
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , true , true  }    , //                                    DIRECT
        { CODE_GENERATED, NULL       ,                       0, NULL                      , true , true , false } } }, //                                    FILE
    { { { CODE_INVALID                                                                                          }    , // OPEN_NET_FUNCTION    NEVER         NONE
        { CODE_USER     , NULL       ,                       0, NULL                      , true , false, false }    , //                                    CALLBACK
        { CODE_USER     , NULL       ,                       0, NULL                      , true , false, true  }    , //                                    DIRECT
        { CODE_USER     , NULL       ,                       0, NULL                      , true , false, false } }  , //                                    FILE
      { { CODE_USER     , NULL       ,                       0, NULL                      , false, true , false }    , //                      ALWAYS        NONE
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false }    , //                                    CALLBACK
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , true  }    , //                                    DIRECT
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false } }  , //                                    FILE
      { { CODE_USER     , NULL       ,                       0, NULL                      , false, true , false }    , //                      FILTERED      NONE
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false }    , //                                    CALLBACK
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , true  }    , //                                    DIRECT
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false } }  , //                                    FILE
      { { CODE_USER     , NULL       ,                       0, NULL                      , false, true , false }    , //                      NOT_FILTERED  NONE
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false }    , //                                    CALLBACK
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , true  }    , //                                    DIRECT
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false } } }, //                                    FILE
    { { { CODE_INVALID                                                                                          }    , // OPEN_NET_KERNEL      NEVER         NONE
        { CODE_USER     , NULL       ,                       0, NULL                      , true , false, false }    , //                                    CALLBACK
        { CODE_USER     , NULL       ,                       0, NULL                      , true , false, true  }    , //                                    DIRECT
        { CODE_USER     , NULL       ,                       0, NULL                      , true , false, false } }  , //                                    FILE
      { { CODE_USER     , NULL       ,                       0, NULL                      , false, true , false }    , //                      ALWAYS        NONE
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false }    , //                                    CALLBACK
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , true  }    , //                                    DIRECT
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false } }  , //                                    FILE
      { { CODE_USER     , NULL       ,                       0, NULL                      , false, true , false }    , //                      FILTERED      NONE
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false }    , //                                    CALLBACK
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , true  }    , //                                    DIRECT
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false } }  , //                                    FILE
      { { CODE_USER     , NULL       ,                       0, NULL                      , false, true , false }    , //                      NOT_FILTERED  NONE
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false }    , //                                    CALLBACK
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , true  }    , //                                    DIRECT
        { CODE_USER     , NULL       ,                       0, NULL                      , true , true , false } } }, //                                    FILE
};
