
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/SearchList.cpp

// CODE COVERAGE  2019-05-08  KMS - Martin Dubois, ing.

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== CodeGen ============================================================
#include "Filter_Internal.h"

#include "SearchList.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static const char * ExtractElement(const char * aIn, unsigned int * aInSize_byte, char * aElement);

// Public
/////////////////////////////////////////////////////////////////////////////

SearchList::SearchList()
{
}

// aIn [---;R--] The text to parse
// aInSize_byte
//
// Return
//  NULL   OK
//  Ohter  Error message
const char * SearchList::Init(const char * aIn, unsigned int aInSize_byte)
{
    assert(NULL != aIn         );
    assert(   0 <  aInSize_byte);

    char         lElement[1024];
    const char * lIn          = aIn         ;
    unsigned int lInSize_byte = aInSize_byte;

    while (NULL != (lIn = ExtractElement(lIn, &lInSize_byte, lElement)))
    {
        SearchElement lE;

        const char * lErrMsg = lE.Init(lElement);
        if (NULL != lErrMsg)
        {
            return lErrMsg;
        }

        mElements.push_back(lE);
    }

    mElements.sort(SearchElement::Compare);

    return NULL;
}

// aResultName [---;R--]
// aOut        [---;RW-]
//
// Return
//  NULL   OK
//  Other  Error message
const char * SearchList::Generate(const char * aResultName, Filter_Internal * aOut)
{
    assert(NULL != aResultName);
    assert(NULL != aOut       );

    GenerateVariables(aOut);

    char lLine[1024];

    sprintf_s(lLine, "for ( unsigned int i = 0; ( i < lPacketInfo->mSize_byte ) && ( ! %s ); i ++ )" EOL, aResultName);
    aOut->Append(lLine);
    aOut->Append("{" EOL);
    aOut->Indent_Inc();

        GenerateCodes(aOut, aResultName);

    aOut->C_for_End();

    return NULL;
}

// Privates
/////////////////////////////////////////////////////////////////////////////

// aOut        [---;RW-]
// aResultName [---;R--]
void SearchList::GenerateCodes(Filter_Internal * aOut, const char * aResultName)
{
    assert(NULL != aOut       );
    assert(NULL != aResultName);

    unsigned int lIndex = 0;
    for (ElementList::iterator lIt = mElements.begin(); lIt != mElements.end(); lIt++)
    {
        lIt->GenerateCode(aOut, aResultName, lIndex);
        lIndex++;
    }
}

// aOut [---;RW-]
void SearchList::GenerateVariables(Filter_Internal * aOut)
{
    assert(NULL != aOut);

    unsigned int lIndex = 0;
    for (ElementList::iterator lIt = mElements.begin(); lIt != mElements.end(); lIt++)
    {
        lIt->GenerateVariables(aOut, lIndex);
        lIndex++;
    }
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

// aIn      [---;R--]
// aInSize_byte
// aElement [---;-W-]
//
// Return  Where to look for the next element
const char * ExtractElement(const char * aIn, unsigned int * aInSize_byte, char * aElement)
{
    assert(NULL != aIn         );
    assert(NULL != aInSize_byte);
    assert(NULL != aElement    );

    const char * lIn          = aIn;
    unsigned int lInSize_byte = (*aInSize_byte);

    if (0 >= lInSize_byte)
    {
        return NULL;
    }

    for (unsigned int i = 0; i < lInSize_byte; i++)
    {
        if (';' == lIn[i])
        {
            aElement[i] = '\0';
            (*aInSize_byte) = lInSize_byte - i - 1;
            return lIn + i + 1;
        }

        aElement[i] = lIn[i];
    }

    aElement[lInSize_byte] = '\0';
    (*aInSize_byte) = 0;
    return lIn + lInSize_byte;
}
