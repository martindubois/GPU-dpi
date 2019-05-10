
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Token.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== C ++ ===============================================================
#include <list>

// Class
/////////////////////////////////////////////////////////////////////////////

class Token;

class TokenList : public std::list< Token * >
{

public:

    ~TokenList();

};

typedef enum
{
    TOKEN_ABOVE            ,
    TOKEN_ABOVE_OR_EQUAL   ,
    TOKEN_ADD              ,
    TOKEN_AND              ,
    TOKEN_AH               ,
    TOKEN_ARP              ,
    TOKEN_ARP_DST_HOST     ,
    TOKEN_ARP_HOST         ,
    TOKEN_ARP_SRC_HOST     ,
    TOKEN_BELOW            ,
    TOKEN_BELOW_OR_EQUAL   ,
    TOKEN_BIT_AND          ,
    TOKEN_BIT_OR           ,
    TOKEN_BIT_XOR          ,
    TOKEN_BRACKET_CLOSE    ,
    TOKEN_BRACKET_OPEN     ,
    TOKEN_BROADCAST        ,
    TOKEN_COLON            ,
    TOKEN_DASH             ,
    TOKEN_DATA             ,
    TOKEN_DIV              ,
    TOKEN_DST              ,
    TOKEN_DST_HOST         ,
    TOKEN_DST_NET          ,
    TOKEN_DST_PORT         ,
    TOKEN_DST_PORTRANGE    ,
    TOKEN_ESP              ,
    TOKEN_EQUAL            ,
    TOKEN_ETHER            ,
    TOKEN_ETHER_DST        ,
    TOKEN_ETHER_HOST       ,
    TOKEN_ETHER_INDEX      ,
    TOKEN_ETHER_PROTO      ,
    TOKEN_ETHER_SRC        ,
    TOKEN_GREATER          ,
    TOKEN_HOST             ,
    TOKEN_ICMP             ,
    TOKEN_ICMP6            ,
    TOKEN_IGMP             ,
    TOKEN_IN_ETHERNET      ,
    TOKEN_IN_IP            ,
    TOKEN_IN_TCP           ,
    TOKEN_IN_UDP           ,
    TOKEN_INBOUND          ,
    TOKEN_INDEX            ,
    TOKEN_IP               ,
    TOKEN_IP_DST_HOST      ,
    TOKEN_IP_HOST          ,
    TOKEN_IP_MULTICAST     ,
    TOKEN_IP_NET           ,
    TOKEN_IP_PROTO         ,
    TOKEN_IP_SRC_HOST      ,
    TOKEN_IP6              ,
    TOKEN_IP6_DST_HOST     ,
    TOKEN_IP6_HOST         ,
    TOKEN_IP6_MULTICAST    ,
    TOKEN_IP6_NET          ,
    TOKEN_IP6_PROTO        ,
    TOKEN_IP6_SRC_HOST     ,
    TOKEN_LEN              ,
    TOKEN_LESS             ,
    TOKEN_LIST             ,
    TOKEN_MOD              ,
    TOKEN_MUL              ,
    TOKEN_MULTICAST        ,
    TOKEN_NET              ,
    TOKEN_NOT              ,
    TOKEN_NOT_EQUAL        ,
    TOKEN_OR               ,
    TOKEN_PARENTHESE_CLOSE ,
    TOKEN_PARENTHESE_OPEN  ,
    TOKEN_PIM              ,
    TOKEN_PORT             ,
    TOKEN_PORTRANGE        ,
    TOKEN_PROTO            ,
    TOKEN_RARP             ,
    TOKEN_SHIFT_LEFT       ,
    TOKEN_SHIFT_RIGHT      ,
    TOKEN_SRC              ,
    TOKEN_SRC_HOST         ,
    TOKEN_SRC_NET          ,
    TOKEN_SRC_PORT         ,
    TOKEN_SRC_PORTRANGE    ,
    TOKEN_TCP              ,
    TOKEN_TCP_DST_PORT     ,
    TOKEN_TCP_DST_PORTRANGE,
    TOKEN_TCP_PORT         ,
    TOKEN_TCP_PORTRANGE    ,
    TOKEN_TCP_SRC_PORT     ,
    TOKEN_TCP_SRC_PORTRANGE,
    TOKEN_UDP              ,
    TOKEN_UDP_DST_PORT     ,
    TOKEN_UDP_DST_PORTRANGE,
    TOKEN_UDP_PORT         ,
    TOKEN_UDP_PORTRANGE    ,
    TOKEN_UDP_SRC_PORT     ,
    TOKEN_UDP_SRC_PORTRANGE,
    TOKEN_VLAN             ,
    TOKEN_VRRP             ,

    TOKEN_QTY,

    // ===== Token family ===================================================
    TOKEN_ANY       ,
    TOKEN_BOOLEAN   ,
    TOKEN_OPERATOR  ,
    TOKEN_PORTRANGES,
    TOKEN_VALUE     ,
}
TokenId;

class Token
{

public:

    Token(TokenId aId, const char * aText, unsigned int aTextLen);

    Token(const Token & aToken);

    ~Token();

    void ClearValue();

    bool Is(TokenId aId) const;

    void SetA           (Token * aA);
    void SetAccessSize  (unsigned char aAccessSize_byte);
    void SetB           (Token * aB);
    void SetBoolean     ();
    void SetCode        (const char * aCode);
    void SetEthernetType(unsigned short aEthernetType);
    void SetIndex       (unsigned short aIndex       );
    void SetIpProtocol  (unsigned char  aIpProtocol  );
    void SetOperator    ();
    void SetValue       ();

    bool ConvertToAccessSize     ();
    bool ConvertToEthernetAddress();
    bool ConvertToEthernetType   ();
    bool ConvertToIndex          ();
    bool ConvertToIpAddress      ();
    bool ConvertToIpProtocol     ();
    bool ConvertToNetwork        ();
    bool ConvertToPacketSize     ();
    bool ConvertToPort           ();
    bool ConvertToPortRange      ();
    bool ConvertToVlanId         ();

    void CopyData(const Token & aSrc);

    struct
    {
        unsigned mBoolean  : 1;
        unsigned mOperator : 1;
        unsigned mValue    : 1;

        unsigned mReserved0 : 29;
    }
    mFlags;

    TokenId mId;

    const char * mText   ;
    unsigned int mTextLen;

    struct
    {
        struct
        {
            unsigned mAccessSize   : 1;
            unsigned mCode         : 1;
            unsigned mEthernet     : 1;
            unsigned mEthernetType : 1;
            unsigned mIndex        : 1;
            unsigned mIPv4         : 1;
            unsigned mIPv6         : 1;
            unsigned mIpProtocol   : 1;
            unsigned mNetMaskLen   : 1;
            unsigned mPacketSize   : 1;
            unsigned mPort         : 1;
            unsigned mPortRange    : 1;
            unsigned mVlanId       : 1;

            unsigned mReserved0 : 19;
        }
        mFlags;

        unsigned char  mAccessSize_byte;
        char           mCode       [64];
        unsigned char  mEthernet   [ 6];
        unsigned short mEthernetType   ;
        unsigned short mIndex          ;
        unsigned char  mIPv4       [ 4];
        unsigned short mIPv6       [ 8];
        unsigned char  mIpProtocol     ;
        unsigned char  mNetMaskLen     ;
        unsigned short mPacketSize_byte;
        unsigned short mPorts      [ 2];
        unsigned short mVlanId         ;
    }
    mData;

    Token * mA;
    Token * mB;

private:

    bool ConvertToIPv4Address(const unsigned int * aA);
    bool ConvertToIPv6Address(const unsigned int * aA);

};
