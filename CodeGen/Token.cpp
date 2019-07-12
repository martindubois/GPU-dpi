
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Token.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <stdint.h>
#include <stdio.h>

#ifdef _KMS_LINUX_

    #include <netdb.h>

    // ===== System =========================================================
    #include <sys/socket.h>
    #include <sys/types.h>

#endif

#ifdef _KMS_WINDOWS_
    // ===== Windows ========================================================
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#endif

// ===== CodeGen ============================================================
#include "Token.h"

// Static function declaration
/////////////////////////////////////////////////////////////////////////////

static bool         TextToUnsignedInt(const char * aText, unsigned int aTextLen, unsigned int * aOut);
static unsigned int TextToUnsignedInt(const char * aText,                        unsigned int * aOut);

// Public
/////////////////////////////////////////////////////////////////////////////

Token::Token(TokenId aId, const char * aText, unsigned int aTextLen)
    : mA      (NULL    )
    , mB      (NULL    )
    , mId     (aId     )
    , mText   (aText   )
    , mTextLen(aTextLen)
{
    assert(TOKEN_QTY >  aId     );
    assert(NULL      != aText   );
    assert(        0 <  aTextLen);

    mFlags.mBoolean  = false;
    mFlags.mOperator = false;
    mFlags.mValue    = false;

    memset(&mData, 0, sizeof(mData));
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

void Token::ClearValue()
{
    assert(mFlags.mValue);

    mFlags.mValue = false;
}

bool Token::Is(TokenId aId) const
{
    switch (aId)
    {
    case TOKEN_ANY     : return true;
    case TOKEN_BOOLEAN : return mFlags.mBoolean ;
    case TOKEN_OPERATOR: return mFlags.mOperator;

    case TOKEN_PORTRANGES:
        switch( mId )
        {
        case TOKEN_DST_PORTRANGE    :
        case TOKEN_PORTRANGE        :
        case TOKEN_SRC_PORTRANGE    :
        case TOKEN_TCP_DST_PORTRANGE:
        case TOKEN_TCP_PORTRANGE    :
        case TOKEN_TCP_SRC_PORTRANGE:
        case TOKEN_UDP_DST_PORTRANGE:
        case TOKEN_UDP_PORTRANGE    :
        case TOKEN_UDP_SRC_PORTRANGE:
            return true;
        }
        break;
    case TOKEN_VALUE   : return mFlags.mValue   ;
    }

    return (mId == aId);
}

void Token::SetA(Token * aA)
{
    assert(NULL != aA);

    assert(NULL == mA);

    mA = aA;
}

// aAccessSize_byte
void Token::SetAccessSize(unsigned char aAccessSize_byte)
{
    assert(0 < aAccessSize_byte);

    assert(0 == mData.mAccessSize_byte);
    assert(!mData.mFlags.mAccessSize);

    mData.mAccessSize_byte   = aAccessSize_byte;
    mData.mFlags.mAccessSize = true            ;
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

// aCode [---;R--]
void Token::SetCode(const char * aCode)
{
    assert(NULL != aCode);

    assert(!mData.mFlags.mCode);

    strcpy_s(mData.mCode, aCode);
    mData.mFlags.mCode = true;
}

// aEthernetType
void Token::SetEthernetType(unsigned short aEthernetType)
{
    assert(0 == mData.mEthernetType   );
    assert(!mData.mFlags.mEthernetType);

    mData.mEthernetType        = aEthernetType;
    mData.mFlags.mEthernetType = true         ;
}

// aIndex
void Token::SetIndex(unsigned short aIndex)
{
    assert(0 == mData.mIndex   );
    assert(!mData.mFlags.mIndex);

    mData.mIndex        = aIndex;
    mData.mFlags.mIndex = true  ;
}

// aIpProtocol
void Token::SetIpProtocol(unsigned char  aIpProtocol)
{
    assert(0 == mData.mIpProtocol   );
    assert(!mData.mFlags.mIpProtocol);

    mData.mIpProtocol        = aIpProtocol;
    mData.mFlags.mIpProtocol = true       ;
}

void Token::SetOperator()
{
    assert(!mFlags.mOperator);

    mFlags.mOperator = true;
}

void Token::SetValue()
{
    assert(!mFlags.mValue);

    mFlags.mValue = true;
}

// Return
//  false  Error
//  true   OK
bool Token::ConvertToAccessSize()
{
    assert(NULL != mText   );
    assert(   0 <  mTextLen);

    unsigned int lValue;

    if (TextToUnsignedInt(mText, mTextLen, &lValue))
    {
        switch (lValue)
        {
        case 1 :
        case 2 :
        case 4 :
            SetAccessSize(lValue);
            return true;
        }
    }

    return false;
}

// Return
//  false  Error
//  true   OK
bool Token::ConvertToEthernetAddress()
{
    assert(NULL != mText   );
    assert(   0 <  mTextLen);

    unsigned int lCount = 0;
    unsigned int lDigit = 0;
    uint8_t      lValue = 0;

    for (unsigned int i = 0; i < mTextLen; i++)
    {
        switch (mText[i])
        {
        case '0' :
        case '1' :
        case '2' :
        case '3' :
        case '4' :
        case '5' :
        case '6' :
        case '7' :
        case '8' :
        case '9' :
            lDigit++;
            lValue <<= 4;
            lValue  |= (mText[i] - '0');
            break;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            lDigit++;
            lValue <<= 4;
            lValue |= (mText[i] - 'a' + 10);
            break;

        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            lDigit++;
            lValue <<= 4;
            lValue |= (mText[i] - 'A' + 10);
            break;

        case ':':
        case '.':
        case '-':
            if (1 == lDigit)
            {
                lDigit = 2;
            }
            break;

        default: return false;
        }

        if (2 == lDigit)
        {
            if (6 <= lCount)
            {
                return false;
            }

            mData.mEthernet[lCount] = lValue;
            lCount++;
            lDigit = 0;
        }
    }

    if ((6 != lCount) || (0 != lDigit))
    {
        return false;
    }

    mData.mFlags.mEthernet = true;
    return true;
}

// Return
//  false  Error
//  true   OK
bool Token::ConvertToEthernetType()
{
    assert(NULL != mText   );
    assert(   0 <  mTextLen);

    unsigned int lValue;

    if (TextToUnsignedInt(mText, mTextLen, &lValue) && (0x10000 > lValue))
    {
        SetEthernetType(lValue);
        return true;
    }

    return false;
}

// Return
//  false  Error
//  true   OK
bool Token::ConvertToIndex()
{
    assert(NULL != mText   );
    assert(   0 <  mTextLen);

    unsigned int lValue;

    if (TextToUnsignedInt(mText, mTextLen, &lValue) && (0x4000 > lValue))
    {
        SetIndex(lValue);
        return true;
    }

    return false;
}

// Return
//  false  Error
//  true   OK
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
        return ConvertToIPv4Address(lA);
    }

    if (8 == sscanf_s(lText, "%x:%x:%x:%x:%x:%x:%x:%x", lA + 0, lA + 1, lA + 2, lA + 3, lA + 4, lA + 5, lA + 6, lA + 7))
    {
        return ConvertToIPv6Address(lA);
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
            case AF_INET : memcpy(mData.mIPv4, &lIPv4->sin_addr , sizeof(lIPv4->sin_addr )); mData.mFlags.mIPv4 = true, lResult = true; break;
            case AF_INET6: memcpy(mData.mIPv6, &lIPv6->sin6_addr, sizeof(lIPv6->sin6_addr)); mData.mFlags.mIPv6 = true, lResult = true; break;
            }

            lAddress = lAddress->ai_next;
        }

        freeaddrinfo(lAddress);

        return lResult;
    }

    return false;
}

// Return
//  false  Error
//  true   OK
bool Token::ConvertToIpProtocol()
{
    assert(NULL != mText   );
    assert(0    <  mTextLen);

    unsigned int lValue;

    if (TextToUnsignedInt(mText, mTextLen, &lValue) && (0x100 > lValue))
    {
        SetIpProtocol(lValue);
        return true;
    }

    return false;
}

// Return
//  false  Error
//  true   OK
bool Token::ConvertToNetwork()
{
    assert(NULL != mText  );
    assert(   0 < mTextLen);

    char lText[1024];

    memset(&lText, 0, sizeof(lText));

    memcpy(lText, mText, mTextLen);

    unsigned int lA[8];
    unsigned int lL;

    if (9 == sscanf_s(lText, "%x:%x:%x:%x:%x:%x:%x:%x/%u", lA + 0, lA + 1, lA + 2, lA + 3, lA + 4, lA + 5, lA + 6, lA + 7, &lL))
    {
        if (128 < lL) {	return false; }
        mData.mNetMaskLen        = lL  ;
        mData.mFlags.mNetMaskLen = true;
        return ConvertToIPv6Address(lA);
    }

    if (8 == sscanf_s(lText, "%x:%x:%x:%x:%x:%x:%x:%x", lA + 0, lA + 1, lA + 2, lA + 3, lA + 4, lA + 5, lA + 6, lA + 7))
    {
        mData.mNetMaskLen        =  128;
        mData.mFlags.mNetMaskLen = true;
        return ConvertToIPv6Address(lA);
    }

    if (5 == sscanf_s(lText, "%u.%u.%u.%u/%u", lA + 0, lA + 1, lA + 2, lA + 3, &lL))
    {
        if (32 < lL) { return false; }
        mData.mNetMaskLen        = lL  ;
        mData.mFlags.mNetMaskLen = true;
        return ConvertToIPv4Address(lA);
    }

    if (4 == sscanf_s(lText, "%u.%u.%u.%u", lA + 0, lA + 1, lA + 2, lA + 3))
    {
        mData.mNetMaskLen        =   32;
        mData.mFlags.mNetMaskLen = true;
        return ConvertToIPv4Address(lA);
    }

    memset(&lA, 0, sizeof(lA));
    if (4 == sscanf_s(lText, "%u.%u.%u/%u", lA + 0, lA + 1, lA + 2, &lL))
    {
        if (24 < lL) { return false; }
        mData.mNetMaskLen        = lL  ;
        mData.mFlags.mNetMaskLen = true;
        return ConvertToIPv4Address(lA);
    }

    memset(&lA, 0, sizeof(lA));
    if (3 == sscanf_s(lText, "%u.%u.%u", lA + 0, lA + 1, lA + 2))
    {
        mData.mNetMaskLen        =   24;
        mData.mFlags.mNetMaskLen = true;
        return ConvertToIPv4Address(lA);
    }

    memset(&lA, 0, sizeof(lA));
    if (3 == sscanf_s(lText, "%u.%u/%u", lA + 0, lA + 1, &lL))
    {
        if (16 < lL) { return false; }
        mData.mNetMaskLen        = lL  ;
        mData.mFlags.mNetMaskLen = true;
        return ConvertToIPv4Address(lA);
    }

    memset(&lA, 0, sizeof(lA));
    if (2 == sscanf_s(lText, "%u.%u", lA + 0, lA + 1))
    {
        mData.mNetMaskLen        =   16;
        mData.mFlags.mNetMaskLen = true;
        return ConvertToIPv4Address(lA);
    }

    memset(&lA, 0, sizeof(lA));
    if (2 == sscanf_s(lText, "%u/%u", lA + 0, &lL))
    {
        if (8 < lL) { return false; }
        mData.mNetMaskLen        = lL  ;
        mData.mFlags.mNetMaskLen = true;
        return ConvertToIPv4Address(lA);
    }

    memset(&lA, 0, sizeof(lA));
    if (1 == sscanf_s(lText, "%u", lA + 0))
    {
        mData.mNetMaskLen        =    8;
        mData.mFlags.mNetMaskLen = true;
        return ConvertToIPv4Address(lA);
    }

    return false;
}

// Return
//  false  Error
//  true   OK
bool Token::ConvertToPacketSize()
{
    assert(NULL != mText   );
    assert(   0 <  mTextLen);

    unsigned int lValue;

    if (TextToUnsignedInt(mText, mTextLen, &lValue) && (0x4000 > lValue))
    {
        mData.mPacketSize_byte   = lValue;
        mData.mFlags.mPacketSize = true  ;
        return true;
    }

    return false;
}

// Return
//  false  Error
//  true   OK
bool Token::ConvertToPort()
{
    assert(NULL != mText   );
    assert(   0 <  mTextLen);

    unsigned int lValue;

    if (TextToUnsignedInt(mText, mTextLen, &lValue) && (0x10000 > lValue))
    {
        mData.mPorts[0]    = lValue;
        mData.mFlags.mPort = true  ;
        return true;
    }

    return false;
}

// Return
//  false  Error
//  true   OK
bool Token::ConvertToPortRange()
{
    assert(NULL != mText   );
    assert(0    <  mTextLen);

    unsigned int lValues[2];

    unsigned int lIndex = TextToUnsignedInt(mText, lValues + 0);
    if ((0 < lIndex) && (mTextLen > (lIndex + 1)) && ('-' == mText[lIndex]) && TextToUnsignedInt(mText + lIndex + 1, mTextLen - lIndex - 1, lValues + 1))
    {
        for (unsigned int i = 0; i < 2; i ++)
        {
            if (0x10000 <= lValues[i])
            {
                return false;
            }

            mData.mPorts[i] = lValues[i];
        }

        mData.mFlags.mPort = true;
        return true;
    }

    return false;
}

// Return
//  false  Error
//  true   OK
bool Token::ConvertToVlanId()
{
    assert(NULL != mText   );
    assert(   0 <  mTextLen);

    unsigned int lValue;

    if (TextToUnsignedInt(mText, mTextLen, &lValue) && (0x1000 > lValue))
    {
        mData.mVlanId        = lValue;
        mData.mFlags.mVlanId = true  ;
        return true;
    }

    return false;
}

// aSrc [---;R--] Copy data from this instance
//
// Return
//  false  Error
//  true   OK
void Token::CopyData(const Token & aSrc)
{
    assert(NULL != (&aSrc));

    mData = aSrc.mData;
}

TokenList::~TokenList()
{
    while (!empty())
    {
        delete front();
        pop_front();
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

// aA [---;R--]
//
// Return
//  false  Error
//  true   OK
bool Token::ConvertToIPv4Address(const unsigned int * aA)
{
    assert(NULL != aA);

    for (unsigned int i = 0; i < 4; i++)
    {
        if (0xff < aA[i])
        {
            return false;
        }

        mData.mIPv4[i] = aA[i];
    }

    mData.mFlags.mIPv4 = true;

    return true;
}

// aA [---;R--]
//
// Return
//  false  Error
//  true   OK
bool Token::ConvertToIPv6Address(const unsigned int * aA)
{
    assert(NULL != aA);

    for (unsigned int i = 0; i < 8; i++)
    {
        if (0xffff < aA[i])
        {
            return false;
        }

        mData.mIPv6[i] = aA[i];
    }

    mData.mFlags.mIPv6 = true;

    return true;
}

// Static function
/////////////////////////////////////////////////////////////////////////////

// aText [---;R--] Text to parse
// aTextLen
// aOut  [---;-W-] Output buffer
//
// Return
//  false  Error
//  true   OK
bool TextToUnsignedInt(const char * aText, unsigned int aTextLen, unsigned int * aOut)
{
    assert(NULL != aText   );
    assert(   0 <  aTextLen);
    assert(NULL != aOut    );

    char       * lEnd  ;
    unsigned int lValue;

    if ((2 < aTextLen) && (0 == strncmp("0x", aText, 2)))
    {
        lValue = strtoul(aText + 2, &lEnd, 16);
    }
    else
    {
        lValue = strtoul(aText, &lEnd, 10);
    }

    if ((aText + aTextLen) != lEnd)
    {
        return false;
    }

    (*aOut) = lValue;
    return true;
}

// aText [---;R--] The text to parse
// aOut  [---;-W-] The output buffer
//
// Return  The len of parsed text
unsigned int TextToUnsignedInt(const char * aText, unsigned int * aOut)
{
    assert(NULL != aText);
    assert(NULL != aOut );

    char       * lEnd;
    unsigned int lValue;

    if (0 == strncmp("0x", aText, 2))
    {
        lValue = strtoul(aText + 2, &lEnd, 16);
    }
    else
    {
        lValue = strtoul(aText, &lEnd, 10);
    }

    unsigned int lResult = static_cast<unsigned int>(lEnd - aText);
    if (0 < lResult)
    {
        (*aOut) = lValue;
    }

    return lResult;
}
