
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Phase0.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <string.h>

// ===== CodeGen ============================================================
#include "Token.h"

#include "Phase0.h"

// Static function declaration
/////////////////////////////////////////////////////////////////////////////

static unsigned int CountBefore                           (const char * aIn, char aC);
static unsigned int CountBefore_OutOfString               (const char * aIn, char aC);
static unsigned int CountColonDashLetterNumberOrPoint     (const char * aIn);
static unsigned int CountColonDashLetterNumberPointOrSlash(const char * aIn);
static unsigned int CountDashLetterNumberOrPoint          (const char * aIn);
static unsigned int CountDashOrHexNumber                  (const char * aIn);
static unsigned int CountHexNumber                        (const char * aIn);

#define COUNT_DEFAULT CountDashLetterNumberOrPoint

// ===== Is =================================================================
static bool IsBlank                  (char aC);
static bool IsDashLetterNumberOrPoint(char aC);
static bool IsFalse                  (char aC);
static bool IsHexNumber              (char aC);
static bool IsLetter                 (char aC);
static bool IsNumber                 (char aC);

#define IS_DEFAULT IsDashLetterNumberOrPoint

// Data types
/////////////////////////////////////////////////////////////////////////////

typedef unsigned int (*CountFunction)(const char * aIn);
typedef bool         (*IsFunction   )(char aC);

typedef struct
{
    TokenId       mId           ;
    bool          mBoolean      ;
    const char  * mCode         ;
    bool          mOperator     ;
    bool          mValue        ;
    const char *  mText         ;
    unsigned int  mLen          ;
    IsFunction    mIsFunction   ;
    CountFunction mCountFunction;
}
Text;

// Constants
/////////////////////////////////////////////////////////////////////////////

#define NOT_FOUND (0xffffffff)

static const Text TEXT[]
{
    { TOKEN_BROADCAST        , true , NULL, false, false, "broadcast", 9, IS_DEFAULT, COUNT_DEFAULT       },
    { TOKEN_MULTICAST        , true , NULL, false, false, "multicast", 9, IS_DEFAULT, COUNT_DEFAULT       },
    { TOKEN_PORTRANGE        , false, NULL, false, false, "portrange", 9, IS_DEFAULT, CountDashOrHexNumber},

    { TOKEN_IN_ETHERNET      , true , NULL, false, false, "in_ether" , 8, IS_DEFAULT, COUNT_DEFAULT },

    { TOKEN_GREATER          , false, NULL, false, false, "greater"  , 7, IS_DEFAULT, CountHexNumber },
    { TOKEN_INBOUND          , true , NULL, false, false, "inbound"  , 7, IsLetter  , COUNT_DEFAULT  },

    { TOKEN_IN_TCP           , true , NULL, false, false, "in_tcp"   , 6, IS_DEFAULT, COUNT_DEFAULT },
    { TOKEN_IN_UDP           , true , NULL, false, false, "in_udp"   , 6, IS_DEFAULT, COUNT_DEFAULT },

    { TOKEN_ETHER            , false, NULL, false, false, "ether"    , 5, IS_DEFAULT, CountColonDashLetterNumberOrPoint },
    { TOKEN_ICMP6            , false, NULL, false, false, "icmp6"    , 5, IS_DEFAULT, COUNT_DEFAULT                     },
    { TOKEN_IN_IP            , true , NULL, false, false, "in_ip"    , 5, IS_DEFAULT, COUNT_DEFAULT                     },
    { TOKEN_PROTO            , false, NULL, false, false, "proto"    , 5, IS_DEFAULT, COUNT_DEFAULT                     },

    { TOKEN_HOST             , false, NULL, false, false, "host"     , 4, IS_DEFAULT, NULL           },
    { TOKEN_ICMP             , false, NULL, false, false, "icmp"     , 4, IS_DEFAULT, COUNT_DEFAULT  },
    { TOKEN_IGMP             , false, NULL, false, false, "igmp"     , 4, IS_DEFAULT, COUNT_DEFAULT  },
    { TOKEN_LESS             , false, NULL, false, false, "less"     , 4, IS_DEFAULT, CountHexNumber },
    { TOKEN_PORT             , false, NULL, false, false, "port"     , 4, IS_DEFAULT, CountHexNumber },
    { TOKEN_RARP             , false, NULL, false, false, "rarp"     , 4, IS_DEFAULT, COUNT_DEFAULT  },
    { TOKEN_VLAN             , true , NULL, false, false, "vlan"     , 4, IS_DEFAULT, CountHexNumber },
    { TOKEN_VRRP             , false, NULL, false, false, "vrrp"     , 4, IS_DEFAULT, COUNT_DEFAULT  },

    { TOKEN_AND              , true , NULL                     , true , false, "and"      , 3, IS_DEFAULT, COUNT_DEFAULT                          },
    { TOKEN_ARP              , false, NULL                     , false, false, "arp"      , 3, IS_DEFAULT, COUNT_DEFAULT                          },
    { TOKEN_DST              , false, NULL                     , false, false, "dst"      , 3, IS_DEFAULT, NULL                                   },
    { TOKEN_ESP              , false, NULL                     , false, false, "esp"      , 3, IS_DEFAULT, COUNT_DEFAULT                          },
    { TOKEN_IP6              , false, NULL                     , false, false, "ip6"      , 3, IS_DEFAULT, CountColonDashLetterNumberPointOrSlash },
    { TOKEN_LEN              , false, "lPacketInfo->mSize_byte", false, true , "len"      , 3, IS_DEFAULT, COUNT_DEFAULT                          },
    { TOKEN_NET              , false, NULL                     , false, false, "net"      , 3, IS_DEFAULT, NULL                                   },
    { TOKEN_NOT              , false, NULL                     , false, false, "not"      , 3, IS_DEFAULT, COUNT_DEFAULT                          },
    { TOKEN_PIM              , false, NULL                     , false, false, "pim"      , 3, IS_DEFAULT, COUNT_DEFAULT                          },
    { TOKEN_SRC              , false, NULL                     , false, false, "src"      , 3, IS_DEFAULT, NULL                                   },
    { TOKEN_TCP              , false, NULL                     , false, false, "tcp"      , 3, IS_DEFAULT, COUNT_DEFAULT                          },
    { TOKEN_UDP              , false, NULL                     , false, false, "udp"      , 3, IS_DEFAULT, COUNT_DEFAULT                          },

    { TOKEN_ABOVE_OR_EQUAL   , true , ">=", true , false, ">=", 2, IsFalse   , COUNT_DEFAULT                          },
    { TOKEN_AND              , true , NULL, true , false, "&&", 2, IsFalse   , COUNT_DEFAULT                          },
    { TOKEN_AH               , false, NULL, false, false, "ah", 2, IS_DEFAULT, COUNT_DEFAULT                          },
    { TOKEN_BELOW_OR_EQUAL   , true , "<=", true , false, "<=", 2, IsFalse   , COUNT_DEFAULT                          },
    { TOKEN_IP               , false, NULL, false, false, "ip", 2, IS_DEFAULT, CountColonDashLetterNumberPointOrSlash },
    { TOKEN_NOT_EQUAL        , true , "!=", true , false, "!=", 2, IsFalse   , COUNT_DEFAULT                          },
    { TOKEN_OR               , true , NULL, true , false, "||", 2, IsFalse   , COUNT_DEFAULT                          },
    { TOKEN_OR               , true , NULL, true , false, "or", 2, IS_DEFAULT, COUNT_DEFAULT                          },
    { TOKEN_SHIFT_LEFT       , false, "<<", true , true , "<<", 2, IsFalse   , COUNT_DEFAULT                          },
    { TOKEN_SHIFT_RIGHT      , false, ">>", true , true , ">>", 2, IsFalse   , COUNT_DEFAULT                          },

    { TOKEN_ABOVE            , true , ">" , true , false, ">" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_ADD              , false, "+" , true , true , "+" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_BELOW            , true , "<" , true , false, "<" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_BIT_AND          , false, "&" , true , true , "&" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_BIT_OR           , false, "|" , true , true , "|" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_BIT_XOR          , false, "^" , true , true , "^" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_BRACKET_CLOSE    , false, NULL, false, false, "]" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_BRACKET_OPEN     , false, NULL, false, false, "[" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_COLON            , false, NULL, false, false, ":" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_DASH             , false, "-" , true , true , "-" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_DIV              , false, "/" , true , true , "/" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_EQUAL            , true , "==", true , false, "=" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_MOD              , false, "%" , true , true , "%" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_MUL              , false, "*" , true , true , "*" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_NOT              , false, NULL, false, false, "!" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_PARENTHESE_CLOSE , false, NULL, false, false, ")" , 1, IsFalse, COUNT_DEFAULT },
    { TOKEN_PARENTHESE_OPEN  , false, NULL, false, false, "(" , 1, IsFalse, COUNT_DEFAULT },
};

#define TEXT_COUNT ( sizeof( TEXT ) / sizeof( TEXT[ 0 ] ) )

// Function
/////////////////////////////////////////////////////////////////////////////

const char * Phase0(const char * aIn, TokenList * aOut)
{
    assert(NULL != aIn );
    assert(NULL != aOut);

    CountFunction lCountFunction = COUNT_DEFAULT;

    unsigned int lIndex = 0;

    while ('\0' != aIn[lIndex])
    {
        if (IsBlank(aIn[lIndex]))
        {
            lIndex++;
        }
        else if ('{' == aIn[lIndex])
        {
            unsigned int lLen = CountBefore_OutOfString(aIn + lIndex + 1, '}');
            if ((NOT_FOUND == lLen) || (0 >= lLen))
            {
                return "ERROR 0.1 - Invalid list";
            }

            Token * lT = new Token(TOKEN_LIST, aIn + lIndex + 1, lLen);
            assert(NULL != lT);

            lT->SetBoolean();

            lIndex += lLen + 2;

            aOut->push_back(lT);
        }
        else
        {
            Token * lT = NULL;

            for (unsigned int i = 0; i < TEXT_COUNT; i++)
            {
                const Text & lText = TEXT[i];

                if ((0 == strncmp(aIn + lIndex, lText.mText, lText.mLen)) && (!lText.mIsFunction(aIn[lIndex + lText.mLen])))
                {
                    if (NULL != lText.mCountFunction)
                    {
                        lCountFunction = lText.mCountFunction;
                    }

                    lT = new Token(lText.mId, aIn + lIndex, lText.mLen);
                    assert(NULL != lT);

                    if (        lText.mBoolean ) { lT->SetBoolean (); }
                    if (NULL != lText.mCode    ) { lT->SetCode    (lText.mCode); }
                    if (        lText.mOperator) { lT->SetOperator(); }
                    if (        lText.mValue   ) { lT->SetValue   (); }

                    lIndex += lText.mLen;

                    break;
                }
            }

            if (NULL == lT)
            {
                unsigned int lCount = lCountFunction(aIn + lIndex);
                if (0 >= lCount)
                {
                    return "ERROR 0.2 - Invalid charactere";
                }

                lT = new Token(TOKEN_DATA, aIn + lIndex, lCount);
                assert(NULL != lT);

                lIndex += lT->mTextLen;
            }

            aOut->push_back(lT);
        }
    }

    return NULL;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

unsigned int CountBefore(const char * aIn, char aC)
{
    assert(NULL != aIn);

    unsigned int lResult = 0;

    while (aC != aIn[lResult])
    {
        switch (aIn[lResult])
        {
        case '\0': return NOT_FOUND;
        }

        lResult++;
    }

    return lResult;
}

unsigned int CountBefore_OutOfString(const char * aIn, char aC)
{
    assert(NULL != aIn);

    unsigned int lResult = 0;

    while (aC != aIn[lResult])
    {
        switch (aIn[lResult])
        {
        case '\0': return NOT_FOUND;
        case '\"':
            lResult++;
            unsigned int lLen = CountBefore(aIn + lResult, '\"');
            if (NOT_FOUND == lLen)
            {
                return NOT_FOUND;
            }
            lResult += lLen;
            break;
        }

        lResult++;
    }

    return lResult;
}

unsigned int CountColonDashLetterNumberOrPoint(const char * aIn)
{
    assert(NULL != aIn);

    unsigned int lResult = 0;

    while ((':' == aIn[lResult]) || IsDashLetterNumberOrPoint(aIn[lResult]))
    {
        lResult++;
    }

    return lResult;
}

unsigned int CountColonDashLetterNumberPointOrSlash(const char * aIn)
{
    assert(NULL != aIn);

    unsigned int lResult = 0;

    while ((':' == aIn[lResult]) || IsDashLetterNumberOrPoint(aIn[lResult]) || ('/' == aIn[lResult]))
    {
        lResult++;
    }

    return lResult;
}

unsigned int CountDashLetterNumberOrPoint(const char * aIn)
{
    assert(NULL != aIn);

    unsigned int lResult = 0;

    while (IsDashLetterNumberOrPoint(aIn[lResult]))
    {
        lResult++;
    }

    return lResult;
}

unsigned int CountDashOrHexNumber(const char * aIn)
{
    assert(NULL != aIn);

    unsigned int lResult = 0;

    while (('-' == aIn[lResult]) || IsHexNumber(aIn[lResult]))
    {
        lResult++;
    }

    return lResult;
}

unsigned int CountHexNumber(const char * aIn)
{
    assert(NULL != aIn);

    unsigned int lResult = 0;

    while (IsHexNumber(aIn[lResult]))
    {
        lResult++;
    }

    return lResult;
}

// ===== Is =================================================================

bool IsBlank(char aC)
{
    switch (aC)
    {
    case ' ':
    case '\n':
    case '\r':
    case '\t':
        return true;
    }

    return false;
}

bool IsDashLetterNumberOrPoint(char aC)
{
    return (('-' == aC) || IsLetter(aC) || IsNumber(aC) || ('.' == aC));
}

bool IsFalse(char)
{
    return false;
}

bool IsHexNumber(char aC)
{
    if (IsNumber(aC))
    {
        return true;
    }

    switch (aC)
    {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'x':
        return true;
    }

    return false;
}

bool IsLetter(char aC)
{
    return (('a' <= aC) && ('z' >= aC));
}

bool IsNumber(char aC)
{
    return (('0' <= aC) && ('9' >= aC));
}
