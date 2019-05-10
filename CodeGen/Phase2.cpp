
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Phase2.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== CodeGen ============================================================
#include "Token.h"

#include "Phase2.h"

// Data types
/////////////////////////////////////////////////////////////////////////////

typedef Token * (*SeqFunction)(Token *, Token *, Token *);

typedef struct
{
    TokenId      mIds    [3];
    SeqFunction  mFunction    ;
    const char * mErrorMessage;
}
TokenSeq;

// Static function declaration
/////////////////////////////////////////////////////////////////////////////

// ===== Seq ================================================================
static Token * Brackets_Data (Token * aOpen, Token * aData    , Token * aClose);
static Token * Brackets_Index(Token * aOpen, Token * aIndx    , Token * aClose);
static Token * Brackets_Value(Token * aOpen, Token * aValue   , Token * aClose);
static Token * Index         (Token * aA   , Token * aColon   , Token * aB    );
static Token * IndexValue    (Token * aA   , Token * aColon   , Token * aB    );
static Token * Operator      (Token * aA   , Token * aOperator, Token * aB    );
static Token * Parentheses   (Token * aOpen, Token * aToken   , Token * aClose);

// Constants
/////////////////////////////////////////////////////////////////////////////

static const TokenSeq SEQ[]
{
    { { TOKEN_BOOLEAN        , TOKEN_OPERATOR, TOKEN_BOOLEAN         }, Operator      , NULL                                    },
    { { TOKEN_BRACKET_OPEN   , TOKEN_DATA    , TOKEN_BRACKET_CLOSE   }, Brackets_Data , "ERROR 2.1 - Invalid brackets"          },
    { { TOKEN_BRACKET_OPEN   , TOKEN_INDEX   , TOKEN_BRACKET_CLOSE   }, Brackets_Index, NULL                                    },
    { { TOKEN_BRACKET_OPEN   , TOKEN_VALUE   , TOKEN_BRACKET_CLOSE   }, Brackets_Value, NULL                                    },
    { { TOKEN_DATA           , TOKEN_COLON   , TOKEN_DATA            }, Index         , "ERROR 2.2 - Invalid access expression" },
    { { TOKEN_VALUE          , TOKEN_COLON   , TOKEN_DATA            }, IndexValue    , "ERROR 2.3 - Invalid access expression" },
    { { TOKEN_VALUE          , TOKEN_OPERATOR, TOKEN_DATA            }, Operator      , NULL                                    },
    { { TOKEN_PARENTHESE_OPEN, TOKEN_ANY     , TOKEN_PARENTHESE_CLOSE}, Parentheses   , NULL                                    },
};

#define SEQ_COUNT ( sizeof( SEQ ) / sizeof( SEQ[ 0 ] ) )

// Function
/////////////////////////////////////////////////////////////////////////////

const char * Phase2(TokenList * aIn, TokenList * aOut)
{
    assert(NULL != aIn);
    assert(NULL != aOut);

    if (!aIn->empty())
    {
        Token * lT2 = aIn->front(); aIn->pop_front();
        assert(NULL != lT2);

        if (!aIn->empty())
        {
            Token * lT1 = lT2;

            lT2 = aIn->front(); aIn->pop_front();
            assert(NULL != lT2);

            while (!aIn->empty())
            {
                Token * lT0 = lT1;

                lT1 = lT2;
                lT2 = aIn->front(); aIn->pop_front();
                assert(NULL != lT2);

                unsigned int i;
                for (i = 0; i < SEQ_COUNT; i++)
                {
                    const TokenSeq & lSeq = SEQ[i];
                    if (   lT0->Is(lSeq.mIds[0])
                        && lT1->Is(lSeq.mIds[1])
                        && lT2->Is(lSeq.mIds[2]))
                    {
                        Token * lT = lSeq.mFunction(lT0, lT1, lT2);
                        if (NULL == lT)
                        {
                            return lSeq.mErrorMessage;
                        }

                        lT1 = lT;

                        if (!aIn->empty())
                        {
                            lT2 = aIn->front(); aIn->pop_front();
                            assert(NULL != lT2);
                        }
                        else
                        {
                            lT2 = NULL;
                        }

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

        if (NULL != lT2)
        {
            aOut->push_back(lT2);
        }
    }

    return NULL;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

// ===== Seq3 ===============================================================

Token * Brackets_Data(Token * aOpen, Token * aData, Token * aClose)
{
    assert(NULL != aOpen );
    assert(NULL != aData );
    assert(NULL != aClose);

    delete aClose;
    delete aOpen ;

    Token * lResult;

    if (aData->ConvertToIndex())
    {
        aData->mId = TOKEN_INDEX;
        aData->SetAccessSize(1);

        lResult = aData;
    }
    else
    {
        delete aData;
        lResult = NULL;
    }

    return lResult;
}

Token * Brackets_Index(Token * aOpen, Token * aIndex, Token * aClose)
{
    assert(NULL != aOpen );
    assert(NULL != aIndex);
    assert(NULL != aClose);

    delete aClose;
    delete aOpen ;

    return aIndex;
}

Token * Brackets_Value(Token * aOpen, Token * aValue, Token * aClose)
{
    assert(NULL != aOpen );
    assert(NULL != aValue);
    assert(NULL != aClose);

    delete aClose;
    delete aOpen ;

    aValue->mId = TOKEN_INDEX;
    aValue->ClearValue   ();

    if (!aValue->mData.mFlags.mAccessSize)
    {
        aValue->SetAccessSize(1);
    }

    return aValue;
}

Token * Index(Token * aA, Token * aIndex, Token * aB)
{
    assert(NULL != aA    );
    assert(NULL != aIndex);
    assert(NULL != aB    );

    Token * lResult;

    if (aA->ConvertToIndex() && aB->ConvertToAccessSize())
    {
        aIndex->SetIndex     (aA->mData.mIndex          );
        aIndex->SetAccessSize(aB->mData.mAccessSize_byte);
        aIndex->mId = TOKEN_INDEX;
        lResult = aIndex;
    }
    else
    {
        delete aIndex;
        lResult = NULL;
    }

    delete aA;
    delete aB;

    return lResult;
}

Token * IndexValue(Token * aA, Token * aIndex, Token * aB)
{
    assert(NULL != aA    );
    assert(NULL != aIndex);
    assert(NULL != aB    );

    Token * lResult;

    if (aB->ConvertToAccessSize())
    {
        aA->SetAccessSize(aB->mData.mAccessSize_byte);
        lResult = aA;
    }
    else
    {
        delete aA;
        lResult = NULL;
    }

    delete aIndex;
    delete aB    ;

    return lResult;
}

Token * Operator(Token * aA, Token * aOperator, Token * aB)
{
    assert(NULL != aA       );
    assert(NULL != aOperator);
    assert(NULL != aB       );

    aOperator->SetA(aA);
    aOperator->SetB(aB);

    return aOperator;
}

Token * Parentheses(Token * aOpen, Token * aToken, Token * aClose)
{
    assert(NULL != aOpen );
    assert(NULL != aToken);
    assert(NULL != aClose);

    delete aOpen ;
    delete aClose;

    return aToken;
}
