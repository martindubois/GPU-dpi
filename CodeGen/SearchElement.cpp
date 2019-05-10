
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/SearchElement.cpp

// CODE COVERAGE  2019-05-08  KMS - Martin Dubois, ing.

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include "string.h"

// ===== CodeGen ============================================================
#include "Filter_Internal.h"

#include "SearchElement.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

static const RegEx_State STATE_OR_END = REG_EX_STATE_OR_END;
static const RegEx_State STATE_START  = REG_EX_STATE_START ;

// Public
/////////////////////////////////////////////////////////////////////////////

// Used with standard sort method
//
// aA [---;R--]
// aB [---;R--]
//
// Return
//  false  B must be placed before A
//  true   A must be placed before B
bool SearchElement::Compare(const SearchElement & aA, const SearchElement & aB)
{
    assert(NULL != (&aA));
    assert(NULL != (&aB));

    return (aA.mBegin < aB.mBegin);
}

SearchElement::SearchElement() : mStateCount(0)
{
    memset(&mStates, 0, sizeof(mStates));
    memset(&mString, 0, sizeof(mString));
}

// aCode [---;R--]
//
// Return
//  NULL   OK
//  Other  Error message
const char * SearchElement::Init(const char * aCode)
{
    assert(NULL != aCode);

    char lType[8];

    memset(&lType, 0, sizeof(lType));

    switch (sscanf_s(aCode, " %[BEGINRTX] \"%[^\"]\" %u %u", lType SIZE_INFO(static_cast<unsigned int>(sizeof(lType) - 1)), mString SIZE_INFO(static_cast<unsigned int>(sizeof(mString) - 1)), &mBegin, &mEnd))
    {
    case 2:
        mBegin = 0;
        // no break;
    case 3:
        mEnd = 0;
        // no break;
    case 4:
        break;

    default: return "ERROR S.1 - Invalid search element";
    }

    mStringSize_byte = static_cast<unsigned int>(strlen(mString));

    if (0 == strcmp("BIN", lType))
    {
        if (!ConvertToBin()) { return "ERROR S.2 - Invalid binary search string"; }
        mType = TYPE_BIN;
    }
    else if (0 == strcmp("REGEX", lType))
    {
        RegEx_State lOK = REG_EX_STATE_OK;
        if (!State_Add(mString, mStringSize_byte, lOK, false)) { return "ERROR S.3 - Invalid regular expression"; }
        mType = TYPE_REGEX;
    }
    else if (0 == strcmp("TEXT", lType)) { mType = TYPE_TEXT; }
    else { return "ERROR S.4 - Invalid search element type"; }

    return NULL;
}

// aOut [---;RW-]
// aIndex         The state index
void SearchElement::GenerateState(Filter_Internal * aOut, unsigned int aIndex)
{
    const RegEx_State & lS = mStates[aIndex];

    const char * lN;
    char         lName[8];

    switch (lS.mC)
    {
    case REG_EX_DIGIT    : lN = "REG_EX_DIGIT"    ; break;
    case REG_EX_DIGIT_NOT: lN = "REG_EX_DIGIT_NOT"; break;
    case REG_EX_DOT      : lN = "REG_EX_DOT"      ; break;
    case REG_EX_END      : lN = "REG_EX_END"      ; break;
    case REG_EX_GROUP    : lN = "REG_EX_GROUP"    ; break;
    case REG_EX_OK       : lN = "REG_EX_OK"       ; break;
    case REG_EX_OR_END   : lN = "REG_EX_OR_END"   ; break;
    case REG_EX_OR_FAST  : lN = "REG_EX_OR_FAST"  ; break;
    case REG_EX_OR_NOT   : lN = "REG_EX_OR_NOT"   ; break;
    case REG_EX_RANGE    : lN = "REG_EX_RANGE"    ; break;
    case REG_EX_RETURN   : lN = "REG_EX_RETURN"   ; break;
    case REG_EX_SPACE    : lN = "REG_EX_SPACE"    ; break;
    case REG_EX_SPACE_NOT: lN = "REG_EX_SPACE_NOT"; break;
    case REG_EX_START    : lN = "REG_EX_START"    ; break;
    case REG_EX_WORD     : lN = "REG_EX_WORD"     ; break;
    case REG_EX_WORD_NOT : lN = "REG_EX_WORD_NOT" ; break;

    case '\n': lN = "'\\n'"; break;
    case '\r': lN = "'\\r'"; break;
    case '\t': lN = "'\\t'"; break;

    default: sprintf_s(lName, "'%c'", lS.mC); lN = lName; break;
    }

    char lLine[1024];

    sprintf_s(lLine, "{ %s, %u, %3u, 0x%04x }," EOL, lN, lS.mMin, lS.mMax, lS.mFlagAndLink);
    aOut->Append(lLine);
}

// aOut        [---;RW-]
// aResultName [---;R--]
// aIndex                The state element index into the list
void SearchElement::GenerateCode(Filter_Internal * aOut, const char * aResultName, unsigned int aIndex)
{
    assert(NULL != aOut       );
    assert(NULL != aResultName);

    bool lIf = false;
    char lLine[1024];

    if     ((0 != mBegin) && (0 != mEnd)) { sprintf_s(lLine, "( %u <= i ) && ( %u >= i )", mBegin, mEnd ); lIf = true; }
    else if (0 != mBegin)                 { sprintf_s(lLine, "%u <= i"                   , mBegin       ); lIf = true; }
    else if                  (0 != mEnd)  { sprintf_s(lLine, "%u >= i"                   ,         mEnd ); lIf = true; }

    if (lIf) { aOut->C_if(lLine); }

        char lLetter = 'A' + aIndex;

        switch (mType)
        {
        case TYPE_BIN :
        case TYPE_TEXT:
            sprintf_s(lLine, "if ( l%c_Data[ l%c_Index ] == lData[ i ] ) { l%c_Index ++; }" EOL, lLetter, lLetter, lLetter);
            aOut->Append(lLine);
            sprintf_s(lLine, "if ( %u <= l%c_Index )", mStringSize_byte, lLetter);
            break;

        case TYPE_REGEX:
            sprintf_s(lLine, "if ( RegEx_Execute( & l%c_RegEx, lData[ i ] ) )", lLetter);
            break;

        default: assert(false);
        }

        aOut->Append(lLine);
        sprintf_s(lLine, " { %s = 1; }" EOL, aResultName);
        aOut->AppendCode(lLine);

    if (lIf) { aOut->C_if_End(); }
}

// aOut [---;RW-]
// aIndex         The element index into the list
void SearchElement::GenerateVariables(Filter_Internal * aOut, unsigned int aIndex)
{
    assert(NULL != aOut);

    char lLetter = 'A' + aIndex;
    char lLine[1024];

    switch (mType)
    {
    case TYPE_BIN:
        sprintf_s(lLine, "OPEN_NET_CONSTANT unsigned char * l%c_Data  = ", 'A' + aIndex);
        aOut->Append(lLine);

        aOut->AppendCode("{ ");
            for (unsigned int i = 0; i < mStringSize_byte; i++)
            {
                sprintf_s(lLine, (0 == i) ? "0x%02x" : ", 0x%02x", mString[i]);
                aOut->AppendCode(lLine);
            }
        aOut->AppendCode(" };" EOL);

        sprintf_s(lLine, "unsigned short    l%c_Index = 0;" EOL, lLetter);
        aOut->Append(lLine);
        break;

    case TYPE_REGEX:
        sprintf_s(lLine, "unsigned char l%c_Counters[ %u ];" EOL, lLetter, static_cast<unsigned int>(mStateCount));
        aOut->Append(lLine);
        sprintf_s(lLine, "RegEx l%c_RegEx;" EOL, lLetter);
        aOut->Append(lLine);
        sprintf_s(lLine, "OPEN_NET_CONSTANT RegEx_State l%c_States[] =" EOL, lLetter);
        aOut->Append(lLine);
        aOut->Append("{" EOL);
        aOut->Indent_Inc();
            for (unsigned int i = 0; i < mStateCount; i++)
            {
                GenerateState(aOut, i);
            }
        aOut->Indent_Dec();
        aOut->Append("};" EOL);
        sprintf_s(lLine, "REG_EX_CREATE( & l%c_RegEx, l%c_States, l%c_Counters );" EOL, lLetter, lLetter, lLetter);
        aOut->Append(lLine);
        break;

    case TYPE_TEXT:
        sprintf_s(lLine, "OPEN_NET_CONSTANT char * l%c_Data = \"%s\";" EOL, lLetter, mString);
        aOut->Append(lLine);

        sprintf_s(lLine, "unsigned short    l%c_Index = 0;" EOL, lLetter);
        aOut->Append(lLine);
        break;

    default: assert(false);
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

// Return
//  false  Error
//  true   OK
bool SearchElement::ConvertToBin()
{
    unsigned int  lDigit = 0;
    unsigned int  lOut   = 0;
    unsigned char lValue = 0;

    for (unsigned int i = 0; i < mStringSize_byte; i++)
    {
        switch (mString[i])
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            lDigit++;
            lValue <<= 4;
            lValue += mString[i] - '0';
            break;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            lDigit++;
            lValue <<= 4;
            lValue += mString[i] - 'a' + 10;
            break;

        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            lDigit++;
            lValue <<= 4;
            lValue += mString[i] - 'A' + 10;
            break;

        case ' ':
            if (1 == lDigit)
            {
                lDigit = 2;
            }
            break;

        default: return false;
        }

        if (2 == lDigit)
        {
            mString[lOut] = lValue;
            lDigit = 0;
            lOut++;
            lValue = 0;
        }
    }

    if (1 == lDigit)
    {
        mString[lOut] = lValue;
        lOut++;
    }

    mStringSize_byte = lOut;

    return true;
}

// ===== State ==============================================================

// aC  The charctere to match
void SearchElement::State_Add(char aC)
{
    assert(mStateCount < STATE_QTY);

    mStates[mStateCount].mC   = aC;
    mStates[mStateCount].mMin =  1;
    mStates[mStateCount].mMax =  1;

    mStateCount++;
}

// aState [---;R--] The state to add
void SearchElement::State_Add(const RegEx_State & aState)
{
    assert(NULL != (&aState));

    assert(mStateCount < STATE_QTY);

    mStates[mStateCount] = aState;

    mStateCount++;
}

// aText [---;R--] The string to parse
// aOps  [---;R--] The operations to execute
// aOpCount        The number of operations to execute
bool SearchElement::State_Add(const char * aText, const Operation * aOps, unsigned int aOpCount)
{
    assert(NULL != aText);
    assert(NULL != aOps );

    bool lResult = true;

    for (unsigned int i = 0; (i < aOpCount) && lResult; i++)
    {
        const Operation & lOp = aOps[i];
        assert(lOp.mBegin < lOp.mEnd);

        assert(0 == (mStates[lOp.mState].mFlagAndLink & REG_EX_LINK_MASK));

        unsigned int lSize_byte = lOp.mEnd - lOp.mBegin - 1;
        const char * lText      = aText    + lOp.mBegin + 1;
        assert(0 < lSize_byte);

        mStates[lOp.mState].mFlagAndLink |= mStateCount;

        RegEx_State lReturn = REG_EX_STATE_RETURN(lOp.mState);

        switch (mStates[lOp.mState].mC)
        {
        case REG_EX_GROUP  : lResult = State_Add(lText, lSize_byte, lReturn     , false); break;
        case REG_EX_OR_FAST: lResult = State_Add(lText, lSize_byte, STATE_OR_END, true ); break;

        default: assert(false);
        }
    }

    return lResult;
}

// TODO  CodeGen.SearchElement
//       Normal (Cleanup) - Remove aOr argument and use the final state to
//       know if we are in a or group.

// aText  [---;R--] The text to parse
// aSize_byte       The size of the text to parse
// aFinal [---;R--] The final state to at after parsing
// aOr
//  false  We are not in a OR group
//  true   We are in a OR group
//
// Return
//  false  Error
//  true   OK
bool SearchElement::State_Add(const char * aText, unsigned int aSize_byte, const RegEx_State & aFinal, bool aOr)
{
    assert(NULL != aText     );
    assert(   0 <  aSize_byte);

    unsigned int  i;
    unsigned int  lLevel   = 0;
    unsigned int  lOpCount = 0;
    Operation     lOps[512];

    memset(&lOps, 0, sizeof(lOps));

    for (i = 0; i < aSize_byte; i++)
    {
        if (0 >= lLevel)
        {
            switch (aText[i])
            {
            case '$':
                if (aSize_byte > (i + 1)) { return false; }
                mStates[mStateCount].mC = REG_EX_END; mStateCount++;
                break;
            case '(': State_Add(REG_EX_GROUP); lLevel++; lOps[lOpCount].mBegin = i; lOps[lOpCount].mState = mStateCount - 1; break;

            case ')':
            case ']': return false;

            case '*':
                if (0 >= i) { return false; }
                mStates[mStateCount - 1].mMin = 0; mStates[mStateCount - 1].mMax = 255;
                break;
            case '+':
                if (0 >= i) { return false; }
                mStates[mStateCount - 1].mMin = 1; mStates[mStateCount - 1].mMax = 255;
                break;
            case '-':
                if (aOr)
                {
                    if ((0 >= i) || (aSize_byte <= (i + 1))) { return false; }
                    i++;
                    mStates[mStateCount - 1].mMax = aText[i]; mStates[mStateCount - 1].mMin = mStates[mStateCount - 1].mC; mStates[mStateCount - 1].mC = REG_EX_RANGE;
                }
                else
                {
                    State_Add('-');
                }
                break;
            case '.': State_Add(REG_EX_DOT); mStateCount++; break;
            case '?':
                if (0 >= i) { return false; }
                mStates[mStateCount - 1].mMin = 0; mStates[mStateCount - 1].mMax = 1;
                break;
            case '[': State_Add(REG_EX_OR_FAST); lLevel++; lOps[lOpCount].mBegin = i; lOps[lOpCount].mState = mStateCount - 1; break;
            case '\\':
                if (aSize_byte <= (i + 1)) { return false; }
                i++;
                switch (aText[i])
                {
                case 'D': State_Add(REG_EX_DIGIT_NOT); break;
                case 'S': State_Add(REG_EX_SPACE_NOT); break;
                case 'W': State_Add(REG_EX_WORD_NOT ); break;
                case 'd': State_Add(REG_EX_DIGIT    ); break;
                case 'n': State_Add('\n'            ); break;
                case 'r': State_Add('\r'            ); break;
                case 's': State_Add(REG_EX_SPACE    ); break;
                case 'w': State_Add(REG_EX_WORD     ); break;
                case 't': State_Add('\t'            ); break;
                default : State_Add(aText[i]        ); break;
                }
                break;
            case '^':
                if (0 < i) { return false; }
                if (aOr)
                {
                    if (REG_EX_OR_FAST != mStates[aFinal.mFlagAndLink & REG_EX_LINK_MASK].mC) { return false; }
                    mStates[aFinal.mFlagAndLink & REG_EX_LINK_MASK].mC = REG_EX_OR_NOT;
                }
                else
                {
                    if (0 != mStateCount) { return false; }
                    State_Add(STATE_START);
                }
                break;
            case '{': if (!State_Add_7d(aText, aSize_byte, &i)) { return false; } break;
            case '|':
                if (0 >= i) { return false; }
                mStates[mStateCount - 1].mFlagAndLink |= REG_EX_FLAG_OR;
                break;

            default: State_Add(aText[i]);
            }
        }
        else
        {
            switch (aText[i])
            {
            case '(':
            case '[': lLevel++; break;

            case ')':
            case ']': lLevel--; if (0 == lLevel) { lOps[lOpCount].mEnd = i; lOpCount++; }; break;
            }
        }
    }

    State_Add(aFinal);

    return State_Add(aText, lOps, lOpCount);
}

// aText   [---;R--] The text to parse
// aSize_byte        The size of the texte to parse
// aOffset [---;RW-] Offset where to parse in input, offset where to continue
//                   to parse on output
//
// Return
//  false  Error
//  true   OK
bool SearchElement::State_Add_7d(const char * aText, unsigned int aSize_byte, unsigned int * aOffset)
{
    assert(NULL       != aText     );
    assert(0          <  aSize_byte);
    assert(NULL       != aOffset   );
    assert(aSize_byte >  (*aOffset));

    if (0 >= (*aOffset)) { return false; }

    (*aOffset)++;

    unsigned int lValue = 0;
    bool         lSep   = false;

    while (aSize_byte > (*aOffset))
    {
        char lC = aText[*aOffset];

        if (('0' < lC) && ('9' >= lC))
        {
            lValue *= 10;
            lValue += lC - '0';
        }
        else if (',' == lC)
        {
            if (lSep || (0xff < lValue)) { return false; }

            mStates[mStateCount - 1].mMin = lValue;
            lValue = 0;
            lSep = true;
        }
        else if ('}' == lC)
        {
            break;
        }
        else { return false; }

        (*aOffset)++;
    }

    if (lSep)
    {
        if (0 == lValue)
        {
            mStates[mStateCount - 1].mMax = 255;
        }
        else
        {
            if (mStates[mStateCount - 1].mMin > lValue) { return false; }

            mStates[mStateCount - 1].mMax = lValue;
        }
    }
    else
    {
        if (0xff < lValue) { return false; }

        mStates[mStateCount - 1].mMin = lValue;
        mStates[mStateCount - 1].mMax = lValue;
    }

    return true;
}
