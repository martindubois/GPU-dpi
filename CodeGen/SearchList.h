
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/SearchList.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== C ++ ===============================================================
#include <list>

// ===== CodeGen ============================================================
#include "SearchElement.h"

class Filter_Internal;

// Class
/////////////////////////////////////////////////////////////////////////////

class SearchList
{

public:

    SearchList();

    const char * Init(const char * aIn, unsigned int aInSize_byte);

    const char * Generate(const char * aResultName, Filter_Internal * aOut);

private:

    typedef std::list<SearchElement> ElementList;

    void GenerateCodes    (Filter_Internal * aOut, const char * aResultName);
    void GenerateVariables(Filter_Internal * aOut);

    ElementList mElements;

};
