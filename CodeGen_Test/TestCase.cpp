
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) KMS 2019. All rights reserved.
// Product    GPU-dpi
// File       CodeGen_Test/TestCase.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== CodeGen_Test =======================================================
#include "TestCase.h"

// Static fonction declaration
/////////////////////////////////////////////////////////////////////////////

static bool Compare  (const TestCase & aTestCase, const char * aCode, unsigned int aCodeSize_byte);
static void WriteFile(const char     * aFileName, const char * aCode, unsigned int aCodeSize_byte);

// Functions
/////////////////////////////////////////////////////////////////////////////

bool TestCase_Verify(const TestCase & aTestCase, Filter * aFilter)
{
    assert(NULL != (&aTestCase));

    if (NULL == aFilter)
    {
        printf("TEST ERROR  No CodeGen instance\n");
        return false;
    }

    const char * lC       = aFilter->GetCode        ();
    unsigned int lCS_byte = aFilter->GetCodeSize    ();
    const char * lEM      = aFilter->GetErrorMessage();
    const char * lFName   = aFilter->GetFunctionName();
    bool         lResult  = true;

    if (0 != strcmp("FilterFunction", lFName))
    {
        printf("TEST ERROR  Invalid function name\n");
        return false;
    }

    if (NULL == aTestCase.mErrorMessage)
    {
        assert(NULL != aTestCase.mResultFile);

        if (NULL != lEM)
        {
            printf("TEST ERROR  Unexpected error message\n%s\n", lEM);
            lResult = false;
        }
        else
        {
            if (0 >= lCS_byte)
            {
                printf("TEST ERROR  No error message and code size is 0\n");
                lResult = false;
            }

            if (NULL == lC)
            {
                printf("TEST ERROR  No error message and no code\n");
                lResult = false;
            }
        }

        if (lResult)
        {
            lResult = Compare(aTestCase, lC, lCS_byte);
        }
    }
    else
    {
        if (NULL == lEM)
        {
            printf("TEST ERROR  No error message\n");
            lResult = false;
        }

        if (0 != strcmp(aTestCase.mErrorMessage, lEM))
        {
            printf("TEST ERROR  The error message does not match.\n");
            printf("Expected : %s\n", aTestCase.mErrorMessage);
            printf("Generated: %s\n", lEM);
            lResult = false;
        }

        if (NULL != lC)
        {
            printf("TEST ERROR  The code and an error message are present\n");
            lResult = false;
        }
    }

    aFilter->Delete();

    return lResult;
}

// Static function
/////////////////////////////////////////////////////////////////////////////

bool Compare(const TestCase & aTestCase, const char * aCode, unsigned int aCodeSize_byte)
{
    assert(NULL != (&aTestCase)           );
    assert(NULL !=   aTestCase.mResultFile);
    assert(NULL !=   aCode                );
    assert(   0 <    aCodeSize_byte       );

    FILE * lFile     ;
    char   lFileName[1024];
    bool   lResult   ;
    int    lRet      ;
    size_t lSize_byte;

    sprintf_s(lFileName, "CodeGen_Test" SLASH "Tests" SLASH "%s.c", aTestCase.mResultFile);

    if (0 == fopen_s(&lFile, lFileName, "r"))
    {
        char * lBuffer = new char[aCodeSize_byte + 1];
        assert(NULL != lBuffer);

        memset(lBuffer, 0, aCodeSize_byte + 1);

        lResult = true;

        lSize_byte = fread(lBuffer, 1, aCodeSize_byte, lFile);
        if (aCodeSize_byte != lSize_byte)
        {
            printf("TEST_ERROR  The result file is smaller than the generated code\n");
            lResult = false;
        }

        if (0 != strcmp(lBuffer, aCode))
        {
            printf("TEST ERROR  Generated code does not match\n");
            lResult = false;
        }

        delete[] lBuffer;

        lRet = fclose(lFile);
        assert(0 == lRet);

        if (!lResult)
        {
            sprintf_s(lFileName, "CodeGen_Test" SLASH "Tests" SLASH "%s_ERROR.c", aTestCase.mResultFile);
        }
    }
    else
    {
        printf("TEST ERROR  Result file does not exist\n");
        lResult = false;
    }

    if (!lResult)
    {
        WriteFile(lFileName, aCode, aCodeSize_byte);
    }

    return lResult;
}

void WriteFile(const char * aFileName, const char * aCode, unsigned int aCodeSize_byte)
{
    assert(NULL != aFileName     );
    assert(NULL != aCode         );
    assert(   0 <  aCodeSize_byte);

    FILE * lFile;

    if (0 == fopen_s(&lFile, aFileName, "w"))
    {
        size_t lSize_byte = fwrite(aCode, 1, aCodeSize_byte, lFile);
        assert(aCodeSize_byte == lSize_byte);

        int lRet = fclose(lFile);
        assert(0 == lRet);
    }
    else
    {
        printf("ERROR  Cannot create the %s file\n", aFileName);
    }
}
