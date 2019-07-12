
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Phase3.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== CodeGen ============================================================
#include "Filter_Internal.h"
#include "SearchList.h"
#include "Token.h"

#include "Phase3.h"

// Data types
/////////////////////////////////////////////////////////////////////////////

typedef void(*Gen0Function)(const Token &, const char *, Filter_Internal *);
typedef void(*Gen1Function)(const Token &,               Filter_Internal *);

typedef struct
{
    TokenId      mId        ;
    Gen0Function mFunction  ;
    bool         mInitResult;
}
Gen0;

typedef struct
{
    TokenId      mId      ;
    Gen1Function mFunction;
}
Gen1;

typedef struct
{
    TokenId        mId          ;
    TokenId        mRealId      ;
    unsigned char  mIpProtocol  ;
    unsigned short mEthernetType;
}
Abbreviation;

// Static function declaration
/////////////////////////////////////////////////////////////////////////////

static void Begin_IP(const char * aResultName, Filter_Internal * aOut, const char * aLocalVar = NULL);

static void End_IP(Filter_Internal * aOut);

static void GenerateCodeForToken(const Token & aToken, const char * aResultName, Filter_Internal * aOut);
static void GenerateCodeForToken(const Token & aToken,                           Filter_Internal * aOut);

static void EthernetAddress(Filter_Internal * aOut, const char * aResultName, const char * aValue, const Token & aToken);

static void In(const Token &, const char * aResultName, Filter_Internal * aOut);

static void InitResult(const char * aResultName, Filter_Internal * aOut);

static void IPv4_Address(const char * aResultName, Filter_Internal * aOut, const char * aValue, const Token & aToken);
static void IPv4_Network(const char * aResultName, Filter_Internal * aOut, const char * aValue, const Token & aToken);
static void IPv6_Address(const char * aResultName, Filter_Internal * aOut, const char * aValue, const Token & aToken);
static void IPv6_Network(const char * aResultName, Filter_Internal * aOut, const char * aValue, const Token & aToken);

static void OneLineIf(Filter_Internal * aOut, const char * aResultName, const char * aCondition);

static void Operator(const Token & aToken, const char * aResultName, Filter_Internal * aOut, const char * aOperator);
static void Operator(const Token & aToken,                           Filter_Internal * aOut, const char * aOperator);

static void Port     (Filter_Internal * aOut, const char * aResultName, const char * aValue, const Token & aToken);
static void PortRange(Filter_Internal * aOut, const char * aResultName, const char * aValue, const Token & aToken);

static void SpecialElse(Filter_Internal * aOut);

static unsigned short SwapBytes(unsigned short aIn);

// ===== Gen0 ===============================================================
static void And            (const Token &, const char * aResultName, Filter_Internal * aOut);
static void ArpDstHost     (const Token &, const char * aResultName, Filter_Internal * aOut);
static void ArpHost        (const Token &, const char * aResultName, Filter_Internal * aOut);
static void ArpSrcHost     (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Broadcast      (const Token &, const char * aResultName, Filter_Internal * aOut);
static void DstHost        (const Token &, const char * aResultName, Filter_Internal * aOut);
static void DstNet         (const Token &, const char * aResultName, Filter_Internal * aOut);
static void DstPort        (const Token &, const char * aResultName, Filter_Internal * aOut);
static void DstPortRange   (const Token &, const char * aResultName, Filter_Internal * aOut);
static void EtherDst       (const Token &, const char * aResultName, Filter_Internal * aOut);
static void EtherHost      (const Token &, const char * aResultName, Filter_Internal * aOut);
static void EtherProto     (const Token &, const char * aResultName, Filter_Internal * aOut);
static void EtherSrc       (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Greater        (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Host           (const Token &, const char * aResultName, Filter_Internal * aOut);
static void InEthernet     (const Token &, const char * aResultName, Filter_Internal * aOut);
static void InIp           (const Token &, const char * aResultName, Filter_Internal * aOut);
static void InUdp          (const Token &, const char * aResultName, Filter_Internal * aOut);
static void InTcp          (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Inbound        (const Token &, const char * aResultName, Filter_Internal * aOut);
static void IpDstHost      (const Token &, const char * aResultName, Filter_Internal * aOut);
static void IpHost         (const Token &, const char * aResultName, Filter_Internal * aOut);
static void IpMulticast    (const Token &, const char * aResultName, Filter_Internal * aOut);
static void IpNet          (const Token &, const char * aResultName, Filter_Internal * aOut);
static void IpProto        (const Token &, const char * aResultName, Filter_Internal * aOut);
static void IpSrcHost      (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Ip6DstHost     (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Ip6Host        (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Ip6Multicast   (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Ip6Net         (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Ip6Proto       (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Ip6SrcHost     (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Less           (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Multicast      (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Net            (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Not            (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Or             (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Operator       (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Port           (const Token &, const char * aResultName, Filter_Internal * aOut);
static void PortRange      (const Token &, const char * aResultName, Filter_Internal * aOut);
static void Proto          (const Token &, const char * aResultName, Filter_Internal * aOut);
static void SrcHost        (const Token &, const char * aResultName, Filter_Internal * aOut);
static void SrcNet         (const Token &, const char * aResultName, Filter_Internal * aOut);
static void SrcPort        (const Token &, const char * aResultName, Filter_Internal * aOut);
static void SrcPortRange   (const Token &, const char * aResultName, Filter_Internal * aOut);
static void TcpDstPort     (const Token &, const char * aResultName, Filter_Internal * aOut);
static void TcpDstPortRange(const Token &, const char * aResultName, Filter_Internal * aOut);
static void TcpPort        (const Token &, const char * aResultName, Filter_Internal * aOut);
static void TcpPortRange   (const Token &, const char * aResultName, Filter_Internal * aOut);
static void TcpSrcPort     (const Token &, const char * aResultName, Filter_Internal * aOut);
static void TcpSrcPortRange(const Token &, const char * aResultName, Filter_Internal * aOut);
static void UdpDstPort     (const Token &, const char * aResultName, Filter_Internal * aOut);
static void UdpDstPortRange(const Token &, const char * aResultName, Filter_Internal * aOut);
static void UdpPort        (const Token &, const char * aResultName, Filter_Internal * aOut);
static void UdpPortRange   (const Token &, const char * aResultName, Filter_Internal * aOut);
static void UdpSrcPort     (const Token &, const char * aResultName, Filter_Internal * aOut);
static void UdpSrcPortRange(const Token &, const char * aResultName, Filter_Internal * aOut);
static void Vlan           (const Token &, const char * aResultName, Filter_Internal * aOut);

// ===== Gen1 ===============================================================
static void Add       (const Token &, Filter_Internal * aOut);
static void BitAnd    (const Token &, Filter_Internal * aOut);
static void BitOr     (const Token &, Filter_Internal * aOut);
static void BitXor    (const Token &, Filter_Internal * aOut);
static void Dash      (const Token &, Filter_Internal * aOut);
static void Data      (const Token &, Filter_Internal * aOut);
static void Div       (const Token &, Filter_Internal * aOut);
static void EtherIndex(const Token &, Filter_Internal * aOut);
static void Len       (const Token &, Filter_Internal * aOut);
static void Mod       (const Token &, Filter_Internal * aOut);
static void Mul       (const Token &, Filter_Internal * aOut);
static void Operator  (const Token &, Filter_Internal * aOut);
static void ShiftLeft (const Token &, Filter_Internal * aOut);
static void ShiftRight(const Token &, Filter_Internal * aOut);

// Constants
/////////////////////////////////////////////////////////////////////////////

static const Gen0 GEN0[]
{
    { TOKEN_AND              , And             , false },
    { TOKEN_ARP_DST_HOST     , ArpDstHost      , true  },
    { TOKEN_ARP_HOST         , ArpHost         , true  },
    { TOKEN_ARP_SRC_HOST     , ArpSrcHost      , true  },
    { TOKEN_BROADCAST        , Broadcast       , true  },
    { TOKEN_DST_HOST         , DstHost         , true  },
    { TOKEN_DST_NET          , DstNet          , true  },
    { TOKEN_DST_PORT         , DstPort         , true  },
    { TOKEN_DST_PORTRANGE    , DstPortRange    , true  },
    { TOKEN_ETHER_DST        , EtherDst        , true  },
    { TOKEN_ETHER_HOST       , EtherHost       , true  },
    { TOKEN_ETHER_PROTO      , EtherProto      , false },
    { TOKEN_ETHER_SRC        , EtherSrc        , true  },
    { TOKEN_GREATER          , Greater         , false },
    { TOKEN_HOST             , Host            , true  },
    { TOKEN_IN_ETHERNET      , InEthernet      , true  },
    { TOKEN_IN_IP            , InIp            , true  },
    { TOKEN_IN_TCP           , InTcp           , true  },
    { TOKEN_IN_UDP           , InUdp           , true  },
    { TOKEN_INBOUND          , Inbound         , false },
    { TOKEN_IP_DST_HOST      , IpDstHost       , true  },
    { TOKEN_IP_HOST          , IpHost          , true  },
    { TOKEN_IP_MULTICAST     , IpMulticast     , true  },
    { TOKEN_IP_NET           , IpNet           , true  },
    { TOKEN_IP_PROTO         , IpProto         , false },
    { TOKEN_IP_SRC_HOST      , IpSrcHost       , true  },
    { TOKEN_IP6_DST_HOST     , Ip6DstHost      , true  },
    { TOKEN_IP6_HOST         , Ip6Host         , true  },
    { TOKEN_IP6_MULTICAST    , Ip6Multicast    , true  },
    { TOKEN_IP6_PROTO        , Ip6Proto        , false },
    { TOKEN_IP6_NET          , Ip6Net          , true  },
    { TOKEN_IP6_SRC_HOST     , Ip6SrcHost      , true  },
    { TOKEN_LESS             , Less            , false },
    { TOKEN_MULTICAST        , Multicast       , true  },
    { TOKEN_NET              , Net             , true  },
    { TOKEN_NOT              , Not             , false },
    { TOKEN_OR               , Or              , true  },
    { TOKEN_PORT             , Port            , true  },
    { TOKEN_PORTRANGE        , PortRange       , true  },
    { TOKEN_PROTO            , Proto           , false },
    { TOKEN_SRC_HOST         , SrcHost         , true  },
    { TOKEN_SRC_NET          , SrcNet          , true  },
    { TOKEN_SRC_PORT         , SrcPort         , true  },
    { TOKEN_SRC_PORTRANGE    , SrcPortRange    , true  },
    { TOKEN_TCP_DST_PORT     , TcpDstPort      , true  },
    { TOKEN_TCP_DST_PORTRANGE, TcpDstPortRange , true  },
    { TOKEN_TCP_PORT         , TcpPort         , true  },
    { TOKEN_TCP_PORTRANGE    , TcpPortRange    , true  },
    { TOKEN_TCP_SRC_PORT     , TcpSrcPort      , true  },
    { TOKEN_TCP_SRC_PORTRANGE, TcpSrcPortRange , true  },
    { TOKEN_UDP_DST_PORT     , UdpDstPort      , true  },
    { TOKEN_UDP_DST_PORTRANGE, UdpDstPortRange , true  },
    { TOKEN_UDP_PORT         , UdpPort         , true  },
    { TOKEN_UDP_PORTRANGE    , UdpPortRange    , true  },
    { TOKEN_UDP_SRC_PORT     , UdpSrcPort      , true  },
    { TOKEN_UDP_SRC_PORTRANGE, UdpSrcPortRange , true  },
    { TOKEN_VLAN             , Vlan            , false },

    { TOKEN_OPERATOR, Operator},
};

#define GEN0_COUNT ( sizeof( GEN0 ) / sizeof( GEN0[ 0 ] ) )

static const Gen1 GEN1[]
{
    { TOKEN_DATA       , Data       },
    { TOKEN_ETHER_INDEX, EtherIndex },
    { TOKEN_LEN        , Len        },

    { TOKEN_OPERATOR, Operator },
};

#define GEN1_COUNT ( sizeof( GEN1 ) / sizeof( GEN1[ 0 ] ) )

static const Abbreviation ABBREVIATION[]
{
    { TOKEN_ARP , TOKEN_ETHER_PROTO,  0, 0x0806 },
    { TOKEN_ICMP, TOKEN_PROTO      ,  1,      0 },
    { TOKEN_IP  , TOKEN_ETHER_PROTO,  0, 0x0800 },
    { TOKEN_IP6 , TOKEN_ETHER_PROTO,  0, 0x86dd },
    { TOKEN_TCP , TOKEN_PROTO      ,  6,      0 },
    { TOKEN_UDP , TOKEN_PROTO      , 17,      0 },
};

#define ABBREVIATION_COUNT ( sizeof( ABBREVIATION ) / sizeof( ABBREVIATION[ 0 ] ) )

static const unsigned short MASK[16] =
{
    0x0000,
    0x0080,
    0x00c0,
    0x00e0,
    0x00f0,
    0x00f8,
    0x00fc,
    0x00fe,
    0x00ff,
    0x80ff,
    0xc0ff,
    0xe0ff,
    0xf0ff,
    0xf8ff,
    0xfcff,
    0xfeff,
};

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

    aOut->C_if("0 != lIP");

        if (NULL != aLocalVar)
        {
            aOut->Append(aLocalVar);
            aOut->Append(EOL);
        }

        aOut->C_switch("lIP_Protocol");
}

void End_IP(Filter_Internal * aOut)
{
    assert(NULL != aOut);

        aOut->C_switch_End();
    aOut->C_if_End();
    aOut->Append(EOL);
}

void GenerateCodeForToken(const Token & aToken, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aToken)  );
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    if (aToken.mFlags.mBoolean)
    {
        for (unsigned int i = 0; i < GEN0_COUNT; i++)
        {
            const Gen0 & lGen = GEN0[i];

            if (aToken.Is(lGen.mId))
            {
                if (lGen.mInitResult)
                {
                    InitResult(aResultName, aOut);
                }

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

                if (0 != lAbb.mEthernetType) { lT.SetEthernetType(lAbb.mEthernetType); }
                if (0 != lAbb.mIpProtocol  ) { lT.SetIpProtocol  (lAbb.mIpProtocol  ); }

                lT.mId = lAbb.mRealId;

                lT.SetBoolean();

                GenerateCodeForToken(lT, aResultName, aOut);
                return;
            }
        }

        aOut->SetErrorMessage("ERROR 3.2 - Invalid filter");
    }
}

void GenerateCodeForToken(const Token & aToken, Filter_Internal * aOut)
{
    assert(NULL != (&aToken));
    assert(NULL != aOut     );

    for (unsigned int i = 0; i < GEN1_COUNT; i++)
    {
        const Gen1 & lGen = GEN1[i];

        if (aToken.Is(lGen.mId))
        {
            lGen.mFunction(aToken, aOut);
            return;
        }
    }

    aOut->SetErrorMessage("ERROR 3.3 - Invalid filter");
}

void EthernetAddress(Filter_Internal * aOut, const char * aResultName, const char * aValue, const Token & aToken)
{
    assert(NULL != aResultName);
    assert(NULL != aOut       );
    assert(NULL != aValue     );
    assert(NULL != (&aToken)  );

    char lLine[1024];

    sprintf_s(lLine, "lA = %s;" EOL, aValue);
    aOut->Append(lLine);

    sprintf_s(lLine,
        "if ( ( 0x%02x%02x == lA[ 0 ] ) && ( 0x%02x%02x == lA[ 1 ] ) && ( 0x%02x%02x == lA[ 2 ] ) ) { %s = 1; }" EOL,
        aToken.mData.mEthernet[1], aToken.mData.mEthernet[0],
        aToken.mData.mEthernet[3], aToken.mData.mEthernet[2],
        aToken.mData.mEthernet[5], aToken.mData.mEthernet[4], aResultName);
    aOut->Append(lLine);
}

void In(const Token & aIn, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIn)     );
    assert(NULL != aResultName);
    assert(NULL != aOut       );
    SearchList lSL;

    const char * lErrMsg = lSL.Init(aIn.mText, aIn.mTextLen);
    if (NULL == lErrMsg)
    {
        lErrMsg = lSL.Generate(aResultName, aOut);
    }

    if (NULL != lErrMsg)
    {
        aOut->SetErrorMessage(lErrMsg);
    }
}

void InitResult(const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    char lLine[1024];

    sprintf_s(lLine, "%s = 0;" EOL, aResultName);
    aOut->Append(lLine);
}

void IPv4_Address(const char * aResultName, Filter_Internal * aOut, const char * aValue, const Token & aToken)
{
    assert(NULL != aResultName);
    assert(NULL != aOut       );
    assert(NULL != aValue     );
    assert(NULL != (&aToken)  );

    char lLine[1024];

    sprintf_s(lLine, "lA = %s;" EOL, aValue);
    aOut->Append(lLine);

    sprintf_s(lLine,
        "if ( ( 0x%02x%02x == lA[ 0 ] ) && ( 0x%02x%02x == lA[ 1 ] ) ) { %s = 1; }" EOL,
        aToken.mData.mIPv4[1], aToken.mData.mIPv4[0],
        aToken.mData.mIPv4[3], aToken.mData.mIPv4[2], aResultName);
    aOut->Append(lLine);
}

void IPv4_Network(const char * aResultName, Filter_Internal * aOut, const char * aValue, const Token & aToken)
{
    assert(NULL != aResultName);
    assert(NULL != aOut);
    assert(NULL != aValue);
    assert(NULL != (&aToken));

    char lLine[1024];

    sprintf_s(lLine, "lA = %s;" EOL, aValue);
    aOut->Append(lLine);

    unsigned short lMask = MASK[aToken.mData.mNetMaskLen % 16];

    if (32 == aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%02x%02x == lA[ 0 ] ) && ( 0x%02x%02x == lA[ 1 ] ) ) { %s = 1; }" EOL,
            aToken.mData.mIPv4[1], aToken.mData.mIPv4[0],
            aToken.mData.mIPv4[3], aToken.mData.mIPv4[2], aResultName);
    }
    else if (16 < aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%02x%02x == lA[ 0 ] ) && ( ( 0x%02x%02x & 0x%04x ) == ( lA[ 1 ] & 0x%04x ) ) ) { %s = 1; }" EOL,
            aToken.mData.mIPv4[1], aToken.mData.mIPv4[0],
            aToken.mData.mIPv4[3], aToken.mData.mIPv4[2], lMask, lMask, aResultName);

    }
    else if (16 == aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( 0x%02x%02x == lA[ 0 ] ) { %s = 1; }" EOL,
            aToken.mData.mIPv4[1], aToken.mData.mIPv4[0], aResultName);
    }
    else
    {
        sprintf_s(lLine,
            "if ( ( 0x%02x%02x & 0x%04x ) == ( lA[ 0 ] & 0x%04x ) ) { %s = 1; }" EOL,
            aToken.mData.mIPv4[1], aToken.mData.mIPv4[0], lMask, lMask, aResultName);
    }

    aOut->Append(lLine);
}

void IPv6_Address(const char * aResultName, Filter_Internal * aOut, const char * aValue, const Token & aToken)
{
    assert(NULL != aResultName);
    assert(NULL != aOut       );
    assert(NULL != aValue     );
    assert(NULL != (&aToken)  );

    char lLine[1024];

    sprintf_s(lLine, "lA = %s;" EOL, aValue);
    aOut->Append(lLine);

    sprintf_s(lLine,
        "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && (0x%04x == lA[ 3 ] ) && ( 0x%04x == lA[ 4 ] ) && ( 0x%04x == lA[ 5 ] ) && ( 0x%04x == lA[ 6 ] ) && (0x%04x == lA[ 7 ] ) ) { %s = 1; }" EOL,
        SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), SwapBytes(aToken.mData.mIPv6[3]),
        SwapBytes(aToken.mData.mIPv6[4]), SwapBytes(aToken.mData.mIPv6[5]), SwapBytes(aToken.mData.mIPv6[6]), SwapBytes(aToken.mData.mIPv6[7]), aResultName);
    aOut->Append(lLine);
}

void IPv6_Network(const char * aResultName, Filter_Internal * aOut, const char * aValue, const Token & aToken)
{
    assert(NULL != aResultName);
    assert(NULL != aOut       );
    assert(NULL != aValue     );
    assert(NULL != (&aToken  ));

    char lLine[1024];

    sprintf_s(lLine, "lA = %s;" EOL, aValue);
    aOut->Append(lLine);

    unsigned short lMask = MASK[aToken.mData.mNetMaskLen % 16];

    if (128 == aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && ( 0x%04x == lA[ 3 ] ) && ( 0x%04x == lA[ 4 ] ) && ( 0x%04x == lA[ 5 ] ) && ( 0x%04x == lA[ 6 ] ) && ( 0x%04x == lA[ 7 ] ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), SwapBytes(aToken.mData.mIPv6[3]),
            SwapBytes(aToken.mData.mIPv6[4]), SwapBytes(aToken.mData.mIPv6[5]), SwapBytes(aToken.mData.mIPv6[6]), SwapBytes(aToken.mData.mIPv6[7]), aResultName);
    }
    else if (112 < aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && ( 0x%04x == lA[ 3 ] ) && ( 0x%04x == lA[ 4 ] ) && ( 0x%04x == lA[ 5 ] ) && ( 0x%04x == lA[ 6 ] ) && ( 0x%04x == ( lA[ 7 ] & 0x%04x ) ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), SwapBytes(aToken.mData.mIPv6[3]),
            SwapBytes(aToken.mData.mIPv6[4]), SwapBytes(aToken.mData.mIPv6[5]), SwapBytes(aToken.mData.mIPv6[6]), SwapBytes(aToken.mData.mIPv6[7]) & lMask, lMask, aResultName);
    }
    else if (112 == aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && ( 0x%04x == lA[ 3 ] ) && ( 0x%04x == lA[ 4 ] ) && ( 0x%04x == lA[ 5 ] ) && ( 0x%04x == lA[ 6 ] ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), SwapBytes(aToken.mData.mIPv6[3]),
            SwapBytes(aToken.mData.mIPv6[4]), SwapBytes(aToken.mData.mIPv6[5]), SwapBytes(aToken.mData.mIPv6[6]), aResultName);
    }
    else if (96 < aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && ( 0x%04x == lA[ 3 ] ) && ( 0x%04x == lA[ 4 ] ) && ( 0x%04x == lA[ 5 ] ) && (0x%04x == ( lA[ 6 ] & 0x%04x ) ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), SwapBytes(aToken.mData.mIPv6[3]),
            SwapBytes(aToken.mData.mIPv6[4]), SwapBytes(aToken.mData.mIPv6[5]), SwapBytes(aToken.mData.mIPv6[6]) & lMask, lMask, aResultName);
    }
    else if (96 == aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && ( 0x%04x == lA[ 3 ] ) && ( 0x%04x == lA[ 4 ] ) && ( 0x%04x == lA[ 5 ] ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), SwapBytes(aToken.mData.mIPv6[3]),
            SwapBytes(aToken.mData.mIPv6[4]), SwapBytes(aToken.mData.mIPv6[5]), aResultName);
    }
    else if (80 < aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && ( 0x%04x == lA[ 3 ] ) && ( 0x%04x == lA[ 4 ] ) && (0x%04x == ( lA[ 5 ] & 0x%04x ) ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), SwapBytes(aToken.mData.mIPv6[3]),
            SwapBytes(aToken.mData.mIPv6[4]), SwapBytes(aToken.mData.mIPv6[5]) & lMask, lMask, aResultName);
    }
    else if (80 == aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && ( 0x%04x == lA[ 3 ] ) && ( 0x%04x == lA[ 4 ] ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), SwapBytes(aToken.mData.mIPv6[3]),
            SwapBytes(aToken.mData.mIPv6[4]), aResultName);
    }
    else if (64 < aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && ( 0x%04x == lA[ 3 ] ) && (0x%04x == ( lA[ 4 ] & 0x%04x ) ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), SwapBytes(aToken.mData.mIPv6[3]),
            SwapBytes(aToken.mData.mIPv6[4]) & lMask, lMask, aResultName);
    }
    else if (64 == aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && ( 0x%04x == lA[ 3 ] ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), SwapBytes(aToken.mData.mIPv6[3]),
            aResultName);
    }
    else if (48 < aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) && ( 0x%04x == ( lA[ 3 ] & 0x%04x ) ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), SwapBytes(aToken.mData.mIPv6[3]) & lMask,
            lMask, aResultName);
    }
    else if (48 == aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == lA[ 2 ] ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]), aResultName);
    }
    else if (32 < aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) && ( 0x%04x == ( lA[ 2 ] & 0x%04x ) ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), SwapBytes(aToken.mData.mIPv6[2]) & lMask, lMask, aResultName);
    }
    else if (32 == aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == lA[ 1 ] ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]), aResultName);
    }
    else if (16 < aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( ( 0x%04x == lA[ 0 ] ) && ( 0x%04x == ( lA[ 1 ] & 0x%04x ) ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), SwapBytes(aToken.mData.mIPv6[1]) & lMask, lMask, aResultName);
    }
    else if (16 == aToken.mData.mNetMaskLen)
    {
        sprintf_s(lLine,
            "if ( 0x%04x == lA[ 0 ] ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]), aResultName);
    }
    else
    {
        sprintf_s(lLine,
            "if ( 0x%04x == ( lA[ 0 ] & 0x%04x ) ) { %s = 1; }" EOL,
            SwapBytes(aToken.mData.mIPv6[0]) & lMask, lMask, aResultName);
    }

    aOut->Append(lLine);
}

void OneLineIf(Filter_Internal * aOut, const char * aResultName, const char * aCondition)
{
    assert(NULL != aOut       );
    assert(NULL != aResultName);
    assert(NULL != aCondition );

    char lLine[1024];

    sprintf_s(lLine, "if ( %s ) { %s = 1; }" EOL, aCondition, aResultName);
    aOut->Append(lLine);
}

void Operator(const Token & aToken, const char * aResultName, Filter_Internal * aOut, const char * aOperator)
{
    assert(NULL != (&aToken  ));
    assert(NULL != aToken.mA  );
    assert(NULL != aToken.mB  );
    assert(NULL != aResultName);
    assert(NULL != aOut       );
    assert(NULL != aOperator  );

    aOut->Append(aResultName);
    aOut->AppendCode(" = ( ");
    GenerateCodeForToken(*aToken.mA, aOut);
    aOut->AppendCode(" ) ");
    aOut->AppendCode(aOperator);
    aOut->AppendCode(" ( ");
    GenerateCodeForToken(*aToken.mB, aOut);
    aOut->AppendCode(" );" EOL);
}

void Operator(const Token & aToken, Filter_Internal * aOut, const char * aOperator)
{
    assert(NULL != (&aToken));
    assert(NULL != aToken.mA);
    assert(NULL != aToken.mB);
    assert(NULL != aOut     );
    assert(NULL != aOperator);

    aOut->AppendCode("( ");
    GenerateCodeForToken(*aToken.mA, aOut);
    aOut->AppendCode(" ) ");
    aOut->AppendCode(aOperator);
    aOut->AppendCode(" ( ");
    GenerateCodeForToken(*aToken.mB, aOut);
    aOut->AppendCode(" )");
}

void Port(Filter_Internal * aOut, const char * aResultName, const char * aValue, const Token & aToken)
{
    assert(NULL != aOut       );
    assert(NULL != aResultName);
    assert(NULL != (&aToken)  );

    unsigned short lPort_nh = SwapBytes(aToken.mData.mPorts[0]);

    char lLine[1024];

    sprintf_s(lLine, "if ( 0x%04x == %s ) { %s = 1; }" EOL, lPort_nh, aValue, aResultName);
    aOut->Append(lLine);
}

void PortRange(Filter_Internal * aOut, const char * aResultName, const char * aValue, const Token & aToken)
{
    assert(NULL != aOut       );
    assert(NULL != aResultName);
    assert(NULL != (&aToken)  );

    unsigned short lPortA = aToken.mData.mPorts[0];
    unsigned short lPortB = aToken.mData.mPorts[1];

    char lLine[1024];

    sprintf_s(lLine, "lPort = SwapUInt16( %s );" EOL, aValue);
    aOut->Append(lLine);

    sprintf_s(lLine, "if ( ( 0x%04x <= lPort ) && ( 0x%04x >= lPort ) ) { lR = 1; }" EOL, lPortA, lPortB);
    aOut->Append(lLine);
}

void SpecialElse(Filter_Internal * aOut)
{
    assert(NULL != aOut);

    aOut->Append("else" EOL);
    aOut->Append("{"    EOL);
    aOut->Indent_Inc();
}

unsigned short SwapBytes(unsigned short aIn)
{
    return ((aIn >> 8) | (aIn << 8));
}

// ===== Gen0 ===============================================================

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
        aOut->C_if(aResultName);

        GenerateCodeForToken(*aAnd.mB, aResultName, aOut);
        if (NULL == aOut->GetErrorMessage())
        {
            aOut->C_if_End();
        }
    }
}

void ArpDstHost(const Token & aArpDstHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aArpDstHost));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("ARP_ETHERNET_TYPE_nh");
            aOut->C_switch("ARP_Protocol( lEthernet)");

                if (aArpDstHost.mData.mFlags.mIPv4)
                {
                    aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                        IPv4_Address(aResultName, aOut, "ARP_Destination( lEthernet )", aArpDstHost);
                    aOut->C_break();
                }

                if (aArpDstHost.mData.mFlags.mIPv6)
                {
                    aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                        IPv6_Address(aResultName, aOut, "ARP_Destination( lEthernet )", aArpDstHost);
                    aOut->C_break();
                }

            aOut->C_switch_End();
        aOut->C_break();
    aOut->C_switch_End();
}

void ArpHost(const Token & aArpHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aArpHost));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("ARP_ETHERNET_TYPE_nh");
            aOut->C_switch("ARP_Protocol( lEthernet)");

                if (aArpHost.mData.mFlags.mIPv4)
                {
                    aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                        IPv4_Address(aResultName, aOut, "ARP_Destination( lEthernet )", aArpHost);
                        SpecialElse(aOut);
                            IPv4_Address(aResultName, aOut, "ARP_Source( lEthernet )", aArpHost);
                        aOut->C_if_End();
                    aOut->C_break();
                }

                if (aArpHost.mData.mFlags.mIPv6)
                {
                    aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                        IPv6_Address(aResultName, aOut, "ARP_Destination( lEthernet )", aArpHost);
                        SpecialElse(aOut);
                            IPv6_Address(aResultName, aOut, "ARP_Source( lEthernet )", aArpHost);
                        aOut->C_if_End();
                    aOut->C_break();
                }

            aOut->C_switch_End();
        aOut->C_break();
    aOut->C_switch_End();
}

void ArpSrcHost(const Token & aArpSrcHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aArpSrcHost));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("ARP_ETHERNET_TYPE_nh");
            aOut->C_switch("ARP_Protocol( lEthernet)");

                if (aArpSrcHost.mData.mFlags.mIPv4)
                {
                    aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                        IPv4_Address(aResultName, aOut, "ARP_Source( lEthernet )", aArpSrcHost);
                    aOut->C_break();
                }

                if (aArpSrcHost.mData.mFlags.mIPv6)
                {
                    aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                        IPv6_Address(aResultName, aOut, "ARP_Source( lEthernet )", aArpSrcHost);
                    aOut->C_break();
                }

            aOut->C_switch_End();
        aOut->C_break();
    aOut->C_switch_End();
}

void Broadcast(const Token & aBroadcast, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aBroadcast));
    assert(NULL != aResultName  );
    assert(NULL != aOut         );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA = Ethernet_Destination( lBase, lPacketInfo );" EOL);

    OneLineIf(aOut, aResultName, "( 0xffff == lA[ 0 ] ) && ( 0xffff == lA[ 1 ] ) && ( 0xffff == lA[ 2 ] )");
}

void DstHost(const Token & aDstHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aDstHost));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("ARP_ETHERNET_TYPE_nh");
            aOut->C_switch("ARP_Protocol( lEthernet)");

                if (aDstHost.mData.mFlags.mIPv4)
                {
                    aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                        IPv4_Address(aResultName, aOut, "ARP_Destination( lEthernet )", aDstHost);
                    aOut->C_break();
                }

                if (aDstHost.mData.mFlags.mIPv6)
                {
                    aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                        IPv6_Address(aResultName, aOut, "ARP_Destination( lEthernet )", aDstHost);
                    aOut->C_break();
                }

            aOut->C_switch_End();
        aOut->C_break();

        if (aDstHost.mData.mFlags.mIPv4)
        {
            aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                IPv4_Address(aResultName, aOut, "IPv4_Destination( lEthernet )", aDstHost);
            aOut->C_break();
        }

        if (aDstHost.mData.mFlags.mIPv6)
        {
            aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                IPv6_Address(aResultName, aOut, "IPv6_Destination( lEthernet )", aDstHost);
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void DstNet(const Token & aDstNet, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aDstNet ));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("ARP_ETHERNET_TYPE_nh");
            aOut->C_switch("ARP_Protocol( lEthernet)");

                if (aDstNet.mData.mFlags.mIPv4)
                {
                    aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                        IPv4_Network(aResultName, aOut, "ARP_Destination( lEthernet )", aDstNet);
                    aOut->C_break();
                }

                if (aDstNet.mData.mFlags.mIPv6)
                {
                    aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                        IPv6_Network(aResultName, aOut, "ARP_Destination( lEthernet )", aDstNet);
                    aOut->C_break();
                }

            aOut->C_switch_End();
        aOut->C_break();

        if (aDstNet.mData.mFlags.mIPv4)
        {
            aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                IPv4_Network(aResultName, aOut, "IPv4_Destination( lEthernet )", aDstNet);
            aOut->C_break();
        }

        if (aDstNet.mData.mFlags.mIPv6)
        {
            aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                IPv6_Network(aResultName, aOut, "IPv6_Destination( lEthernet )", aDstNet);
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void DstPort(const Token & aDstPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aDstPort));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    Begin_IP(aResultName, aOut);

    aOut->C_case("TCP_IP_PROTOCOL");
        Port(aOut, aResultName, "TCP_DestinationPort( lIP )", aDstPort);
    aOut->C_break();
    aOut->C_case("UDP_IP_PROTOCOL");
        Port(aOut, aResultName, "UDP_DestinationPort( lIP )", aDstPort);
    aOut->C_break();

    End_IP(aOut);
}

void DstPortRange(const Token & aDstPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aDstPortRange));
    assert(NULL != aResultName     );
    assert(NULL != aOut            );

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->C_case("TCP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "TCP_DestinationPort( lIP )", aDstPortRange);
    aOut->C_break();
    aOut->C_case("UDP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "UDP_DestinationPort( lIP )", aDstPortRange);
    aOut->C_break();

    End_IP(aOut);
}

void EtherDst(const Token & aEtherDst, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aEtherDst));
    assert(NULL != aResultName );
    assert(NULL != aOut        );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);

    EthernetAddress(aOut, aResultName, "Ethernet_Destination( lBase, lPacketInfo )", aEtherDst);
}

void EtherHost(const Token & aEtherHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aEtherHost));
    assert(NULL != aResultName  );
    assert(NULL != aOut         );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);

    EthernetAddress(aOut, aResultName, "Ethernet_Destination( lBase, lPacketInfo )", aEtherHost);
    SpecialElse(aOut);
        EthernetAddress(aOut, aResultName, "Ethernet_Source( lBase, lPacketInfo )", aEtherHost);
    aOut->C_if_End();
}

void EtherProto(const Token & aEtherProto, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aEtherProto));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    char lLine[1024];

    sprintf_s(lLine, "%s = ( lEthernet_Type_nh == 0x%04x );" EOL, aResultName, SwapBytes(aEtherProto.mData.mEthernetType));
    aOut->Append(lLine);
}

void EtherSrc(const Token & aEtherSrc, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aEtherSrc));
    assert(NULL != aResultName );
    assert(NULL != aOut        );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);

    EthernetAddress(aOut, aResultName, "Ethernet_Source( lBase, lPacketInfo )", aEtherSrc);
}

void Greater(const Token & aGreater, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aGreater));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    char lLine[1024];

    sprintf_s(lLine, "%s = ( lPacketInfo->mSize_byte >= %u );" EOL, aResultName, aGreater.mData.mPacketSize_byte);
    aOut->Append(lLine);
}

void Host(const Token & aHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aHost   ));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("ARP_ETHERNET_TYPE_nh");
            aOut->C_switch("ARP_Protocol( lEthernet)");

                if (aHost.mData.mFlags.mIPv4)
                {
                    aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                        IPv4_Address(aResultName, aOut, "ARP_Destination( lEthernet )", aHost);
                        SpecialElse(aOut);
                            IPv4_Address(aResultName, aOut, "ARP_Source( lEthernet )", aHost);
                        aOut->C_if_End();
                    aOut->C_break();
                }

                if (aHost.mData.mFlags.mIPv6)
                {
                    aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                        IPv6_Address(aResultName, aOut, "ARP_Destination( lEthernet )", aHost);
                        SpecialElse(aOut);
                            IPv6_Address(aResultName, aOut, "ARP_Source( lEthernet )", aHost);
                        aOut->C_if_End();
                    aOut->C_break();
                }

            aOut->C_switch_End();
        aOut->C_break();

        if (aHost.mData.mFlags.mIPv4)
        {
            aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                IPv4_Address(aResultName, aOut, "IPv4_Destination( lEthernet )", aHost);
                SpecialElse(aOut);
                    IPv4_Address(aResultName, aOut, "IPv4_Source( lEthernet )", aHost);
                aOut->C_if_End();
            aOut->C_break();
        }

        if (aHost.mData.mFlags.mIPv6)
        {
            aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                IPv6_Address(aResultName, aOut, "IPv6_Destination( lEthernet )", aHost);
                SpecialElse(aOut);
                    IPv6_Address(aResultName, aOut, "IPv6_Source( lEthernet )", aHost);
                aOut->C_if_End();
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void InEthernet(const Token & aInEthernet, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aInEthernet));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    aOut->Append("OPEN_NET_GLOBAL unsigned char * lData = Ethernet_Data( lBase, lPacketInfo );" EOL);
    aOut->Append("unsigned int lDataSize_byte = Ethernet_DataSize( lBase, lPacketInfo );"       EOL);

    // TODO  CodeGen.Phase3
    //       Remove these security when no longer needed!
    aOut->Append("if ( (                           16384 - 14 ) < lDataSize_byte ) { lDataSize_byte = (                           16384 - 14 ); }" EOL);
    aOut->Append("if ( ( aBufferHeader->mPacketSize_byte - 14 ) < lDataSize_byte ) { lDataSize_byte = ( aBufferHeader->mPacketSize_byte - 14 ); }" EOL);

    In(aInEthernet, aResultName, aOut);
}

void InIp(const Token & aInIp, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aInIp)   );
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->C_if("0 != lIP");

        aOut->Append("OPEN_NET_GLOBAL char * lData = ( OPEN_NET_GLOBAL char * )( lIP );" EOL);
        aOut->Append("unsigned int lDataSize_byte = lIP_DataSize_byte;" EOL);

        // TODO  CodeGen.Phase3
        //       Remove these security when no longer needed!
        aOut->Append("if ( (                           16384 - 34 ) < lDataSize_byte ) { lDataSize_byte = (                           16384 - 34 ); }" EOL);
        aOut->Append("if ( ( aBufferHeader->mPacketSize_byte - 34 ) < lDataSize_byte ) { lDataSize_byte = ( aBufferHeader->mPacketSize_byte - 34 ); }" EOL);

        In(aInIp, aResultName, aOut);

        if (NULL != aOut->GetErrorMessage())
        {
            return;
        }

    aOut->C_if_End();
}

void InTcp(const Token & aInTcp, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aInTcp)  );
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->C_if("( 0 != lIP ) && ( TCP_IP_PROTOCOL == lIP_Protocol )");

        aOut->Append("OPEN_NET_GLOBAL unsigned char * lData = TCP_Data( lIP );"                EOL);
        aOut->Append("unsigned int lDataSize_byte = lIP_DataSize_byte - TCP_HEADER_SIZE_byte;" EOL);

        // TODO  CodeGen.Phase3
        //       Remove these security when no longer needed!
        aOut->Append("if ( (                           16384 - 54 ) < lDataSize_byte ) { lDataSize_byte = (                           16384 - 54 ); }" EOL);
        aOut->Append("if ( ( aBufferHeader->mPacketSize_byte - 54 ) < lDataSize_byte ) { lDataSize_byte = ( aBufferHeader->mPacketSize_byte - 54 ); }" EOL);

        In(aInTcp, aResultName, aOut);

        if (NULL != aOut->GetErrorMessage())
        {
            return;
        }

        aOut->C_if_End();
}

void InUdp(const Token & aInUdp, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aInUdp)  );
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->C_if("( 0 != lIP ) && ( UDP_IP_PROTOCOL == lIP_Protocol )");

        aOut->Append("OPEN_NET_GLOBAL char * lData = UDP_Data( lIP );"                         EOL);
        aOut->Append("unsigned int lDataSize_byte = lIP_DataSize_byte - UDP_HEADER_SIZE_byte;" EOL);

        aOut->Append("if ( (                           16384 - 42 ) < lDataSize_byte ) { lDataSize_byte = (                           16384 - 42 ); }" EOL);
        aOut->Append("if ( ( aBufferHeader->mPacketSize_byte - 42 ) < lDataSize_byte ) { lDataSize_byte = ( aBufferHeader->mPacketSize_byte - 42 ); }" EOL);

        In(aInUdp, aResultName, aOut);

        if (NULL != aOut->GetErrorMessage())
        {
            return;
        }

    aOut->C_if_End();
}

void Inbound(const Token & aInbound, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aInbound));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    char lLine[1024];

    sprintf_s(lLine, "%s = 1;" EOL, aResultName);
    aOut->Append(lLine);
}

void IpDstHost(const Token & aIpDstHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIpDstHost));
    assert(NULL != aResultName  );
    assert(NULL != aOut         );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        if (aIpDstHost.mData.mFlags.mIPv4)
        {
            aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                IPv4_Address(aResultName, aOut, "IPv4_Destination( lEthernet )", aIpDstHost);
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void IpHost(const Token & aIpHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIpHost ));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        if (aIpHost.mData.mFlags.mIPv4)
        {
            aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                IPv4_Address(aResultName, aOut, "IPv4_Destination( lEthernet )", aIpHost);
                SpecialElse(aOut);
                    IPv4_Address(aResultName, aOut, "IPv4_Source( lEthernet )", aIpHost);
                aOut->C_if_End();
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void IpMulticast(const Token & aIpMulticast, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIpMulticast));
    assert(NULL != aResultName    );
    assert(NULL != aOut           );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("IPv4_ETHERNET_TYPE_nh");
            aOut->Append("lA = IPv4_Destination(lEthernet);" EOL);
            OneLineIf(aOut, aResultName, "0x00e0 == ( lA[ 0 ] & 0x00f0 )");
        aOut->C_break();
    aOut->C_switch_End();
}

void IpNet(const Token & aIpNet, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIpNet    ));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("ARP_ETHERNET_TYPE_nh");
            aOut->C_switch("ARP_Protocol( lEthernet)");

                if (aIpNet.mData.mFlags.mIPv4)
                {
                    aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                        IPv4_Network(aResultName, aOut, "ARP_Destination( lEthernet )", aIpNet);
                        SpecialElse(aOut);
                            IPv4_Network(aResultName, aOut, "ARP_Source( lEthernet )", aIpNet);
                        aOut->C_if_End();
                    aOut->C_break();
                }

            aOut->C_switch_End();
        aOut->C_break();

        if (aIpNet.mData.mFlags.mIPv4)
        {
            aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                IPv4_Network(aResultName, aOut, "IPv4_Destination( lEthernet )", aIpNet);
                SpecialElse(aOut);
                    IPv4_Network(aResultName, aOut, "IPv4_Source( lEthernet )", aIpNet);
                aOut->C_if_End();
            aOut->C_break();
        }

    aOut->C_switch_End();
}

void IpProto(const Token & aIpProto, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIpProto));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    char lLine[1024];

    sprintf_s(lLine, "%s = ( ( IPv4_ETHERNET_TYPE_nh == lEthernet_Type_nh ) && ( %u == lIP_Protocol ) );" EOL, aResultName, aIpProto.mData.mIpProtocol);
    aOut->Append(lLine);
}

void IpSrcHost(const Token & aIpSrcHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIpSrcHost));
    assert(NULL != aResultName  );
    assert(NULL != aOut         );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        if (aIpSrcHost.mData.mFlags.mIPv4)
        {
            aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                IPv4_Address(aResultName, aOut, "IPv4_Source( lEthernet )", aIpSrcHost);
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void Ip6DstHost(const Token & aIp6DstHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIp6DstHost));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        if (aIp6DstHost.mData.mFlags.mIPv6)
        {
            aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                IPv6_Address(aResultName, aOut, "IPv6_Destination( lEthernet )", aIp6DstHost);
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void Ip6Host(const Token & aIp6Host, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIp6Host));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        if (aIp6Host.mData.mFlags.mIPv6)
        {
            aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                IPv6_Address(aResultName, aOut, "IPv6_Destination( lEthernet )", aIp6Host);
                SpecialElse(aOut);
                    IPv6_Address(aResultName, aOut, "IPv6_Source( lEthernet )", aIp6Host);
                aOut->C_if_End();
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void Ip6Multicast(const Token & aIp6Multicast, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIp6Multicast));
    assert(NULL != aResultName     );
    assert(NULL != aOut            );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("IPv6_ETHERNET_TYPE_nh");
            aOut->Append("lA = IPv6_Destination(lEthernet);" EOL);
            OneLineIf(aOut, aResultName, "( 0x1eff == lA[ 0 ] ) && ( 0x0000 == lA[ 1 ] ) && ( 0x0000 == lA[ 2 ] ) && ( 0x0000 == lA[ 3 ] ) && ( 0x0000 == lA[ 4 ] ) && ( 0x0000 == lA[ 5 ] )");
        aOut->C_break();
    aOut->C_switch_End();
}

void Ip6Net(const Token & aIp6Net, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIp6Net ));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("ARP_ETHERNET_TYPE_nh");
            aOut->C_switch("ARP_Protocol( lEthernet)");

                if (aIp6Net.mData.mFlags.mIPv6)
                {
                    aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                        IPv6_Network(aResultName, aOut, "ARP_Destination( lEthernet )", aIp6Net);
                        SpecialElse(aOut);
                            IPv6_Network(aResultName, aOut, "ARP_Source( lEthernet )", aIp6Net);
                        aOut->C_if_End();
                    aOut->C_break();
                }

            aOut->C_switch_End();
        aOut->C_break();

        if (aIp6Net.mData.mFlags.mIPv6)
        {
            aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                IPv6_Network(aResultName, aOut, "IPv6_Destination( lEthernet )", aIp6Net);
                SpecialElse(aOut);
                    IPv6_Network(aResultName, aOut, "IPv6_Source( lEthernet )", aIp6Net);
                aOut->C_if_End();
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void Ip6Proto(const Token & aIpProto, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIpProto));
    assert(NULL != aResultName);
    assert(NULL != aOut);

    char lLine[1024];

    sprintf_s(lLine, "%s = ( ( IPv6_ETHERNET_TYPE_nh == lEthernet_Type_nh ) && ( %u == lIP_Protocol ) );" EOL, aResultName, aIpProto.mData.mIpProtocol);
    aOut->Append(lLine);
}

void Ip6SrcHost(const Token & aIp6SrcHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIp6SrcHost));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        if (aIp6SrcHost.mData.mFlags.mIPv6)
        {
            aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                IPv6_Address(aResultName, aOut, "IPv6_Source( lEthernet )", aIp6SrcHost);
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void Less(const Token & aLess, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aLess)   );
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    char lLine[1024];

    sprintf_s(lLine, "%s = ( lPacketInfo->mSize_byte <= %u );" EOL, aResultName, aLess.mData.mPacketSize_byte);
    aOut->Append(lLine);
}

void Multicast(const Token & aMulticast, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aMulticast));
    assert(NULL != aResultName  );
    assert(NULL != aOut         );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA = Ethernet_Destination( lBase, lPacketInfo );" EOL);
    OneLineIf(aOut, aResultName, "( ( 0x0001 == lA[ 0 ] ) && ( 0x005e == ( lA[ 1 ] & 0x80ff ) ) ) || ( 0x3333 == lA[ 0 ] )");
}

void Net(const Token & aNet, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aNet    ));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("ARP_ETHERNET_TYPE_nh");
            aOut->C_switch("ARP_Protocol( lEthernet)");

                if (aNet.mData.mFlags.mIPv4)
                {
                    aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                        IPv4_Network(aResultName, aOut, "ARP_Destination( lEthernet )", aNet);
                        SpecialElse(aOut);
                            IPv4_Network(aResultName, aOut, "ARP_Source( lEthernet )", aNet);
                        aOut->C_if_End();
                    aOut->C_break();
                }

                if (aNet.mData.mFlags.mIPv6)
                {
                    aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                        IPv6_Network(aResultName, aOut, "ARP_Destination( lEthernet )", aNet);
                        SpecialElse(aOut);
                            IPv6_Network(aResultName, aOut, "ARP_Source( lEthernet )", aNet);
                        aOut->C_if_End();
                    aOut->C_break();
                }

            aOut->C_switch_End();
        aOut->C_break();

        if (aNet.mData.mFlags.mIPv4)
        {
            aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                IPv4_Network(aResultName, aOut, "IPv4_Destination( lEthernet )", aNet);
                SpecialElse(aOut);
                    IPv4_Network(aResultName, aOut, "IPv4_Source( lEthernet )", aNet);
                aOut->C_if_End();
            aOut->C_break();
        }

        if (aNet.mData.mFlags.mIPv6)
        {
            aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                IPv6_Network(aResultName, aOut, "IPv6_Destination( lEthernet )", aNet);
                SpecialElse(aOut);
                    IPv6_Network(aResultName, aOut, "IPv6_Source( lEthernet )", aNet);
                aOut->C_if_End();
            aOut->C_break();
        }
    aOut->C_switch_End();
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

void Operator(const Token & aOperator, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aOperator));
    assert(NULL != aResultName );
    assert(NULL != aOut        );

    Operator(aOperator, aResultName, aOut, aOperator.mData.mCode);
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
            aOut->C_if_End();
        }
    }
}

void Port(const Token & aPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aPort)   );
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    Begin_IP(aResultName, aOut);

    aOut->C_case("TCP_IP_PROTOCOL");
        Port(aOut, aResultName, "TCP_DestinationPort( lIP )", aPort);
        SpecialElse(aOut);
            Port(aOut, aResultName, "TCP_SourcePort( lIP )", aPort);
        aOut->C_if_End();
    aOut->C_break();
    aOut->C_case("UDP_IP_PROTOCOL");
        Port(aOut, aResultName, "UDP_DestinationPort( lIP )", aPort);
        SpecialElse(aOut);
            Port(aOut, aResultName, "UDP_SourcePort( lIP )", aPort);
        aOut->C_if_End();
    aOut->C_break();

    End_IP(aOut);
}

void PortRange(const Token & aPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aPortRange));
    assert(NULL != aResultName  );
    assert(NULL != aOut         );

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->C_case("TCP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "TCP_DestinationPort( lIP )", aPortRange);
        SpecialElse(aOut);
            PortRange(aOut, aResultName, "TCP_SourcePort( lIP )", aPortRange);
        aOut->C_if_End();
    aOut->C_break();
    aOut->C_case("UDP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "UDP_DestinationPort( lIP )", aPortRange);
        SpecialElse(aOut);
            PortRange(aOut, aResultName, "UDP_SourcePort( lIP )", aPortRange);
        aOut->C_if_End();
    aOut->C_break();

    End_IP(aOut);
}

void Proto(const Token & aIpProto, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aIpProto));
    assert(NULL != aResultName);
    assert(NULL != aOut);

    char lLine[1024];

    sprintf_s(lLine, "%s = (0 != lIP) && ( %u == lIP_Protocol );" EOL, aResultName, aIpProto.mData.mIpProtocol);
    aOut->Append(lLine);
}

void SrcHost(const Token & aSrcHost, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aSrcHost));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("ARP_ETHERNET_TYPE_nh");
            aOut->C_switch("ARP_Protocol( lEthernet)");

                if (aSrcHost.mData.mFlags.mIPv4)
                {
                    aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                        IPv4_Address(aResultName, aOut, "ARP_Source( lEthernet )", aSrcHost);
                    aOut->C_break();
                }

                if (aSrcHost.mData.mFlags.mIPv6)
                {
                    aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                        IPv6_Address(aResultName, aOut, "ARP_Source( lEthernet )", aSrcHost);
                    aOut->C_break();
                }

            aOut->C_switch_End();
        aOut->C_break();

        if (aSrcHost.mData.mFlags.mIPv4)
        {
            aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                IPv4_Address(aResultName, aOut, "IPv4_Source( lEthernet )", aSrcHost);
            aOut->C_break();
        }

        if (aSrcHost.mData.mFlags.mIPv6)
        {
            aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                IPv6_Address(aResultName, aOut, "IPv6_Source( lEthernet )", aSrcHost);
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void SrcNet(const Token & aSrcNet, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aSrcNet ));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    aOut->Append("OPEN_NET_GLOBAL unsigned short * lA;" EOL);
    aOut->Append(                                       EOL);
    aOut->C_switch("lEthernet_Type_nh");
        aOut->C_case("ARP_ETHERNET_TYPE_nh");
            aOut->C_switch("ARP_Protocol( lEthernet)");

                if (aSrcNet.mData.mFlags.mIPv4)
                {
                    aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                        IPv4_Network(aResultName, aOut, "ARP_Source( lEthernet )", aSrcNet);
                    aOut->C_break();
                }

                if (aSrcNet.mData.mFlags.mIPv6)
                {
                    aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                        IPv6_Network(aResultName, aOut, "ARP_Source( lEthernet )", aSrcNet);
                    aOut->C_break();
                }

            aOut->C_switch_End();
        aOut->C_break();

        if (aSrcNet.mData.mFlags.mIPv4)
        {
            aOut->C_case("IPv4_ETHERNET_TYPE_nh");
                IPv4_Network(aResultName, aOut, "IPv4_Source( lEthernet )", aSrcNet);
            aOut->C_break();
        }

        if (aSrcNet.mData.mFlags.mIPv6)
        {
            aOut->C_case("IPv6_ETHERNET_TYPE_nh");
                IPv6_Network(aResultName, aOut, "IPv6_Source( lEthernet )", aSrcNet);
            aOut->C_break();
        }
    aOut->C_switch_End();
}

void SrcPort(const Token & aSrcPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aSrcPort));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    Begin_IP(aResultName, aOut);

    aOut->C_case("TCP_IP_PROTOCOL");
        Port(aOut, aResultName, "TCP_SourcePort( lIP )", aSrcPort);
    aOut->C_break();
    aOut->C_case("UDP_IP_PROTOCOL");
        Port(aOut, aResultName, "UDP_SourcePort( lIP )", aSrcPort);
    aOut->C_break();

    End_IP(aOut);
}

void SrcPortRange(const Token & aSrcPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aSrcPortRange));
    assert(NULL != aResultName     );
    assert(NULL != aOut            );

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->C_case("TCP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "TCP_SourcePort( lIP )", aSrcPortRange);
    aOut->C_break();
    aOut->C_case("UDP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "UDP_SourcePort( lIP )", aSrcPortRange);
    aOut->C_break();

    End_IP(aOut);
}

void UdpDstPort(const Token & aUdpDstPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpDstPort));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    Begin_IP(aResultName, aOut);

    aOut->C_case("UDP_IP_PROTOCOL");
        Port(aOut, aResultName, "UDP_DestinationPort( lIP )", aUdpDstPort);
    aOut->C_break();

    End_IP(aOut);
}

void UdpDstPortRange(const Token & aUdpDstPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpDstPortRange));
    assert(NULL != aResultName        );
    assert(NULL != aOut               );

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->C_case("UDP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "UDP_DestinationPort( lIP )", aUdpDstPortRange);
    aOut->C_break();

    End_IP(aOut);
}

void UdpPort(const Token & aUdpPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpPort));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    Begin_IP(aResultName, aOut);

    aOut->C_case("UDP_IP_PROTOCOL");
        Port(aOut, aResultName, "UDP_DestinationPort( lIP )", aUdpPort);
        SpecialElse(aOut);
            Port(aOut, aResultName, "UDP_SourcePort( lIP )", aUdpPort);
        aOut->C_if_End();
    aOut->C_break();

    End_IP(aOut);
}

void UdpPortRange(const Token & aUdpPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpPortRange));
    assert(NULL != aResultName     );
    assert(NULL != aOut            );

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->C_case("UDP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "UDP_DestinationPort( lIP )", aUdpPortRange);
        SpecialElse(aOut);
            PortRange(aOut, aResultName, "UDP_SourcePort( lIP )", aUdpPortRange);
        aOut->C_if_End();
    aOut->C_break();

    End_IP(aOut);
}

void UdpSrcPort(const Token & aUdpSrcPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpSrcPort));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    Begin_IP(aResultName, aOut);

    aOut->C_case("UDP_IP_PROTOCOL");
        Port(aOut, aResultName, "UDP_SourcePort( lIP )", aUdpSrcPort);
    aOut->C_break();

    End_IP(aOut);
}

void UdpSrcPortRange(const Token & aUdpSrcPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aUdpSrcPortRange));
    assert(NULL != aResultName        );
    assert(NULL != aOut               );

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->C_case("UDP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "UDP_SourcePort( lIP )", aUdpSrcPortRange);
    aOut->C_break();

    End_IP(aOut);
}

void TcpDstPort(const Token & aTcpDstPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpDstPort));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    Begin_IP(aResultName, aOut);

    aOut->C_case("TCP_IP_PROTOCOL");
        Port(aOut, aResultName, "TCP_DestinationPort( lIP )", aTcpDstPort);
    aOut->C_break();

    End_IP(aOut);
}

void TcpDstPortRange(const Token & aTcpDstPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpDstPortRange));
    assert(NULL != aResultName        );
    assert(NULL != aOut               );

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->C_case("TCP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "TCP_DestinationPort( lIP )", aTcpDstPortRange);
    aOut->C_break();

    End_IP(aOut);

}

void TcpPort(const Token & aTcpPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpPort));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    Begin_IP(aResultName, aOut);

    aOut->C_case("TCP_IP_PROTOCOL");
        Port(aOut, aResultName, "TCP_DestinationPort( lIP )", aTcpPort);
        SpecialElse(aOut);
            Port(aOut, aResultName, "TCP_SourcePort( lIP )", aTcpPort);
        aOut->C_if_End();
    aOut->C_break();

    End_IP(aOut);
}

void TcpPortRange(const Token & aTcpPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpPortRange));
    assert(NULL != aResultName     );
    assert(NULL != aOut            );

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->C_case("TCP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "TCP_DestinationPort( lIP )", aTcpPortRange);
        SpecialElse(aOut);
            PortRange(aOut, aResultName, "TCP_SourcePort( lIP )", aTcpPortRange);
        aOut->C_if_End();
    aOut->C_break();

    End_IP(aOut);
}

void TcpSrcPort(const Token & aTcpSrcPort, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpSrcPort));
    assert(NULL != aResultName   );
    assert(NULL != aOut          );

    Begin_IP(aResultName, aOut);

    aOut->C_case("TCP_IP_PROTOCOL");
        Port(aOut, aResultName, "TCP_SourcePort( lIP )", aTcpSrcPort);
    aOut->C_break();

    End_IP(aOut);
}

void TcpSrcPortRange(const Token & aTcpSrcPortRange, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aTcpSrcPortRange));
    assert(NULL != aResultName        );
    assert(NULL != aOut               );

    Begin_IP(aResultName, aOut, "unsigned short lPort;" EOL);

    aOut->C_case("TCP_IP_PROTOCOL");
        PortRange(aOut, aResultName, "TCP_SourcePort( lIP )", aTcpSrcPortRange);
    aOut->C_break();

    End_IP(aOut);
}

void Vlan(const Token & aVlan, const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != (&aVlan   ));
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    char lLine[1024];

    if (aVlan.mData.mFlags.mVlanId)
    {
        sprintf_s(lLine, "%s = ( 0x%04x == ( Ethernet_VlanTag( lBase, lPacketInfo ) & 0xff0f ) );" EOL, aResultName, SwapBytes(aVlan.mData.mVlanId));
    }
    else
    {
        sprintf_s(lLine, "%s = Ethernet_Vlan( lBase, lPacketInfo );" EOL, aResultName);
    }

    aOut->Append(lLine);
}

// ===== Gen1 ===============================================================

void Add   (const Token & aAdd   , Filter_Internal * aOut) { Operator(aAdd   , aOut, "+" ); }
void BitAnd(const Token & aBitAnd, Filter_Internal * aOut) { Operator(aBitAnd, aOut, "&" ); }
void BitOr (const Token & aBitOr , Filter_Internal * aOut) { Operator(aBitOr , aOut, "|" ); }
void BitXor(const Token & aBitXor, Filter_Internal * aOut) { Operator(aBitXor, aOut, "^" ); }
void Dash  (const Token & aDash  , Filter_Internal * aOut) { Operator(aDash  , aOut, "-" ); }

void Data(const Token & aData, Filter_Internal * aOut)
{
    assert(NULL != (&aData)      );
    assert(NULL != aData.mText   );
    assert(   0 <  aData.mTextLen);
    assert(NULL != aOut          );

    char lCode[1024];

    memset(&lCode, 0, sizeof(lCode));
    memcpy(lCode, aData.mText, aData.mTextLen);

    aOut->AppendCode(lCode);
}

void Div(const Token & aDiv, Filter_Internal * aOut) { Operator(aDiv, aOut, "/" ); }

void EtherIndex(const Token & aEtherIndex, Filter_Internal * aOut)
{
    assert(NULL != (&aEtherIndex));
    assert(NULL != aOut          );

    char lCode[1024];

    switch (aEtherIndex.mData.mAccessSize_byte)
    {
    case 1: aOut->AppendCode("( * ( lBase + lPacketInfo->mOffset_byte + ( "); break;
    case 2:	aOut->AppendCode("SwapUInt16( * ( ( OPEN_NET_GLOBAL unsigned short * )( lBase + lPacketInfo->mOffset_byte + ( "); break;
    case 4: aOut->AppendCode("ReadUInt32(   ( ( OPEN_NET_GLOBAL unsigned short * )( lBase + lPacketInfo->mOffset_byte + ( "); break;
    default: assert(false);
    }

    if (aEtherIndex.mData.mFlags.mIndex)
    {
        sprintf_s(lCode, "0x%04x", aEtherIndex.mData.mIndex);
        aOut->AppendCode(lCode);
    }
    else
    {
        if (NULL != aEtherIndex.mA)
        {
            GenerateCodeForToken(*aEtherIndex.mA, aOut);
        }

        sprintf_s(lCode, " %s ", aEtherIndex.mData.mCode);
        aOut->AppendCode(lCode);

        if (NULL != aEtherIndex.mB)
        {
            GenerateCodeForToken(*aEtherIndex.mB, aOut);
        }
    }

    switch (aEtherIndex.mData.mAccessSize_byte)
    {
    case 1: aOut->AppendCode(" ) ) )"  ); break;

    case 2:
    case 4:	aOut->AppendCode(" ) ) ) )"); break;

    default: assert(false);
    }
}

void Len(const Token & aLen, Filter_Internal * aOut)
{
    assert(NULL != aOut);

    aOut->AppendCode("lPacketInfo->mSize_byte");
}

void Operator(const Token & aOperator  , Filter_Internal * aOut)
{
    assert(NULL != (&aOperator));
    assert(NULL != aOut        );

    Operator(aOperator, aOut, aOperator.mData.mCode );
}
