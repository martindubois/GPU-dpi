
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
    TOKEN_AND              ,
    TOKEN_DASH             ,
    TOKEN_DATA             ,
    TOKEN_DST              ,
    TOKEN_DST_PORT         ,
    TOKEN_DST_PORTRANGE    ,
    TOKEN_GREATER          ,
    TOKEN_HOST             ,
    TOKEN_ICMP             ,
    TOKEN_IP               ,
    TOKEN_IP_PROTO         ,
    TOKEN_IP6              ,
    TOKEN_IP6_PROTO        ,
    TOKEN_LESS             ,
    TOKEN_NOT              ,
    TOKEN_OR               ,
    TOKEN_PARENTHESIS_CLOSE,
    TOKEN_PARENTHESIS_OPEN ,
    TOKEN_PORT             ,
    TOKEN_PORTRANGE        ,
    TOKEN_PORT_RANGE       ,
    TOKEN_PROTO            ,
    TOKEN_SRC              ,
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

    TOKEN_QTY,

    // ===== Token family ===================================================
    TOKEN_ANY     ,
    TOKEN_BOOLEAN ,
    TOKEN_OPERATOR,
}
TokenId;

class Token
{

public:

    Token(TokenId aId, const char * aText, unsigned int aTextLen);

    Token(const Token & aToken);

    ~Token();

    bool Is(TokenId aId) const;

    void SetA       (Token * aA);
    void SetB       (Token * aB);
    void SetBoolean ();
    void SetOperator();

    bool ConvertToIpAddress();
    bool ConvertToUInt16   ();
    bool ConvertToUInt8    ();

    void CopyData(const Token & aSrc);

    struct
    {
        unsigned mBoolean  : 1;
        unsigned mIPv4     : 1;
        unsigned mIPv6     : 1;
        unsigned mOperator : 1;

        unsigned mReserved0 : 28;
    }
    mFlags;

    TokenId mId;

    const char * mText   ;
    unsigned int mTextLen;

    uint16_t mData_UInt16[ 8 ];
    uint8_t  mData_UInt8 [ 4 ];

    Token * mA;
    Token * mB;
};
