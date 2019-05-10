
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
static Token * Arp_DstHost     (Token *, Token *);
static Token * Arp_Host        (Token *, Token *);
static Token * Arp_SrcHost     (Token *, Token *);
static Token * Dst_Host        (Token *, Token *);
static Token * Dst_Net         (Token *, Token *);
static Token * Dst_Port        (Token *, Token *);
static Token * Dst_PortRange   (Token *, Token *);
static Token * Ether_Dst       (Token *, Token *);
static Token * Ether_Host      (Token *, Token *);
static Token * Ether_Index     (Token *, Token *);
static Token * Ether_Proto     (Token *, Token *);
static Token * Ether_Src       (Token *, Token *);
static Token * In_List         (Token *, Token *);
static Token * Ip_DstHost      (Token *, Token *);
static Token * Ip_Host         (Token *, Token *);
static Token * Ip_Multicast    (Token *, Token *);
static Token * Ip_Net          (Token *, Token *);
static Token * Ip_Proto        (Token *, Token *);
static Token * Ip_SrcHost      (Token *, Token *);
static Token * Ip6_DstHost     (Token *, Token *);
static Token * Ip6_Host        (Token *, Token *);
static Token * Ip6_Multicast   (Token *, Token *);
static Token * Ip6_Net         (Token *, Token *);
static Token * Ip6_Proto       (Token *, Token *);
static Token * Ip6_SrcHost     (Token *, Token *);
static Token * Not_Boolean     (Token *, Token *);
static Token * PortRange_Data  (Token *, Token *);
static Token * Proto_Ah        (Token *, Token *);
static Token * Proto_Esp       (Token *, Token *);
static Token * Proto_Icmp6     (Token *, Token *);
static Token * Proto_Igmp      (Token *, Token *);
static Token * Proto_Pim       (Token *, Token *);
static Token * Proto_Vrrp      (Token *, Token *);
static Token * Src_Host        (Token *, Token *);
static Token * Src_Net         (Token *, Token *);
static Token * Src_Port        (Token *, Token *);
static Token * Src_PortRange   (Token *, Token *);
static Token * Tcp_DstPort     (Token *, Token *);
static Token * Tcp_DstPortRange(Token *, Token *);
static Token * Tcp_Port        (Token *, Token *);
static Token * Tcp_PortRange   (Token *, Token *);
static Token * Tcp_SrcPort     (Token *, Token *);
static Token * Tcp_SrcPortRange(Token *, Token *);
static Token * Udp_DstPort     (Token *, Token *);
static Token * Udp_DstPortRange(Token *, Token *);
static Token * Udp_Port        (Token *, Token *);
static Token * Udp_PortRange   (Token *, Token *);
static Token * Udp_SrcPort     (Token *, Token *);
static Token * Udp_SrcPortRange(Token *, Token *);

static Token * OptionalPrefix_Token(Token * aPrefix, Token * aToken);

static Token * Token_EthernetAddress(Token * aToken, Token * aData);
static Token * Token_EthernetType   (Token * aToken, Token * aData);
static Token * Token_IpAddress      (Token * aToken, Token * aData);
static Token * Token_IpProtocol     (Token * aToken, Token * aData);
static Token * Token_Network        (Token * aToken, Token * aData);
static Token * Token_PacketSize     (Token * aToken, Token * aData);
static Token * Token_Port           (Token * aToken, Token * aData);
static Token * Token_VlanId         (Token * aToken, Token * aData);

// Constants
/////////////////////////////////////////////////////////////////////////////

static const Seq SEQ[]
{
    { { TOKEN_ARP              , TOKEN_DST_HOST      }, Arp_DstHost          , NULL                                                 },
    { { TOKEN_ARP              , TOKEN_HOST          }, Arp_Host             , NULL                                                 },
    { { TOKEN_ARP              , TOKEN_SRC_HOST      }, Arp_SrcHost          , NULL                                                 },
    { { TOKEN_ARP_DST_HOST     , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.1 - Invalid arp dst host expression"        },
    { { TOKEN_ARP_HOST         , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.2 - Invalid arp host expression"            },
    { { TOKEN_ARP_SRC_HOST     , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.3 - Invalid arp src host expression"        },
    { { TOKEN_DST              , TOKEN_HOST          }, Dst_Host             , NULL                                                 },
    { { TOKEN_DST              , TOKEN_NET           }, Dst_Net              , NULL                                                 },
    { { TOKEN_DST              , TOKEN_PORT          }, Dst_Port             , NULL                                                 },
    { { TOKEN_DST              , TOKEN_PORTRANGE     }, Dst_PortRange        , NULL                                                 },
    { { TOKEN_DST_HOST         , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.4 - Invalid dst host expression"            },
    { { TOKEN_DST_NET          , TOKEN_DATA          }, Token_Network        , "ERROR 1.5 - Invalid dst net expression"             },
    { { TOKEN_DST_PORT         , TOKEN_DATA          }, Token_Port           , "ERROR 1.6 - Invalid dst port number"                },
    { { TOKEN_ETHER            , TOKEN_BROADCAST     }, OptionalPrefix_Token , NULL                                                 },
    { { TOKEN_ETHER            , TOKEN_DST           }, Ether_Dst            , NULL                                                 },
    { { TOKEN_ETHER            , TOKEN_HOST          }, Ether_Host           , NULL                                                 },
    { { TOKEN_ETHER            , TOKEN_INDEX         }, Ether_Index          , NULL                                                 },
    { { TOKEN_ETHER            , TOKEN_MULTICAST     }, OptionalPrefix_Token , NULL                                                 },
    { { TOKEN_ETHER            , TOKEN_PROTO         }, Ether_Proto          , NULL                                                 },
    { { TOKEN_ETHER            , TOKEN_SRC           }, Ether_Src            , NULL                                                 },
    { { TOKEN_ETHER            , TOKEN_VALUE         }, Ether_Index          , NULL                                                 },
    { { TOKEN_ETHER_DST        , TOKEN_DATA          }, Token_EthernetAddress, "ERROR 1.7 - Invalid ether dst expression"           },
    { { TOKEN_ETHER_HOST       , TOKEN_DATA          }, Token_EthernetAddress, "ERROR 1.8 - Invalid ether host expression"          },
    { { TOKEN_ETHER_PROTO      , TOKEN_DATA          }, Token_EthernetType   , "ERROR 1.9 - Invalid ether proto expression"         },
    { { TOKEN_ETHER_SRC        , TOKEN_DATA          }, Token_EthernetAddress, "ERROR 1.10 - Invalid ether src expression"          },
    { { TOKEN_GREATER          , TOKEN_DATA          }, Token_PacketSize     , "ERROR 1.11 - Invalid greater expression"            },
    { { TOKEN_HOST             , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.12 - Invalid host expression"               },
    { { TOKEN_IN_ETHERNET      , TOKEN_LIST          }, In_List              , NULL                                                 },
    { { TOKEN_IN_IP            , TOKEN_LIST          }, In_List              , NULL                                                 },
    { { TOKEN_IN_TCP           , TOKEN_LIST          }, In_List              , NULL                                                 },
    { { TOKEN_IN_UDP           , TOKEN_LIST          }, In_List              , NULL                                                 },
    { { TOKEN_IP               , TOKEN_DST_HOST      }, Ip_DstHost           , NULL                                                 },
    { { TOKEN_IP               , TOKEN_HOST          }, Ip_Host              , NULL                                                 },
    { { TOKEN_IP               , TOKEN_MULTICAST     }, Ip_Multicast         , NULL                                                 },
    { { TOKEN_IP               , TOKEN_NET           }, Ip_Net               , NULL                                                 },
    { { TOKEN_IP               , TOKEN_PROTO         }, Ip_Proto             , NULL                                                 },
    { { TOKEN_IP               , TOKEN_SRC_HOST      }, Ip_SrcHost           , NULL                                                 },
    { { TOKEN_IP_DST_HOST      , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.13 - Invalid ip dst host expression"        },
    { { TOKEN_IP_HOST          , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.14 - Invalid ip host expression"            },
    { { TOKEN_IP_NET           , TOKEN_DATA          }, Token_Network        , "ERROR 1.15 - Invalid ip net expression"             },
    { { TOKEN_IP_PROTO         , TOKEN_DATA          }, Token_IpProtocol     , "ERROR 1.16 - Invalid ip proto expression"           },
    { { TOKEN_IP_SRC_HOST      , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.17 - Invalid ip src host expression"        },
    { { TOKEN_IP6              , TOKEN_DST_HOST      }, Ip6_DstHost          , NULL                                                 },
    { { TOKEN_IP6              , TOKEN_HOST          }, Ip6_Host             , NULL                                                 },
    { { TOKEN_IP6              , TOKEN_MULTICAST     }, Ip6_Multicast        , NULL                                                 },
    { { TOKEN_IP6              , TOKEN_NET           }, Ip6_Net              , NULL                                                 },
    { { TOKEN_IP6              , TOKEN_PROTO         }, Ip6_Proto            , NULL                                                 },
    { { TOKEN_IP6              , TOKEN_SRC_HOST      }, Ip6_SrcHost          , NULL                                                 },
    { { TOKEN_IP6_DST_HOST     , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.18 - Invalid ip6 dst host expression"       },
    { { TOKEN_IP6_HOST         , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.19 - Invalid ip6 host expression"           },
    { { TOKEN_IP6_PROTO        , TOKEN_DATA          }, Token_IpProtocol     , "ERROR 1.20 - Invalid ip6 proto expression"          },
    { { TOKEN_IP6_NET          , TOKEN_DATA          }, Token_Network        , "ERROR 1.21 - Invalid ip6 net expression"            },
    { { TOKEN_IP6_SRC_HOST     , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.22 - Invalid ip6 src host expression"       },
    { { TOKEN_LESS             , TOKEN_DATA          }, Token_PacketSize     , "ERROR 1.23 - Invalid less expression"               },
    { { TOKEN_NET              , TOKEN_DATA          }, Token_Network        , "ERROR 1.24 - Invalid net expression"                },
    { { TOKEN_NOT              , TOKEN_BOOLEAN       }, Not_Boolean          , NULL                                                 },
    { { TOKEN_PORT             , TOKEN_DATA          }, Token_Port           , "ERROR 1.25 - Invalid port number"                   },
    { { TOKEN_PORTRANGES       , TOKEN_DATA          }, PortRange_Data       , "ERROR 1.26 - Invalid port range expression"         },
    { { TOKEN_PROTO            , TOKEN_DATA          }, Token_IpProtocol     , "ERROR 1.27 - Invalid proto expression"              },
    { { TOKEN_PROTO            , TOKEN_AH            }, Proto_Ah             , NULL                                                 },
    { { TOKEN_PROTO            , TOKEN_ESP           }, Proto_Esp            , NULL                                                 },
    { { TOKEN_PROTO            , TOKEN_ICMP6         }, Proto_Icmp6          , NULL                                                 },
    { { TOKEN_PROTO            , TOKEN_IGMP          }, Proto_Igmp           , NULL                                                 },
    { { TOKEN_PROTO            , TOKEN_PIM           }, Proto_Pim            , NULL                                                 },
    { { TOKEN_PROTO            , TOKEN_VRRP          }, Proto_Vrrp           , NULL                                                 },
    { { TOKEN_SRC              , TOKEN_HOST          }, Src_Host             , NULL                                                 },
    { { TOKEN_SRC              , TOKEN_NET           }, Src_Net              , NULL                                                 },
    { { TOKEN_SRC              , TOKEN_PORT          }, Src_Port             , NULL                                                 },
    { { TOKEN_SRC              , TOKEN_PORTRANGE     }, Src_PortRange        , NULL                                                 },
    { { TOKEN_SRC_HOST         , TOKEN_DATA          }, Token_IpAddress      , "ERROR 1.28 - Invalid src host expression"           },
    { { TOKEN_SRC_NET          , TOKEN_DATA          }, Token_Network        , "ERROR 1.29 - Invalid dst net expression"            },
    { { TOKEN_SRC_PORT         , TOKEN_DATA          }, Token_Port           , "ERROR 1.30 - Invalid src port number"               },
    { { TOKEN_TCP              , TOKEN_DST_PORT      }, Tcp_DstPort          , NULL                                                 },
    { { TOKEN_TCP              , TOKEN_DST_PORTRANGE }, Tcp_DstPortRange     , NULL                                                 },
    { { TOKEN_TCP              , TOKEN_PORT          }, Tcp_Port             , NULL                                                 },
    { { TOKEN_TCP              , TOKEN_PORTRANGE     }, Tcp_PortRange        , NULL                                                 },
    { { TOKEN_TCP              , TOKEN_SRC_PORT      }, Tcp_SrcPort          , NULL                                                 },
    { { TOKEN_TCP              , TOKEN_SRC_PORTRANGE }, Tcp_SrcPortRange     , NULL                                                 },
    { { TOKEN_TCP_DST_PORT     , TOKEN_DATA          }, Token_Port           , "ERROR 1.31 - Invalid tcp dst port expression"       },
    { { TOKEN_TCP_PORT         , TOKEN_DATA          }, Token_Port           , "ERROR 1.32 - Invalid tcp port expression"           },
    { { TOKEN_TCP_SRC_PORT     , TOKEN_DATA          }, Token_Port           , "ERROR 1.33 - Invalid tcp src port expression"       },
    { { TOKEN_UDP              , TOKEN_DST_PORT      }, Udp_DstPort          , NULL                                                 },
    { { TOKEN_UDP              , TOKEN_DST_PORTRANGE }, Udp_DstPortRange     , NULL                                                 },
    { { TOKEN_UDP              , TOKEN_PORT          }, Udp_Port             , NULL                                                 },
    { { TOKEN_UDP              , TOKEN_PORTRANGE     }, Udp_PortRange        , NULL                                                 },
    { { TOKEN_UDP              , TOKEN_SRC_PORT      }, Udp_SrcPort          , NULL                                                 },
    { { TOKEN_UDP              , TOKEN_SRC_PORTRANGE }, Udp_SrcPortRange     , NULL                                                 },
    { { TOKEN_UDP_DST_PORT     , TOKEN_DATA          }, Token_Port           , "ERROR 1.34 - Invalid udp dst port expression"       },
    { { TOKEN_UDP_PORT         , TOKEN_DATA          }, Token_Port           , "ERROR 1.35 - Invalid udp port expression"           },
    { { TOKEN_UDP_SRC_PORT     , TOKEN_DATA          }, Token_Port           , "ERROR 1.36 - Invalid udp src port expression"       },
    { { TOKEN_VLAN             , TOKEN_DATA          }, Token_VlanId         , "ERROR 1.37 - Invalid vlan expression"               },
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

Token * Arp_DstHost  (Token * aArp  , Token * aDstHost  ) { aDstHost  ->mId = TOKEN_ARP_DST_HOST ;                     delete aArp  ; return aDstHost  ; }
Token * Arp_Host     (Token * aArp  , Token * aHost     ) { aHost     ->mId = TOKEN_ARP_HOST     ;                     delete aArp  ; return aHost     ; }
Token * Arp_SrcHost  (Token * aArp  , Token * aSrcHost  ) { aSrcHost  ->mId = TOKEN_ARP_SRC_HOST ;                     delete aArp  ; return aSrcHost  ; }
Token * Dst_Host     (Token * aDst  , Token * aHost     ) { aHost     ->mId = TOKEN_DST_HOST     ;                     delete aDst  ; return aHost     ; }
Token * Dst_Net      (Token * aDst  , Token * aNet      ) { aNet      ->mId = TOKEN_DST_NET      ;                     delete aDst  ; return aNet      ; }
Token * Dst_Port     (Token * aDst  , Token * aPort     ) { aPort     ->mId = TOKEN_DST_PORT     ;                     delete aDst  ; return aPort     ; }
Token * Dst_PortRange(Token * aDst  , Token * aPortRange) { aPortRange->mId = TOKEN_DST_PORTRANGE;                     delete aDst  ; return aPortRange; }
Token * Ether_Dst    (Token * aEther, Token * aDst      ) { aEther    ->mId = TOKEN_ETHER_DST    ;                     delete aDst  ; return aEther    ; }
Token * Ether_Host   (Token * aEther, Token * aHost     ) { aEther    ->mId = TOKEN_ETHER_HOST   ;                     delete aHost ; return aEther    ; }
Token * Ether_Index  (Token * aEther, Token * aIndex    ) { aIndex    ->mId = TOKEN_ETHER_INDEX  ; aIndex->SetValue(); delete aEther; return aIndex    ; }
Token * Ether_Proto  (Token * aEther, Token * aProto    ) { aProto    ->mId = TOKEN_ETHER_PROTO  ;                     delete aEther; return aProto    ; }
Token * Ether_Src    (Token * aEther, Token * aSrc      ) { aEther    ->mId = TOKEN_ETHER_SRC    ;                     delete aSrc  ; return aEther    ; }
Token * In_List      (Token * aIn   , Token * aList     ) { aList     ->mId = aIn->mId           ;                     delete aIn   ; return aList     ; }
Token * Ip_DstHost   (Token * aIp   , Token * aDstHost  ) { aDstHost  ->mId = TOKEN_IP_DST_HOST  ;                     delete aIp   ; return aDstHost  ; }
Token * Ip_Host      (Token * aIp   , Token * aHost     ) { aHost     ->mId = TOKEN_IP_HOST      ;                     delete aIp   ; return aHost     ; }
Token * Ip_Multicast (Token * aIp   , Token * aMulticast) { aMulticast->mId = TOKEN_IP_MULTICAST ;                     delete aIp   ; return aMulticast; }
Token * Ip_Net       (Token * aIp   , Token * aNet      ) { aNet      ->mId = TOKEN_IP_NET       ;                     delete aIp   ; return aNet      ; }
Token * Ip_Proto     (Token * aIp   , Token * aProto    ) { aProto    ->mId = TOKEN_IP_PROTO     ;                     delete aIp   ; return aProto    ; }
Token * Ip_SrcHost   (Token * aIp   , Token * aSrcHost  ) { aSrcHost  ->mId = TOKEN_IP_SRC_HOST  ;                     delete aIp   ; return aSrcHost  ; }
Token * Ip6_DstHost  (Token * aIp6  , Token * aDstHost  ) { aDstHost  ->mId = TOKEN_IP6_DST_HOST ;                     delete aIp6  ; return aDstHost  ; }
Token * Ip6_Host     (Token * aIp6  , Token * aHost     ) { aHost     ->mId = TOKEN_IP6_HOST     ;                     delete aIp6  ; return aHost     ; }
Token * Ip6_Multicast(Token * aIp6  , Token * aMulticast) { aMulticast->mId = TOKEN_IP6_MULTICAST;                     delete aIp6  ; return aMulticast; }
Token * Ip6_Net      (Token * aIp6  , Token * aNet      ) { aNet      ->mId = TOKEN_IP6_NET      ;                     delete aIp6  ; return aNet      ; }
Token * Ip6_Proto    (Token * aIp6  , Token * aProto    ) { aProto    ->mId = TOKEN_IP6_PROTO    ;                     delete aIp6  ; return aProto    ; }
Token * Ip6_SrcHost  (Token * aIp6  , Token * aSrcHost  ) { aSrcHost  ->mId = TOKEN_IP6_SRC_HOST ;                     delete aIp6  ; return aSrcHost  ; }

Token * Not_Boolean(Token * aNot, Token * aBoolean)
{
    assert(NULL != aNot);
    assert(NULL != aBoolean);

    aNot->SetA(aBoolean);
    aNot->SetBoolean();

    return aNot;
}

Token * PortRange_Data(Token * aPortRange, Token * aData)
{
    assert(NULL != aPortRange);
    assert(NULL != aData     );

    Token * lResult;

    if (aData->ConvertToPortRange())
    {
        aPortRange->CopyData(*aData);
        aPortRange->SetBoolean();

        lResult = aPortRange;
    }
    else
    {
        delete aPortRange;
        lResult = NULL;
    }

    delete aData;

    return lResult;
}

Token * Proto_Ah   (Token * aProto, Token * aAh   ) { aProto->mData.mIpProtocol =  51; aProto->mData.mFlags.mIpProtocol = true; aProto->SetBoolean(); delete aAh   ; return aProto; }
Token * Proto_Esp  (Token * aProto, Token * aEsp  ) { aProto->mData.mIpProtocol =  50; aProto->mData.mFlags.mIpProtocol = true; aProto->SetBoolean(); delete aEsp  ; return aProto; }
Token * Proto_Icmp6(Token * aProto, Token * aIcmp6) { aProto->mData.mIpProtocol =  58; aProto->mData.mFlags.mIpProtocol = true; aProto->SetBoolean(); delete aIcmp6; return aProto; }
Token * Proto_Igmp (Token * aProto, Token * aIgmp ) { aProto->mData.mIpProtocol =   2; aProto->mData.mFlags.mIpProtocol = true; aProto->SetBoolean(); delete aIgmp ; return aProto; }
Token * Proto_Pim  (Token * aProto, Token * aPim  ) { aProto->mData.mIpProtocol = 103; aProto->mData.mFlags.mIpProtocol = true; aProto->SetBoolean(); delete aPim  ; return aProto; }
Token * Proto_Vrrp (Token * aProto, Token * aVrrp ) { aProto->mData.mIpProtocol = 112; aProto->mData.mFlags.mIpProtocol = true; aProto->SetBoolean(); delete aVrrp ; return aProto; }

Token * Src_Host        (Token * aSrc, Token * aHost        ) { aHost        ->mId = TOKEN_SRC_HOST         ; delete aSrc; return aHost        ; }
Token * Src_Net         (Token * aSrc, Token * aNet         ) { aNet         ->mId = TOKEN_SRC_NET          ; delete aSrc; return aNet         ; }
Token * Src_Port        (Token * aSrc, Token * aPort        ) { aPort        ->mId = TOKEN_SRC_PORT         ; delete aSrc; return aPort        ; }
Token * Src_PortRange   (Token * aSrc, Token * aPortRange   ) { aPortRange   ->mId = TOKEN_SRC_PORTRANGE    ; delete aSrc; return aPortRange   ; }
Token * Tcp_DstPort     (Token * aTcp, Token * aDstPort     ) { aDstPort     ->mId = TOKEN_TCP_DST_PORT     ; delete aTcp; return aDstPort     ; }
Token * Tcp_DstPortRange(Token * aTcp, Token * aDstPortRange) { aDstPortRange->mId = TOKEN_TCP_DST_PORTRANGE; delete aTcp; return aDstPortRange; }
Token * Tcp_Port        (Token * aTcp, Token * aPort        ) { aPort        ->mId = TOKEN_TCP_PORT         ; delete aTcp; return aPort        ; }
Token * Tcp_PortRange   (Token * aTcp, Token * aPortRange   ) { aPortRange   ->mId = TOKEN_TCP_PORTRANGE    ; delete aTcp; return aPortRange   ; }
Token * Tcp_SrcPort     (Token * aTcp, Token * aSrcPort     ) { aSrcPort     ->mId = TOKEN_TCP_SRC_PORT     ; delete aTcp; return aSrcPort     ; }
Token * Tcp_SrcPortRange(Token * aTcp, Token * aSrcPortRange) { aSrcPortRange->mId = TOKEN_TCP_SRC_PORTRANGE; delete aTcp; return aSrcPortRange; }
Token * Udp_DstPort     (Token * aUdp, Token * aDstPort     ) { aDstPort     ->mId = TOKEN_UDP_DST_PORT     ; delete aUdp; return aDstPort     ; }
Token * Udp_DstPortRange(Token * aUdp, Token * aDstPortRange) { aDstPortRange->mId = TOKEN_UDP_DST_PORTRANGE; delete aUdp; return aDstPortRange; }
Token * Udp_Port        (Token * aUdp, Token * aPort        ) { aPort        ->mId = TOKEN_UDP_PORT         ; delete aUdp; return aPort        ; }
Token * Udp_PortRange   (Token * aUdp, Token * aPortRange   ) { aPortRange   ->mId = TOKEN_UDP_PORTRANGE    ; delete aUdp; return aPortRange   ; }
Token * Udp_SrcPort     (Token * aUdp, Token * aSrcPort     ) { aSrcPort     ->mId = TOKEN_UDP_SRC_PORT     ; delete aUdp; return aSrcPort     ; }
Token * Udp_SrcPortRange(Token * aUdp, Token * aSrcPortRange) { aSrcPortRange->mId = TOKEN_UDP_SRC_PORTRANGE; delete aUdp; return aSrcPortRange; }

Token * OptionalPrefix_Token(Token * aPrefix, Token * aToken) { delete aPrefix; return aToken; }

Token * Token_EthernetAddress(Token * aToken, Token * aData)
{
    assert(NULL != aToken);
    assert(NULL != aData );

    Token * lResult;

    if ((!aToken->mFlags.mBoolean) && aData->ConvertToEthernetAddress())
    {
        aToken->CopyData(*aData);
        aToken->SetBoolean();
        lResult = aToken;
    }
    else
    {
        lResult = NULL;
        delete aToken;
    }

    delete aData;

    return lResult;
}

Token * Token_EthernetType(Token * aToken, Token * aData)
{
    assert(NULL != aToken);
    assert(NULL != aData);

    Token * lResult;

    if ((!aToken->mFlags.mBoolean) && aData->ConvertToEthernetType())
    {
        aToken->CopyData(*aData);
        aToken->SetBoolean();
        lResult = aToken;
    }
    else
    {
        lResult = NULL;
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

Token * Token_IpProtocol(Token * aToken, Token * aData)
{
    assert(NULL != aToken);
    assert(NULL != aData);

    Token * lResult;

    if ((!aToken->mFlags.mBoolean) && aData->ConvertToIpProtocol())
    {
        aToken->CopyData(*aData);
        aToken->SetBoolean();
        lResult = aToken;
    }
    else
    {
        delete aToken;
        lResult = NULL;
    }

    delete aData;

    return lResult;
}

Token * Token_Network(Token * aToken, Token * aData)
{
    assert(NULL != aToken);
    assert(NULL != aData );

    Token * lResult;

    if ((!aToken->mFlags.mBoolean) && aData->ConvertToNetwork())
    {
        aToken->CopyData(*aData);
        aToken->SetBoolean();
        lResult = aToken;
    }
    else
    {
        delete aToken;
        lResult = NULL;
    }

    delete aData;

    return lResult;
}

Token * Token_PacketSize(Token * aToken, Token * aData)
{
    assert(NULL != aToken);
    assert(NULL != aData );

    Token * lResult;

    if ((!aToken->mFlags.mBoolean) && aData->ConvertToPacketSize())
    {
        aToken->CopyData(*aData);
        aToken->SetBoolean();
        lResult = aToken;
    }
    else
    {
        delete aToken;
        lResult = NULL;
    }

    delete aData;

    return lResult;
}

Token * Token_Port(Token * aToken, Token * aData)
{
    assert(NULL != aToken);
    assert(NULL != aData);

    Token * lResult;

    if ((!aToken->mFlags.mBoolean) && aData->ConvertToPort())
    {
        aToken->CopyData(*aData);
        aToken->SetBoolean();
        lResult = aToken;
    }
    else
    {
        delete aToken;
        lResult = NULL;
    }

    delete aData;

    return lResult;
}

Token * Token_VlanId(Token * aToken, Token * aData)
{
    assert(NULL != aToken);
    assert(NULL != aData );

    Token * lResult;

    if (aData->ConvertToVlanId())
    {
        aToken->CopyData(*aData);
        lResult = aToken;
    }
    else
    {
        delete aToken;
        lResult = NULL;
    }

    delete aData;

    return lResult;
}
