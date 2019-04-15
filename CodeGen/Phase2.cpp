
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
static Token * Operator   (Token * aA   , Token * aOperator, Token * aB    );
static Token * Parenthesis(Token * aOpen, Token * aToken   , Token * aClose);
static Token * Port_Range (Token * aA   , Token * aOperator, Token * aB    );

// Constants
/////////////////////////////////////////////////////////////////////////////

static const TokenSeq SEQ[]
{
    { { TOKEN_BOOLEAN         , TOKEN_OPERATOR, TOKEN_BOOLEAN          }, Operator   , "ERROR 2.1 - Invalid and expression" },
    { { TOKEN_DATA            , TOKEN_DASH    , TOKEN_DATA             }, Port_Range , "ERROR 2.2 - Invalid port range"     },
    { { TOKEN_PARENTHESIS_OPEN, TOKEN_ANY     , TOKEN_PARENTHESIS_CLOSE}, Parenthesis, "ERROR 2.2 - Invalid parenthesis"    },
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

Token * Operator(Token * aA, Token * aOperator, Token * aB)
{
    assert(NULL != aA       );
    assert(NULL != aOperator);
    assert(NULL != aB       );

    aOperator->SetA(aA);
    aOperator->SetB(aB);

    aOperator->SetBoolean();

    return aOperator;
}

Token * Parenthesis(Token * aOpen, Token * aToken, Token * aClose)
{
    assert(NULL != aOpen );
    assert(NULL != aToken);
    assert(NULL != aClose);

    delete aOpen ;
    delete aClose;

    return aToken;
}

Token * Port_Range(Token * aA, Token * aDash, Token * aB)
{
    assert(NULL != aA   );
    assert(NULL != aDash);
    assert(NULL != aB   );

    Token * lResult = NULL;

    if (aA->ConvertToUInt16() && aB->ConvertToUInt16())
    {
        aDash->mId = TOKEN_PORT_RANGE;

        aDash->mData_UInt16[0] = aA->mData_UInt16[0];
        aDash->mData_UInt16[1] = aB->mData_UInt16[0];

        lResult = aDash;
    }

    delete aA;
    delete aB;

    if (NULL == lResult)
    {
        delete aDash;
    }

    return lResult;
}
