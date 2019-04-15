
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Filter.cpp

// TEST COVERAGE  2019-04-11  KMS - Martin Dubois, ing.

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== Common =============================================================
#include "../Common/Filter.h"

// ===== CodeGen ============================================================
#include "Phase0.h"
#include "Phase1.h"
#include "Phase2.h"
#include "Phase3.h"
#include "Token.h"

#include "Filter_Internal.h"

// Public
/////////////////////////////////////////////////////////////////////////////

Filter * Filter::Create(GPU_dpi::OutputType aOutputType, GPU_dpi::ForwardType aForwardType, const char * aIn)
{
    Filter_Internal * lResult = new Filter_Internal(aOutputType, aForwardType, "FilterFunction");

    TokenList lTokenList0;

    Phase0(aIn, &lTokenList0);

    if (0 == lTokenList0.size())
    {
        lResult->SetErrorMessage("ERROR 1 - Empty filter");
        return lResult;
    }

    TokenList lTokenList1;

    while (1 < lTokenList0.size())
    {
        size_t lCount = lTokenList0.size();

        const char * lErrMsg = Phase1(&lTokenList0, &lTokenList1);
        if (NULL != lErrMsg)
        {
            lResult->SetErrorMessage(lErrMsg);
            return lResult;
        }

        lErrMsg = Phase2(&lTokenList1, &lTokenList0);
        if (NULL != lErrMsg)
        {
            lResult->SetErrorMessage(lErrMsg);
            return lResult;
        }

        if (lTokenList0.size() == lCount)
        {
            lResult->SetErrorMessage("ERROR 2 - Invalid filter");
            return lResult;
        }
    }

    Phase3(lTokenList0, lResult);

    return lResult;
}

void Filter::Delete()
{
    assert(NULL != this);

    delete this;
}

// Protected
/////////////////////////////////////////////////////////////////////////////

Filter::Filter()
{
}

Filter::~Filter()
{
}
