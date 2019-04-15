
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Phase3.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== CodeGen ============================================================
#include "Filter_Internal.h"
#include "Token.h"

#include "Phase3.h"

// Data types
/////////////////////////////////////////////////////////////////////////////

typedef void(*GenFunction)(const Token &, const char *, Filter_Internal *);

typedef struct
{
    TokenId     mId      ;
    GenFunction mFunction;
}
Gen;

typedef struct
{
    TokenId mId        ;
    TokenId mRealId    ;
    uint8_t mData_UInt8;
}
Abbreviation;

// Static function declaration
/////////////////////////////////////////////////////////////////////////////

static void Begin_IP(const char * aResultName, Filter_Internal * aOut, const char * aLocalVar = NULL);

static void End_IP(Filter_Internal * aOut);

static void GenerateCodeForToken(const Token & aToken, const char * aResultName, Filter_Internal * aOut);

unsigned short SwapBytes(unsigned short aIn);

// ===== Gen ================================================================
static void And            (const Token & aAnd            , const char * aResultName, Filter_Internal * aOut);
static void DstPort        (const Token & aDstPort        , const char * aResultName, Filter_Internal * aOut);
static void DstPortRange   (const Token & aDstPortRange   , const char * aResultName, Filter_Internal * aOut);
static void Greater        (const Token & aGreater        , const char * aResultName, Filter_Internal * aOut);
static void Host           (const Token & aHost           , const char * aResultName, Filter_Internal * aOut);
static void IpProto        (const Token & aIpProto        , const char * aResultName, Filter_Internal * aOut);
static void Ip6Proto       (const Token & aIpProto        , const char * aResultName, Filter_Internal * aOut);
static void Less           (const Token & aLess           , const char * aResultName, Filter_Internal * aOut);
static void Not            (const Token & aNot            , const char * aResultName, Filter_Internal * aOut);
static void Or             (const Token & aOr             , const char * aResultName, Filter_Internal * aOut);
static void Port           (const Token & aPort           , const char * aResultName, Filter_Internal * aOut);
static void PortRange      (const Token & aPortRange      , const char * aResultName, Filter_Internal * aOut);
static void Proto          (const Token & aIpProto        , const char * aResultName, Filter_Internal * aOut);
static void SrcPort        (const Token & aSrcPort        , const char * aResultName, Filter_Internal * aOut);
static void SrcPortRange   (const Token & aSrcPortRange   , const char * aResultName, Filter_Internal * aOut);
static void UdpDstPort     (const Token & aUdpDstPort     , const char * aResultName, Filter_Internal * aOut);
static void UdpDstPortRange(const Token & aUdpDstPortRange, const char * aResultName, Filter_Internal * aOut);
static void UdpPort        (const Token & aUdpPort        , const char * aResultName, Filter_Internal * aOut);
static void UdpPortRange   (const Token & aUdpPortRange   , const char * aResultName, Filter_Internal * aOut);
static void UdpSrcPort     (const Token & aUdpSrcPort     , const char * aResultName, Filter_Internal * aOut);
static void UdpSrcPortRange(const Token & aUdpSrcPortRange, const char * aResultName, Filter_Internal * aOut);
static void TcpDstPort     (const Token & aTcpDstPort     , const char * aResultName, Filter_Internal * aOut);
static void TcpDstPortRange(const Token & aTcpDstPortRange, const char * aResultName, Filter_Internal * aOut);
static void TcpPort        (const Token & aTcpPort        , const char * aResultName, Filter_Internal * aOut);
static void TcpPortRange   (const Token & aTcpPortRange   , const char * aResultName, Filter_Internal * aOut);
static void TcpSrcPort     (const Token & aTcpSrcPort     , const char * aResultName, Filter_Internal * aOut);
static void TcpSrcPortRange(const Token & aTcpSrcPortRange, const char * aResultName, Filter_Internal * aOut);

// Constants
/////////////////////////////////////////////////////////////////////////////

static const Gen GEN[]
{
    { TOKEN_AND              , And             },
    { TOKEN_DST_PORT         , DstPort         },
    { TOKEN_DST_PORTRANGE    , DstPortRange    },
    { TOKEN_GREATER          , Greater         },
    { TOKEN_HOST             , Host            },
    { TOKEN_IP_PROTO         , IpProto         },
    { TOKEN_IP6_PROTO        , Ip6Proto        },
    { TOKEN_LESS             , Less            },
    { TOKEN_NOT              , Not             },
    { TOKEN_OR               , Or              },
    { TOKEN_PORT             , Port            },
    { TOKEN_PORTRANGE        , PortRange       },
    { TOKEN_PROTO            , Proto           },
    { TOKEN_SRC_PORT         , SrcPort         },
    { TOKEN_SRC_PORTRANGE    , SrcPortRange    },
    { TOKEN_UDP_DST_PORT     , UdpDstPort      },
    { TOKEN_UDP_DST_PORTRANGE, UdpDstPortRange },
    { TOKEN_UDP_PORT         , UdpPort         },
    { TOKEN_UDP_PORTRANGE    , UdpPortRange    },
    { TOKEN_UDP_SRC_PORT     , UdpSrcPort      },
    { TOKEN_UDP_SRC_PORTRANGE, UdpSrcPortRange },
    { TOKEN_TCP_DST_PORT     , TcpDstPort      },
    { TOKEN_TCP_DST_PORTRANGE, TcpDstPortRange },
    { TOKEN_TCP_PORT         , TcpPort         },
    { TOKEN_TCP_PORTRANGE    , TcpPortRange    },
    { TOKEN_TCP_SRC_PORT     , TcpSrcPort      },
    { TOKEN_TCP_SRC_PORTRANGE, TcpSrcPortRange },
};

#define GEN_COUNT ( sizeof( GEN ) / sizeof( GEN[ 0 ] ) )

static const Abbreviation ABBREVIATION[]
{
    { TOKEN_ICMP, TOKEN_PROTO,  1 },
    { TOKEN_TCP , TOKEN_PROTO,  6 },
    { TOKEN_UDP , TOKEN_PROTO, 17 },
};

#define ABBREVIATION_COUNT ( sizeof( ABBREVIATION ) / sizeof( ABBREVIATION[ 0 ] ) )

// Function
/////////////////////////////////////////////////////////////////////////////

void Phase3(const TokenList & aIn, Filter_Internal * aOut)
{
    aOut->Begin();

    Token * lT = aIn.front();

    GenerateCodeForToken(*lT, "lR", aOut);

    if (NULL == aOut->GetErrorMessage())
    {
        aOut->End();
    }
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void Begin_IP(const char * aResultName, Filter_Internal * aOut, const char * aLocalVar)
{
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    char lLine[1024];

    sprintf_s(lLine, "%s = 0;" EOL, aResultName);
    aOut->Append(lLine);

    aOut->Append(    "if ( NULL != lIPv4 )" EOL);
    aOut->Append(    "{"                    EOL);
    aOut->Indent_Inc();

    if (NULL != aLocalVar)
    {
        aOut->Append(aLocalVar);
        aOut->Append(EOL);
    }

    aOut->Append(        "switch ( lIPv4_Protocol )" EOL);
    aOut->Append(        "{"                         EOL);
}

void End_IP(Filter_Internal * aOut)
{
    assert(NULL != aOut);

    aOut->Indent_Dec();
    aOut->Append(        "}" EOL);
    aOut->Indent_Dec();
    aOut->Append(    "}" EOL);
    aOut->Append(        EOL);
}

void GenerateCodeForToken(const Token & aToken, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aToken)  );
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    if (aToken.mFlags.mBoolean)
    {
        for (unsigned int i = 0; i < GEN_COUNT; i++)
        {
            const Gen & lGen = GEN[i];

            if (lGen.mId == aToken.mId)
            {
                lGen.mFunction(aToken, aResultName, aOut);
                return;
            }
        }

        aOut->SetErrorMessage("ERROR 3.1 - Invalid filter");
    }
    else
    {
        for (unsigned int i = 0; i < ABBREVIATION_COUNT; i++)
        {
            const Abbreviation & lAbb = ABBREVIATION[i];

            if (lAbb.mId == aToken.mId)
            {
                Token lT( aToken );

                lT.mData_UInt8[0] = lAbb.mData_UInt8;
                lT.mId            = lAbb.mRealId    ;

                lT.SetBoolean();

                GenerateCodeForToken(lT, aResultName, aOut);
                return;
            }
        }

        aOut->SetErrorMessage("ERROR 3.2 - Invalid filter");
    }
}

unsigned short SwapBytes(unsigned short aIn)
{
    return ((aIn >> 8) | (aIn << 8));
}

// ===== Gen ================================================================

void And(const Token & aAnd, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aAnd)    );
    assert(NULL != aAnd.mA    );
    assert(NULL != aAnd.mB    );
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    GenerateCodeForToken(*aAnd.mA, aResultName, aOut);
    if (NULL == aOut->GetErrorMessage())
    {
        char lLine[1024];

        sprintf_s(lLine, "if ( %s )" EOL, aResultName);
        aOut->Append(lLine);
        
        aOut->Append(    "{" EOL);
        aOut->Indent_Inc();

        GenerateCodeForToken(*aAnd.mB, aResultName, aOut);
        if (NULL == aOut->GetErrorMessage())
        {
            aOut->Indent_Dec();
            aOut->Append("}" EOL);
        }
    }
}

void DstPort(const Token & aDstPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aDstPort));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    unsigned short lPort_nh = SwapBytes(aDstPort.mData_UInt16[0]);

    char lLine[1024];

    Begin_IP(aResultName, aOut);

    aOut->Append("case TCP_IP_PROTOCOL :"  EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == TCP_DestinationPort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);          
                                  
    aOut->Append(    "break;" EOL);
    aOut->Indent_Dec();
    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == UDP_DestinationPort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);       
                               
    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void DstPortRange(const Token & aDstPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aDstPortRange));
    assert(NULL != aResultName     );
    assert(NULL != aOut            );

    unsigned short lPortA = aDstPortRange.mData_UInt16[0];
    unsigned short lPortB = aDstPortRange.mData_UInt16[1];

    char lLine[1024];

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->Append("case TCP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = TCP_DestinationPort( lIP );"      EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    sprintf_s(lLine, "if ( ( 0x%04x <= lPort ) && ( 0x%04x >= lPort ) ) { lR = 1; }" EOL, lPortA, lPortB);
    aOut->Append(lLine);

    aOut->Append(    "break;" EOL);
    aOut->Indent_Dec();
    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = UDP_DestinationPort( lIP );"      EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    aOut->Append(lLine);

    aOut->Append("break;" EOL);

    End_IP(aOut);
}

void Greater(const Token & aGreater, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aGreater));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    char lLine[1024];

    sprintf_s(lLine, "%s = ( lPacketInfo->mSize_byte >= %u );" EOL, aResultName, aGreater.mData_UInt16[0]);
    aOut->Append(lLine);
}

void Host(const Token & aHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aHost   ));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    char lLine[1024];

    sprintf_s(lLine, "%s = 0;" EOL, aResultName);
    aOut->Append(lLine);

    aOut->Append(    "OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                           EOL);
    aOut->Append(    "switch(lEthernet_Type)"               EOL);
    aOut->Append(    "{"                                    EOL);

    if (aHost.mFlags.mIPv4)
    {
        aOut->Append("case IPv4_ETHERNET_TYPE:" EOL);
        aOut->Indent_Inc();
        aOut->Append(    "lA = IPv4_Destination( lEthernet );" EOL);

        sprintf_s(lLine, "if ( ( 0x%02x%02x == lA[ 0 ] ) && ( 0x%02x%02x == lA[ 1 ] ) ) { %s = 1; }" EOL, aHost.mData_UInt8[1], aHost.mData_UInt8[0], aHost.mData_UInt8[3], aHost.mData_UInt8[2], aResultName);
        aOut->Append(lLine);

        aOut->Append(    "lA = IPv4_Source( lEthernet );" EOL);

        aOut->Append(lLine);

        aOut->Append(    "break;" EOL);
        aOut->Indent_Dec();
    }

    if (aHost.mFlags.mIPv6)
    {
        aOut->Append("case IPv6_ETHERNET_TYPE:" EOL);
        aOut->Indent_Inc();
        aOut->Append("lA = IPv6_Destination( lEthernet );" EOL);

        sprintf_s(lLine, "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && (0x%04x == lA[ 3 ] ) && ( 0x%04x == lA[ 4 ] ) && ( 0x%04x == lA[ 5 ] ) && ( 0x%04x == lA[ 6 ] ) && (0x%04x == lA[ 7 ] ) ) { %s = 1; }" EOL,
            SwapBytes( aHost.mData_UInt16[0] ), SwapBytes( aHost.mData_UInt16[1]), SwapBytes(aHost.mData_UInt16[2]), SwapBytes(aHost.mData_UInt16[3]), SwapBytes(aHost.mData_UInt16[4]), SwapBytes(aHost.mData_UInt16[5]), SwapBytes(aHost.mData_UInt16[6]), SwapBytes(aHost.mData_UInt16[7]), aResultName);
        aOut->Append(lLine);

        aOut->Append("lA = IPv6_Source( lEthernet );" EOL);

        aOut->Append(lLine);

        aOut->Append("break;" EOL);
        aOut->Indent_Dec();
    }

    aOut->Append("}" EOL);
}

void IpProto(const Token & aIpProto, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIpProto));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    char lLine[1024];

    sprintf_s(lLine, "%s = ( ( IPv4_ETHERNET_TYPE_nh == lEthernetType ) && ( %u == lIP_Protocol ) );" EOL, aResultName, aIpProto.mData_UInt8[0]);
    aOut->Append(lLine);
}

void Ip6Proto(const Token & aIpProto, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIpProto));
    assert(NULL != aResultName);
    assert(NULL != aOut);

    char lLine[1024];

    sprintf_s(lLine, "%s = ( ( IPv6_ETHERNET_TYPE_nh == lEthernetType ) && ( %u == lIP_Protocol ) );" EOL, aResultName, aIpProto.mData_UInt8[0]);
    aOut->Append(lLine);
}

void Less(const Token & aLess, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aLess)   );
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    char lLine[1024];

    sprintf_s(lLine, "%s = ( lPacketInfo->mSize_byte <= %u );" EOL, aResultName, aLess.mData_UInt16[0]);
    aOut->Append(lLine);
}

void Not(const Token & aNot, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aNot)    );
    assert(NULL != aNot.mA    );
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    GenerateCodeForToken(*aNot.mA, aResultName, aOut);
    if (NULL == aOut->GetErrorMessage())
    {
        char lLine[1024];

        sprintf_s(lLine, "%s = ! %s;" EOL, aResultName, aResultName);
        aOut->Append(lLine);
    }
}

void Or(const Token & aAnd, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aAnd));
    assert(NULL != aAnd.mA);
    assert(NULL != aAnd.mB);
    assert(NULL != aResultName);
    assert(NULL != aOut);

    GenerateCodeForToken(*aAnd.mA, aResultName, aOut);
    if (NULL == aOut->GetErrorMessage())
    {
        char lLine[1024];

        sprintf_s(lLine, "if ( ! %s )" EOL, aResultName);
        aOut->Append(lLine);

        aOut->Append(    "{" EOL);
        aOut->Indent_Inc();

        GenerateCodeForToken(*aAnd.mB, aResultName, aOut);
        if (NULL == aOut->GetErrorMessage())
        {
            aOut->Indent_Dec();
            aOut->Append("}" EOL);
        }
    }
}

void Port(const Token & aPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aPort)   );
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    unsigned short lPort_nh = SwapBytes(aPort.mData_UInt16[0]);

    char lLine[1024];

    Begin_IP(aResultName, aOut);

    aOut->Append("case TCP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == TCP_DestinationPort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);

    aOut->Append(    "else" EOL);
    aOut->Append(    "{"    EOL);
    aOut->Indent_Inc();
                                  
    sprintf_s(lLine,     "if ( 0x%04x == TCP_SourcePort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);

    aOut->Indent_Dec();
    aOut->Append(    "}"      EOL);
    aOut->Append(    "break;" EOL);
    aOut->Indent_Dec();
    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == UDP_DestinationPort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);       
                              
    aOut->Append(    "else" EOL);
    aOut->Append(    "{"    EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine,     "if ( 0x%04x == UDP_SourcePort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);

    aOut->Indent_Dec();
    aOut->Append(    "}"      EOL);
    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void PortRange(const Token & aPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aPortRange));
    assert(NULL != aResultName  );
    assert(NULL != aOut         );

    unsigned short lPortA = aPortRange.mData_UInt16[0];
    unsigned short lPortB = aPortRange.mData_UInt16[1];

    char lLine[1024];

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->Append("case TCP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = TCP_DestinationPort( lIP );"      EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    sprintf_s(lLine, "if ( ( 0x%04x <= lPort ) && ( 0x%04x >= lPort ) ) { lR = 1; }" EOL, lPortA, lPortB);
    aOut->Append(lLine);

    aOut->Append(    "else" EOL);
    aOut->Append(    "{"    EOL);
    aOut->Indent_Inc();
    aOut->Append(        "lPort = TCP_SourcePort( lIP );"           EOL);
    aOut->Append(        "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    aOut->Append(lLine);

    aOut->Indent_Dec();
    aOut->Append(    "}"      EOL);
    aOut->Append(    "break;" EOL);
    aOut->Indent_Dec();
    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = UDP_DestinationPort( lIP );"      EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    aOut->Append(lLine);

    aOut->Append(    "else" EOL);
    aOut->Append(    "{"    EOL);
    aOut->Indent_Inc();
    aOut->Append(        "lPort = UDP_SourcePort( lIP );"           EOL);
    aOut->Append(        "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    aOut->Append(lLine);

    aOut->Indent_Dec();
    aOut->Append(    "}"      EOL);
    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void Proto(const Token & aIpProto, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIpProto));
    assert(NULL != aResultName);
    assert(NULL != aOut);

    char lLine[1024];

    sprintf_s(lLine, "%s = ( %u == lIP_Protocol );" EOL, aResultName, aIpProto.mData_UInt8[0]);
    aOut->Append(lLine);
}

void SrcPort(const Token & aSrcPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aSrcPort));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    unsigned short lPort_nh = SwapBytes(aSrcPort.mData_UInt16[0]);

    char lLine[1024];

    Begin_IP(aResultName, aOut);

    aOut->Append("case TCP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == TCP_SourcePort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);

    aOut->Append(    "break;" EOL);
    aOut->Indent_Dec();
    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == UDP_SourcePort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);

    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void SrcPortRange(const Token & aSrcPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aSrcPortRange));
    assert(NULL != aResultName     );
    assert(NULL != aOut            );

    unsigned short lPortA = aSrcPortRange.mData_UInt16[0];
    unsigned short lPortB = aSrcPortRange.mData_UInt16[1];

    char lLine[1024];

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->Append("case TCP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = TCP_SourcePort( lIP );"           EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    sprintf_s(lLine, "if ( ( 0x%04x <= lPort ) && ( 0x%04x >= lPort ) ) { lR = 1; }" EOL, lPortA, lPortB);
    aOut->Append(lLine);

    aOut->Append(    "break;" EOL);
    aOut->Indent_Dec();
    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = UDP_SourcePort( lIP );"           EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    aOut->Append(lLine);

    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void UdpDstPort(const Token & aUdpDstPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpDstPort));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    unsigned short lPort_nh = SwapBytes(aUdpDstPort.mData_UInt16[0]);

    char lLine[1024];

    Begin_IP(aResultName, aOut);

    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == UDP_DestinationPort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);       
                               
    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void UdpDstPortRange(const Token & aUdpDstPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpDstPortRange));
    assert(NULL != aResultName        );
    assert(NULL != aOut               );

    unsigned short lPortA = aUdpDstPortRange.mData_UInt16[0];
    unsigned short lPortB = aUdpDstPortRange.mData_UInt16[1];

    char lLine[1024];

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = UDP_DestinationPort( lIP );"      EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    sprintf_s(lLine, "if ( ( 0x%04x <= lPort ) && ( 0x%04x >= lPort ) ) { lR = 1; }" EOL, lPortA, lPortB);
    aOut->Append(lLine);

    aOut->Append("break;" EOL);

    End_IP(aOut);
}

void UdpPort(const Token & aUdpPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpPort));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    unsigned short lPort_nh = SwapBytes(aUdpPort.mData_UInt16[0]);

    char lLine[1024];

    Begin_IP(aResultName, aOut);

    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == UDP_DestinationPort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);       
                              
    aOut->Append(    "else" EOL);
    aOut->Append(    "{"    EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine,     "if ( 0x%04x == UDP_SourcePort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);

    aOut->Indent_Dec();
    aOut->Append(    "}"      EOL);
    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void UdpPortRange(const Token & aUdpPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpPortRange));
    assert(NULL != aResultName     );
    assert(NULL != aOut            );

    unsigned short lPortA = aUdpPortRange.mData_UInt16[0];
    unsigned short lPortB = aUdpPortRange.mData_UInt16[1];

    char lLine[1024];

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = UDP_DestinationPort( lIP );"      EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    sprintf_s(lLine, "if ( ( 0x%04x <= lPort ) && ( 0x%04x >= lPort ) ) { lR = 1; }" EOL, lPortA, lPortB);
    aOut->Append(lLine);

    aOut->Append(    "else" EOL);
    aOut->Append(    "{"    EOL);
    aOut->Indent_Inc();
    aOut->Append(        "lPort = UDP_SourcePort( lIP );"           EOL);
    aOut->Append(        "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    aOut->Append(lLine);

    aOut->Indent_Dec();
    aOut->Append(    "}"      EOL);
    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void UdpSrcPort(const Token & aUdpSrcPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpSrcPort));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    unsigned short lPort_nh = SwapBytes(aUdpSrcPort.mData_UInt16[0]);

    char lLine[1024];

    Begin_IP(aResultName, aOut);

    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == UDP_SourcePort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);

    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void UdpSrcPortRange(const Token & aUdpSrcPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpSrcPortRange));
    assert(NULL != aResultName        );
    assert(NULL != aOut               );

    unsigned short lPortA = aUdpSrcPortRange.mData_UInt16[0];
    unsigned short lPortB = aUdpSrcPortRange.mData_UInt16[1];

    char lLine[1024];

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->Append("case UDP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = UDP_SourcePort( lIP );"           EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    sprintf_s(lLine, "if ( ( 0x%04x <= lPort ) && ( 0x%04x >= lPort ) ) { lR = 1; }" EOL, lPortA, lPortB);
    aOut->Append(lLine);

    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void TcpDstPort(const Token & aTcpDstPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpDstPort));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    unsigned short lPort_nh = SwapBytes(aTcpDstPort.mData_UInt16[0]);

    char lLine[1024];

    Begin_IP(aResultName, aOut);

    aOut->Append("case TCP_IP_PROTOCOL :"  EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == TCP_DestinationPort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);          
                                  
    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void TcpDstPortRange(const Token & aTcpDstPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpDstPortRange));
    assert(NULL != aResultName        );
    assert(NULL != aOut               );

    unsigned short lPortA = aTcpDstPortRange.mData_UInt16[0];
    unsigned short lPortB = aTcpDstPortRange.mData_UInt16[1];

    char lLine[1024];

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->Append("case TCP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = TCP_DestinationPort( lIP );"      EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    sprintf_s(lLine, "if ( ( 0x%04x <= lPort ) && ( 0x%04x >= lPort ) ) { lR = 1; }" EOL, lPortA, lPortB);
    aOut->Append(lLine);

    aOut->Append(    "break;" EOL);

    End_IP(aOut);

}

void TcpPort(const Token & aTcpPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpPort));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    unsigned short lPort_nh = SwapBytes(aTcpPort.mData_UInt16[0]);

    char lLine[1024];

    Begin_IP(aResultName, aOut);

    aOut->Append("case TCP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == TCP_DestinationPort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);

    aOut->Append(    "else" EOL);
    aOut->Append(    "{"    EOL);
    aOut->Indent_Inc();
                                  
    sprintf_s(lLine,     "if ( 0x%04x == TCP_SourcePort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);

    aOut->Indent_Dec();
    aOut->Append(    "}"      EOL);
    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void TcpPortRange(const Token & aTcpPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpPortRange));
    assert(NULL != aResultName     );
    assert(NULL != aOut            );

    unsigned short lPortA = aTcpPortRange.mData_UInt16[0];
    unsigned short lPortB = aTcpPortRange.mData_UInt16[1];

    char lLine[1024];

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->Append("case TCP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = TCP_DestinationPort( lIP );"      EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    sprintf_s(lLine, "if ( ( 0x%04x <= lPort ) && ( 0x%04x >= lPort ) ) { lR = 1; }" EOL, lPortA, lPortB);
    aOut->Append(lLine);

    aOut->Append(    "else" EOL);
    aOut->Append(    "{"    EOL);
    aOut->Indent_Inc();
    aOut->Append(        "lPort = TCP_SourcePort( lIP );"           EOL);
    aOut->Append(        "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    aOut->Append(lLine);

    aOut->Indent_Dec();
    aOut->Append(    "}"      EOL);
    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void TcpSrcPort(const Token & aTcpSrcPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpSrcPort));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    unsigned short lPort_nh = SwapBytes(aTcpSrcPort.mData_UInt16[0]);

    char lLine[1024];

    Begin_IP(aResultName, aOut);

    aOut->Append("case TCP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();

    sprintf_s(lLine, "if ( 0x%04x == TCP_SourcePort( lIP ) ) { lR = 1; }" EOL, lPort_nh);
    aOut->Append(lLine);

    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}

void TcpSrcPortRange(const Token & aTcpSrcPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpSrcPortRange));
    assert(NULL != aResultName        );
    assert(NULL != aOut               );

    unsigned short lPortA = aTcpSrcPortRange.mData_UInt16[0];
    unsigned short lPortB = aTcpSrcPortRange.mData_UInt16[1];

    char lLine[1024];

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->Append("case TCP_IP_PROTOCOL :" EOL);
    aOut->Indent_Inc();
    aOut->Append(    "lPort = TCP_SourcePort( lIP );"           EOL);
    aOut->Append(    "lPort = ( lPort >> 8 ) | ( lPort << 8 );" EOL);

    sprintf_s(lLine, "if ( ( 0x%04x <= lPort ) && ( 0x%04x >= lPort ) ) { lR = 1; }" EOL, lPortA, lPortB);
    aOut->Append(lLine);

    aOut->Append(    "break;" EOL);

    End_IP(aOut);
}
