
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Phase1.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== CodeGen ============================================================
#include "Token.h"

#include "Phase1.h"

// Data types
/////////////////////////////////////////////////////////////////////////////

typedef Token * (*Seq2Function)(Token *, Token *);

typedef struct
{
    TokenId      mIds      [2];
    Seq2Function mFunction    ;
    const char * mErrorMessage;
}
Seq;

// Static function declaration
/////////////////////////////////////////////////////////////////////////////

// ===== Seq2 ===============================================================
static Token * Dst_Port            (Token * aDst      , Token * aPort        );
static Token * Dst_PortRange       (Token * aDst      , Token * aPortRange   );
static Token * Ip_Proto            (Token * aIp       , Token * aProto       );
static Token * Ip6_Proto           (Token * aIp6      , Token * aProto       );
static Token * Not_Boolean         (Token * aNot      , Token * aBoolean     );
static Token * PortRange_Port_Range(Token * aPortRange, Token * aRange       );
static Token * Src_Port            (Token * aSrc      , Token * aPort        );
static Token * Src_PortRange       (Token * aSrc      , Token * aPortRange   );
static Token * Udp_DstPort         (Token * aUdp      , Token * aDstPort     );
static Token * Udp_DstPortRange    (Token * aUdp      , Token * aDstPortRange);
static Token * Udp_Port            (Token * aUdp      , Token * aPort        );
static Token * Udp_PortRange       (Token * aUdp      , Token * aPortRange   );
static Token * Udp_SrcPort         (Token * aUdp      , Token * aSrcPort     );
static Token * Udp_SrcPortRange    (Token * aUdp      , Token * aSrcPortRange);
static Token * Tcp_DstPort         (Token * aTcp      , Token * aDstPort     );
static Token * Tcp_DstPortRange    (Token * aTcp      , Token * aDstPortRange);
static Token * Tcp_Port            (Token * aTcp      , Token * aPort        );
static Token * Tcp_PortRange       (Token * aTcp      , Token * aPortRange   );
static Token * Tcp_SrcPort         (Token * aTcp      , Token * aSrcPort     );
static Token * Tcp_SrcPortRange    (Token * aTcp      , Token * aSrcPortRange);

static Token * Token_DataUInt16(Token * aToken, Token * aData);
static Token * Token_DataUInt8 (Token * aToken, Token * aData);
static Token * Token_IpAddress (Token * aToken, Token * aData);

// Constants
/////////////////////////////////////////////////////////////////////////////

static const Seq SEQ[]
{
    { { TOKEN_DST              , TOKEN_PORT          }, Dst_Port            , "ERROR 1.1 - Invalid dst port expression"            },
    { { TOKEN_DST              , TOKEN_PORTRANGE     }, Dst_PortRange       , "ERROR 1.2 - Invalid dst portrange expression"       },
    { { TOKEN_DST_PORT         , TOKEN_DATA          }, Token_DataUInt16    , "ERROR 1.3 - Invalid dst port number"                },
    { { TOKEN_DST_PORTRANGE    , TOKEN_PORT_RANGE    }, PortRange_Port_Range, "ERROR 1.4 - Invalid dst portrange expression"       },
    { { TOKEN_GREATER          , TOKEN_DATA          }, Token_DataUInt16    , "ERROR 1.5 - Invalid greater expression"             },
    { { TOKEN_HOST             , TOKEN_DATA          }, Token_IpAddress     , "ERROR 1.6 - Invalid host expression"                },
    { { TOKEN_IP               , TOKEN_PROTO         }, Ip_Proto            , "ERROR 1.7 - Invalid ip proto expression"            },
    { { TOKEN_IP_PROTO         , TOKEN_DATA          }, Token_DataUInt8     , "ERROR 1.8 - Invalid ip proto expression"            },
    { { TOKEN_IP6              , TOKEN_PROTO         }, Ip6_Proto           , "ERROR 1.9 - Invalid ip6 proto expression"           },
    { { TOKEN_IP6_PROTO        , TOKEN_DATA          }, Token_DataUInt8     , "ERROR 1.10 - Invalid ip6 proto expression"           },
    { { TOKEN_LESS             , TOKEN_DATA          }, Token_DataUInt16    , "ERROR 1.11 - Invalid less expression"                },
    { { TOKEN_NOT              , TOKEN_BOOLEAN       }, Not_Boolean         , "ERROR 1.12 - Invalid not expression"                 },
    { { TOKEN_PORT             , TOKEN_DATA          }, Token_DataUInt16    , "ERROR 1.13 - Invalid port number"                    },
    { { TOKEN_PORTRANGE        , TOKEN_PORT_RANGE    }, PortRange_Port_Range, "ERROR 1.14 - Invalid port range expression"         },
    { { TOKEN_PROTO            , TOKEN_DATA          }, Token_DataUInt8     , "ERROR 1.15 - Invalid proto expression"              },
    { { TOKEN_SRC              , TOKEN_PORT          }, Src_Port            , "ERROR 1.16 - Invalid src port expression"           },
    { { TOKEN_SRC              , TOKEN_PORTRANGE     }, Src_PortRange       , "ERROR 1.17 - Invalid src portrange expression"      },
    { { TOKEN_SRC_PORT         , TOKEN_DATA          }, Token_DataUInt16    , "ERROR 1.18 - Invalid src port number"               },
    { { TOKEN_SRC_PORTRANGE    , TOKEN_PORT_RANGE    }, PortRange_Port_Range, "ERROR 1.19 - Invalid src portrange expression"      },
    { { TOKEN_UDP              , TOKEN_DST_PORT      }, Udp_DstPort         , "ERROR 1.20 - Invalid udp dst port expression"       },
    { { TOKEN_UDP              , TOKEN_DST_PORTRANGE }, Udp_DstPortRange    , "ERROR 1.21 - Invalid udp dst portrange expression"  },
    { { TOKEN_UDP              , TOKEN_PORT          }, Udp_Port            , "ERROR 1.22 - Invalid udp port expression"           },
    { { TOKEN_UDP              , TOKEN_PORTRANGE     }, Udp_PortRange       , "ERROR 1.23 - Invalid udp portrange expression"      },
    { { TOKEN_UDP              , TOKEN_SRC_PORT      }, Udp_SrcPort         , "ERROR 1.24 - Invalid udp src port expression"       },
    { { TOKEN_UDP              , TOKEN_SRC_PORTRANGE }, Udp_SrcPortRange    , "ERROR 1.25 - Invalid udp src portrange expression"  },
    { { TOKEN_UDP_DST_PORT     , TOKEN_DATA          }, Token_DataUInt16    , "ERROR 1.26 - Invalid udp dst port expression"       },
    { { TOKEN_UDP_DST_PORTRANGE, TOKEN_PORT_RANGE    }, PortRange_Port_Range, "ERROR 1.27 - Invalid udp dst portranage expression" },
    { { TOKEN_UDP_PORT         , TOKEN_DATA          }, Token_DataUInt16    , "ERROR 1.28 - Invalid udp port expression"           },
    { { TOKEN_UDP_PORTRANGE    , TOKEN_PORT_RANGE    }, PortRange_Port_Range, "ERROR 1.29 - Invalid udp portrange expression"      },
    { { TOKEN_UDP_SRC_PORT     , TOKEN_DATA          }, Token_DataUInt16    , "ERROR 1.30 - Invalid udp src port expression"       },
    { { TOKEN_UDP_SRC_PORTRANGE, TOKEN_PORT_RANGE    }, PortRange_Port_Range, "ERROR 1.31 - Invalid udp src portranage expression" },
    { { TOKEN_TCP              , TOKEN_DST_PORT      }, Tcp_DstPort         , "ERROR 1.32 - Invalid udp dst port expression"       },
    { { TOKEN_TCP              , TOKEN_DST_PORTRANGE }, Tcp_DstPortRange    , "ERROR 1.33 - Invalid udp dst portrange expression"  },
    { { TOKEN_TCP              , TOKEN_PORT          }, Tcp_Port            , "ERROR 1.34 - Invalid udp port expression"           },
    { { TOKEN_TCP              , TOKEN_PORTRANGE     }, Tcp_PortRange       , "ERROR 1.35 - Invalid udp portrange expression"      },
    { { TOKEN_TCP              , TOKEN_SRC_PORT      }, Tcp_SrcPort         , "ERROR 1.36 - Invalid udp src port expression"       },
    { { TOKEN_TCP              , TOKEN_SRC_PORTRANGE }, Tcp_SrcPortRange    , "ERROR 1.37 - Invalid udp src portrange expression"  },
    { { TOKEN_TCP_DST_PORT     , TOKEN_DATA          }, Token_DataUInt16    , "ERROR 1.38 - Invalid tcp dst port expression"       },
    { { TOKEN_TCP_DST_PORTRANGE, TOKEN_PORT_RANGE    }, PortRange_Port_Range, "ERROR 1.39 - Invalid tcp dst portranage expression" },
    { { TOKEN_TCP_PORT         , TOKEN_DATA          }, Token_DataUInt16    , "ERROR 1.40 - Invalid tcp port expression"           },
    { { TOKEN_TCP_PORTRANGE    , TOKEN_PORT_RANGE    }, PortRange_Port_Range, "ERROR 1.41 - Invalid tcp portrange expression"      },
    { { TOKEN_TCP_SRC_PORT     , TOKEN_DATA          }, Token_DataUInt16    , "ERROR 1.42 - Invalid tcp src port expression"       },
    { { TOKEN_TCP_SRC_PORTRANGE, TOKEN_PORT_RANGE    }, PortRange_Port_Range, "ERROR 1.43 - Invalid tcp src portranage expression" },
};

#define SEQ_COUNT ( sizeof( SEQ ) / sizeof( SEQ[ 0 ] ) )

// Function
/////////////////////////////////////////////////////////////////////////////

const char * Phase1(TokenList * aIn, TokenList * aOut)
{
    assert(NULL != aIn);
    assert(NULL != aOut);

    if (!aIn->empty())
    {
        Token * lT1 = aIn->front(); aIn->pop_front();
        assert(NULL != lT1);

        while (!aIn->empty())
        {
            Token * lT0 = lT1;

            lT1 = aIn->front(); aIn->pop_front();
            assert(NULL != lT1);

            unsigned int i;
            for (i = 0; i < SEQ_COUNT; i++)
            {
                const Seq & lSeq = SEQ[i];
                if (   lT0->Is(lSeq.mIds[0])
                    && lT1->Is(lSeq.mIds[1]))
                {
                    Token * lT = lSeq.mFunction(lT0, lT1);
                    if (NULL == lT)
                    {
                        return lSeq.mErrorMessage;
                    }

                    lT1 = lT;

                    break;
                }
            }

            if (SEQ_COUNT <= i)
            {
                aOut->push_back(lT0);
            }
        }

        aOut->push_back(lT1);
    }

    return NULL;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

// ===== Seq ================================================================

Token * Dst_Port     (Token * aDst, Token * aPort     ) { aPort     ->mId = TOKEN_DST_PORT     ; delete aDst; return aPort     ; }
Token * Dst_PortRange(Token * aDst, Token * aPortRange) { aPortRange->mId = TOKEN_DST_PORTRANGE; delete aDst; return aPortRange; }
Token * Ip_Proto     (Token * aIp , Token * aProto    ) { aProto    ->mId = TOKEN_IP_PROTO     ; delete aIp ; return aProto    ; }
Token * Ip6_Proto    (Token * aIp6, Token * aProto    ) { aProto    ->mId = TOKEN_IP6_PROTO    ; delete aIp6; return aProto    ; }

Token * Not_Boolean(Token * aNot, Token * aBoolean)
{
    assert(NULL != aNot);
    assert(NULL != aBoolean);

    aNot->SetA(aBoolean);
    aNot->SetBoolean();

    return aNot;
}

Token * PortRange_Port_Range(Token * aPortRange, Token * aPort_Range)
{
    assert(NULL != aPortRange );
    assert(NULL != aPort_Range);

    Token * lResult = NULL;

    if (!aPortRange->mFlags.mBoolean)
    {
        aPortRange->mData_UInt16[0] = aPort_Range->mData_UInt16[0];
        aPortRange->mData_UInt16[1] = aPort_Range->mData_UInt16[1];

        aPortRange->SetBoolean();

        lResult = aPortRange;
    }

    if (NULL == lResult)
    {
        delete aPortRange;
    }

    delete aPort_Range;

    return lResult;
}

Token * Src_Port        (Token * aSrc, Token * aPort        ) { aPort        ->mId = TOKEN_SRC_PORT         ; delete aSrc; return aPort        ; }
Token * Src_PortRange   (Token * aSrc, Token * aPortRange   ) { aPortRange   ->mId = TOKEN_SRC_PORTRANGE    ; delete aSrc; return aPortRange   ; }
Token * Udp_DstPort     (Token * aUdp, Token * aDstPort     ) { aDstPort     ->mId = TOKEN_UDP_DST_PORT     ; delete aUdp; return aDstPort     ; }
Token * Udp_DstPortRange(Token * aUdp, Token * aDstPortRange) { aDstPortRange->mId = TOKEN_UDP_DST_PORTRANGE; delete aUdp; return aDstPortRange; }
Token * Udp_Port        (Token * aUdp, Token * aPort        ) { aPort        ->mId = TOKEN_UDP_PORT         ; delete aUdp; return aPort        ; }
Token * Udp_PortRange   (Token * aUdp, Token * aPortRange   ) { aPortRange   ->mId = TOKEN_UDP_PORTRANGE    ; delete aUdp; return aPortRange   ; }
Token * Udp_SrcPort     (Token * aUdp, Token * aSrcPort     ) { aSrcPort     ->mId = TOKEN_UDP_SRC_PORT     ; delete aUdp; return aSrcPort     ; }
Token * Udp_SrcPortRange(Token * aUdp, Token * aSrcPortRange) { aSrcPortRange->mId = TOKEN_UDP_SRC_PORTRANGE; delete aUdp; return aSrcPortRange; }
Token * Tcp_DstPort     (Token * aTcp, Token * aDstPort     ) { aDstPort     ->mId = TOKEN_TCP_DST_PORT     ; delete aTcp; return aDstPort     ; }
Token * Tcp_DstPortRange(Token * aTcp, Token * aDstPortRange) { aDstPortRange->mId = TOKEN_TCP_DST_PORTRANGE; delete aTcp; return aDstPortRange; }
Token * Tcp_Port        (Token * aTcp, Token * aPort        ) { aPort        ->mId = TOKEN_TCP_PORT         ; delete aTcp; return aPort        ; }
Token * Tcp_PortRange   (Token * aTcp, Token * aPortRange   ) { aPortRange   ->mId = TOKEN_TCP_PORTRANGE    ; delete aTcp; return aPortRange   ; }
Token * Tcp_SrcPort     (Token * aTcp, Token * aSrcPort     ) { aSrcPort     ->mId = TOKEN_TCP_SRC_PORT     ; delete aTcp; return aSrcPort     ; }
Token * Tcp_SrcPortRange(Token * aTcp, Token * aSrcPortRange) { aSrcPortRange->mId = TOKEN_TCP_SRC_PORTRANGE; delete aTcp; return aSrcPortRange; }

Token * Token_DataUInt16(Token * aToken, Token * aData)
{
    assert(NULL != aToken);
    assert(NULL != aData );

    Token * lResult = NULL;

    if ((!aToken->mFlags.mBoolean) && aData->ConvertToUInt16())
    {
        aToken->mData_UInt16[0] = aData->mData_UInt16[0];
        aToken->SetBoolean();

        lResult = aToken;
    }

    if (NULL == lResult)
    {
        delete aToken;
    }

    delete aData;

    return lResult;
}

Token * Token_DataUInt8(Token * aToken, Token * aData)
{
    assert(NULL != aToken);
    assert(NULL != aData );

    Token * lResult = NULL;

    if ((!aToken->mFlags.mBoolean) && aData->ConvertToUInt8())
    {
        aToken->CopyData(*aData);
        aToken->SetBoolean();

        lResult = aToken;
    }

    if (NULL == lResult)
    {
        delete aToken;
    }

    delete aData;

    return lResult;
}

Token * Token_IpAddress(Token * aToken, Token * aData)
{
    assert(NULL != aToken);
    assert(NULL != aData );

    Token * lResult = NULL;

    if ((!aToken->mFlags.mBoolean) && aData->ConvertToIpAddress())
    {
        aToken->CopyData(*aData);
        aToken->SetBoolean();

        lResult = aToken;
    }

    if (NULL == lResult)
    {
        delete aToken;
    }

    delete aData;

    return lResult;
}
