
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Token.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== Windows ============================================================
#include <WinSock2.h>
#include <WS2tcpip.h>

// ===== CodeGen ============================================================
#include "Token.h"

// Public
/////////////////////////////////////////////////////////////////////////////

Token::Token(TokenId aId, const char * aText, unsigned int aTextLen)
    : mA      (NULL    )
    , mB      (NULL    )
    , mText   (aText   )
    , mTextLen(aTextLen)
    , mId     (aId     )
{
    assert(TOKEN_QTY >  aId     );
    assert(NULL      != aText   );
    assert(        0 <  aTextLen);

    mFlags.mBoolean  = false;
    mFlags.mOperator = false;

    for (unsigned int i = 0; i < 8; i++)
    {
        mData_UInt16[i] = 0;
    }

    for (unsigned int i = 0; i < 4; i++)
    {
        mData_UInt8[i] = 0;
    }
}

Token::Token(const Token & aToken)
    : mA      (NULL           )
    , mB      (NULL           )
    , mId     (aToken.mId     )
    , mText   (aToken.mText   )
    , mTextLen(aToken.mTextLen)
{
    mFlags.mBoolean  = aToken.mFlags.mBoolean ;
    mFlags.mOperator = aToken.mFlags.mOperator;

    CopyData(aToken);
}

Token::~Token()
{
    if (NULL != mA)
    {
        delete mA;
    }

    if (NULL != mB)
    {
        delete mB;
    }
}

bool Token::Is(TokenId aId) const
{
    switch (aId)
    {
    case TOKEN_ANY     : return true;
    case TOKEN_BOOLEAN : return mFlags.mBoolean ;
    case TOKEN_OPERATOR: return mFlags.mOperator;
    }

    return (mId == aId);
}

void Token::SetA(Token * aA)
{
    assert(NULL != aA);

    assert(NULL == mA);

    mA = aA;
}

void Token::SetB(Token * aB)
{
    assert(NULL != aB);

    assert(NULL == mB);

    mB = aB;
}

void Token::SetBoolean()
{
    assert(!mFlags.mBoolean);

    mFlags.mBoolean = true;
}

void Token::SetOperator()
{
    assert(!mFlags.mOperator);

    mFlags.mOperator = true;
}

bool Token::ConvertToIpAddress()
{
    assert(NULL != mText   );
    assert(   0 <  mTextLen);

    char lText[1024];

    memset(&lText, 0, sizeof(lText));

    memcpy(lText, mText, mTextLen);

    unsigned int lA[8];

    if (4 == sscanf_s(lText, "%u.%u.%u.%u", lA + 0, lA + 1, lA + 2, lA + 3))
    {
        for (unsigned int i = 0; i < 4; i++)
        {
            if (0xff < lA[i])
            {
                return false;
            }

            mData_UInt8[i] = lA[i];
        }

        mFlags.mIPv4 = true;
        return true;
    }

    if (8 == sscanf_s(lText, "%x:%x:%x:%x:%x:%x:%x:%x", lA + 0, lA + 1, lA + 2, lA + 3, lA + 4, lA + 5, lA + 6, lA + 7))
    {
        for (unsigned int i = 0; i < 8; i++)
        {
            if (0xffff < lA[i])
            {
                return false;
            }

            mData_UInt16[i] = lA[i];
        }

        mFlags.mIPv6 = true;
        return true;
    }

    struct addrinfo lInfo;

    memset(&lInfo, 0, sizeof(lInfo));

    lInfo.ai_flags = AI_ALL;

    struct addrinfo * lAddress;

    if (0 == getaddrinfo(lText, NULL, &lInfo, &lAddress))
    {
        bool lResult = false;

        while (NULL != lAddress)
        {
            struct sockaddr_in  * lIPv4 = reinterpret_cast<struct sockaddr_in  *>(lAddress->ai_addr);
            struct sockaddr_in6 * lIPv6 = reinterpret_cast<struct sockaddr_in6 *>(lAddress->ai_addr);

            switch (lAddress->ai_family)
            {
            case AF_INET : memcpy(mData_UInt8 , &lIPv4->sin_addr , sizeof(mData_UInt8 )); mFlags.mIPv4 = true, lResult = true; break;
            case AF_INET6: memcpy(mData_UInt16, &lIPv6->sin6_addr, sizeof(mData_UInt16)); mFlags.mIPv6 = true, lResult = true; break;
            }

            lAddress = lAddress->ai_next;
        }

        freeaddrinfo(lAddress);

        return lResult;
    }

    return false;
}

bool Token::ConvertToUInt16()
{
    char * lEnd;

    unsigned int lValue = strtoul(mText, &lEnd, 10);
    if ((0x10000 > lValue) && ((mText + mTextLen) == lEnd))
    {
        mData_UInt16[0] = lValue;
        return true;
    }

    return false;
}

bool Token::ConvertToUInt8()
{
    char * lEnd;

    unsigned int lValue = strtoul(mText, &lEnd, 10);
    if ((0x100 > lValue) && ((mText + mTextLen) == lEnd))
    {
        mData_UInt8[0] = lValue;
        return true;
    }

    return false;
}

void Token::CopyData(const Token & aSrc)
{
    assert(NULL != (&aSrc));

    for (unsigned int i = 0; i < 8; i++)
    {
        mData_UInt16[i] = aSrc.mData_UInt16[i];
    }

    for (unsigned int i = 0; i < 4; i++)
    {
        mData_UInt8[i] = aSrc.mData_UInt8[i];
    }

    mFlags.mIPv4 = aSrc.mFlags.mIPv4;
    mFlags.mIPv6 = aSrc.mFlags.mIPv6;
}

TokenList::~TokenList()
{
    while (!empty())
    {
        delete front();
        pop_front();
    }
}
