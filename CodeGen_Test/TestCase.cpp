
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) KMS 2019. All rights reserved.
// Product    GPU-dpi
// File       CodeGen_Test/TestCase.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== CodeGen_Test =======================================================
#include "TestCase.h"

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
            FILE * lFile     ;
            char   lFileName[1024];
            size_t lSize_byte;
            int    lRet      ;

            sprintf_s(lFileName, "CodeGen_Test" SLASH "Tests" SLASH "%s.c", aTestCase.mResultFile);

            if (0 == fopen_s(&lFile, lFileName, "r"))
            {
                char * lBuffer = new char[lCS_byte + 1];
                assert(NULL != lBuffer);

                memset(lBuffer, 0, lCS_byte + 1);

                lSize_byte = fread(lBuffer, 1, lCS_byte, lFile);
                if (lCS_byte != lSize_byte)
                {
                    printf("TEST_ERROR  The result file is smaller than the generated code\n");
                    lResult = false;
                }

                if (0 != strcmp(lBuffer, lC))
                {
                    printf("TEST ERROR  Generated code does not match\n");
                    lResult = false;
                }

                delete[] lBuffer;

                lRet = fclose(lFile);
                assert(0 == lRet);
            }
            else
            {
                printf("TEST ERROR  Result file does not exist\n");
                lResult = false;

                if (0 == fopen_s(&lFile, lFileName, "w"))
                {
                    lSize_byte = fwrite(lC, 1, lCS_byte, lFile);
                    assert(lCS_byte == lSize_byte);

                    lRet = fclose(lFile);
                    assert(0 == lRet);
                }
                else
                {
                    printf("ERROR  Cannot create the result file\n");
                }
            }
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