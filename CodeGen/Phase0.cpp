
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Phase0.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== CodeGen ============================================================
#include "Token.h"

#include "Phase0.h"

// Static function declaration
/////////////////////////////////////////////////////////////////////////////

static unsigned int CountLetterNumberOrPoint(const char * aIn);

// ===== Is =================================================================
static bool IsBlank              (char aC);
static bool IsFalse              (char aC);
static bool IsLetter             (char aC);
static bool IsLetterNumberOrPoint(char aC);
static bool IsNumber             (char aC);

// Data types
/////////////////////////////////////////////////////////////////////////////

typedef bool(*IsFunction)(char aC);

typedef struct
{
    TokenId      mId      ;
    bool         mOperator;
    const char * mText    ;
    unsigned int mLen     ;
    IsFunction   mFunction;
}
Text;

// Constants
/////////////////////////////////////////////////////////////////////////////

static const Text TEXT[]
{
    { TOKEN_AND              , true , "&&"       , 2, IsFalse               },
    { TOKEN_AND              , true , "and"      , 3, IsLetterNumberOrPoint },
    { TOKEN_DASH             , true , "-"        , 1, IsFalse               },
    { TOKEN_DST              , false, "dst"      , 3, IsLetterNumberOrPoint },
    { TOKEN_GREATER          , false, "greater"  , 7, IsLetterNumberOrPoint },
    { TOKEN_HOST             , false, "host"     , 4, IsLetterNumberOrPoint },
    { TOKEN_ICMP             , false, "icmp"     , 4, IsLetterNumberOrPoint },
    { TOKEN_IP               , false, "ip"       , 2, IsLetterNumberOrPoint },
    { TOKEN_IP6              , false, "ip6"      , 3, IsLetterNumberOrPoint },
    { TOKEN_LESS             , false, "less"     , 4, IsLetterNumberOrPoint },
    { TOKEN_NOT              , false, "!"        , 1, IsFalse               },
    { TOKEN_NOT              , false, "not"      , 3, IsLetterNumberOrPoint },
    { TOKEN_OR               , true , "||"       , 2, IsFalse               },
    { TOKEN_OR               , true , "or"       , 2, IsLetterNumberOrPoint },
    { TOKEN_PARENTHESIS_CLOSE, false, ")"        , 1, IsFalse               },
    { TOKEN_PARENTHESIS_OPEN , false, "("        , 1, IsFalse               },
    { TOKEN_PORT             , false, "port"     , 4, IsLetterNumberOrPoint },
    { TOKEN_PORTRANGE        , false, "portrange", 9, IsLetterNumberOrPoint },
    { TOKEN_PROTO            , false, "proto"    , 5, IsLetterNumberOrPoint },
    { TOKEN_SRC              , false, "src"      , 3, IsLetterNumberOrPoint },
    { TOKEN_TCP              , false, "tcp"      , 3, IsLetterNumberOrPoint },
    { TOKEN_UDP              , false, "udp"      , 3, IsLetterNumberOrPoint },
};

#define TEXT_COUNT ( sizeof( TEXT ) / sizeof( TEXT[ 0 ] ) )

// Function
/////////////////////////////////////////////////////////////////////////////

void Phase0(const char * aIn, TokenList * aOut)
{
    assert(NULL != aIn );
    assert(NULL != aOut);

    unsigned int lIndex = 0;

    while ('\0' != aIn[lIndex])
    {
        if (IsBlank(aIn[lIndex]))
        {
            lIndex++;
        }
        else
        {
            Token * lT = NULL;

            for (unsigned int i = 0; i < TEXT_COUNT; i++)
            {
                const Text & lText = TEXT[i];

                if ((0 == strncmp(aIn + lIndex, lText.mText, lText.mLen)) && (!lText.mFunction(aIn[lIndex + lText.mLen])))
                {
                    lT = new Token(lText.mId, aIn + lIndex, lText.mLen);
                    assert(NULL != lT);

                    if (lText.mOperator)
                    {
                        lT->SetOperator();
                    }

                    lIndex += lText.mLen;

                    break;
                }
            }

            if (NULL == lT)
            {
                lT = new Token(TOKEN_DATA, aIn + lIndex, CountLetterNumberOrPoint(aIn + lIndex));
                assert(NULL != lT);

                lIndex += lT->mTextLen;
            }

            aOut->push_back(lT);
        }
    }
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

unsigned int CountLetterNumberOrPoint(const char * aIn)
{
    unsigned int lResult = 0;

    while (IsLetterNumberOrPoint(aIn[lResult]))
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

bool IsFalse(char)
{
    return false;
}

bool IsLetter(char aC)
{
    return (('a' <= aC) && ('z' >= aC));
}

bool IsLetterNumberOrPoint(char aC)
{
    return (IsLetter(aC) || IsNumber(aC) || ('.' == aC));
}

bool IsNumber(char aC)
{
    return (('0' <= aC) && ('9' >= aC));
}
