
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/SearchElement.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== C ++ ===============================================================
#include <set>
#include <vector>

// ===== Import =============================================================

#define _OPEN_NET_NO_FUNCTION_
#define OPEN_NET_CONSTANT

#include <OpenNetK/RegEx.h>

// ===== Includes ===========================================================
#include <GPU_dpi/Status.h>

// ===== CodeGen ============================================================
class Filter_Internal;

// Class
/////////////////////////////////////////////////////////////////////////////

class SearchElement
{

public:

    typedef enum
    {
        TYPE_BIN  ,
        TYPE_REGEX,
        TYPE_TEXT ,

        TYPE_QTY
    }
    Type;

    static bool Compare(const SearchElement & aA, const SearchElement & aB);

    SearchElement();

    const char * Init(const char * aCode);

    void GenerateCode     (Filter_Internal * aOut, const char * aResultName, unsigned int aIndex);
    void GenerateVariables(Filter_Internal * aOut, unsigned int aIndex);

private:

    enum
    {
        STATE_QTY = 1024,
    };

    typedef struct
    {
        unsigned short mBegin;
        unsigned short mEnd  ;
        unsigned short mState;
    }
    Operation;

    bool ConvertToBin();

    void GenerateState(Filter_Internal * aOut, unsigned int aIndex);

    // ===== State ==========================================================
    void State_Add(char aC);
    void State_Add(const RegEx_State & aState);
    bool State_Add(const char * aText, const Operation * aOps, unsigned int aOpCount);
    bool State_Add(const char * aText, unsigned int aSize_byte, const RegEx_State & aFinal, bool aOr);

    bool State_Add_7d(const char * aText, unsigned int aSize_byte, unsigned int * aOffset);

    unsigned int mBegin            ;
    unsigned int mEnd              ;
    unsigned int mStateCount       ;
    RegEx_State  mStates[STATE_QTY];
    char         mString[      256];
    unsigned int mStringSize_byte  ;
    Type         mType             ;

};
