
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All right reserved.
// Product    GPU-dpi
// File       GPU-dpi_TG/GPU-dpi_TG.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include <KmsBase.h>

// ===== C ==================================================================
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _KMS_WINDOWS_
    // ===== Windows ========================================================
    #include <Windows.h>
#endif

// ===== Import/Includes ====================================================
#include <KmsLib/ThreadBase.h>
#include <KmsTool.h>
#include <OpenNet/Adapter.h>
#include <OpenNet/PacketGenerator.h>
#include <OpenNet/System.h>

// ===== Includes ===========================================================
#include <GPU_dpi/System.h>

// ===== Common =============================================================
#include "../Common/Version.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static unsigned int CountHexDigit(const char * aIn);
static void         Help         ();
static bool         IsBlank      (char aC);
static bool         IsHexDigit   (char aC);
static void         List         ();
static bool         Read         (const char * aFileName, unsigned int aSize_byte, unsigned char * aOut);
static int          Run          (unsigned int aAdapter, unsigned int aBandwidth_MB_s, const char * aFolder);
static int          Test         (OpenNet::Adapter * aAdapter, const char * aFolder);

// Entry point
/////////////////////////////////////////////////////////////////////////////

int main(int aCount, const char * * aVector)
{
    KMS_TOOL_BANNER("GPU-dpi", "GPU-dpi_TG", VERSION_STR, VERSION_TYPE);

    int lA = 0;
    int lB = 0;
    int lF = 0;

    for (int i = 0; i < aCount; i++)
    {
        if (0 == strcmp("-a"  , aVector[i])) { i++; lA = i; }
        if (0 == strcmp("-b"  , aVector[i])) { i++; lB = i; }
        if (0 == strcmp("-f"  , aVector[i])) { i++; lF = i; }
        if (0 == strcmp("-h"  , aVector[i])) { Help(); return 0; }
        if (0 == strcmp("-?"  , aVector[i])) { Help(); return 0; }
        if (0 == strcmp("help", aVector[i])) { Help(); return 0; }
        if (0 == strcmp("list", aVector[i])) { List(); return 0; }
    }

    unsigned int lAdapter = 0;

    if (0 < lA)
    {
        if (aCount <= lA)
        {
            fprintf(stderr, "USER ERROR  -a without adapter index\n");
            return __LINE__;
        }

        lAdapter = strtoul(aVector[lA], NULL, 10);
    }

    unsigned int lBandwidth_MB_s = 30;

    if (0 < lB)
    {
        if (aCount <= lB)
        {
            fprintf(stderr, "USER ERROR  -b without bandwidth value\n");
            return __LINE__;
        }

        lBandwidth_MB_s = strtoul(aVector[lB], NULL, 10);
    }

    const char * lFolder = NULL;

    if (0 < lF)
    {
        if (aCount <= lF)
        {
            fprintf(stderr, "USER ERROR  -b without folder name\n");
            return __LINE__;
        }

        lFolder = aVector[lF];
    }

    return Run(lAdapter, lBandwidth_MB_s, lFolder);
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

// aIn [---;R--]
//
// Return  This function returns the number of hex digit
unsigned int CountHexDigit(const char * aIn)
{
    assert(NULL != aIn);

    unsigned int lResult = 0;

    while (IsHexDigit(aIn[lResult]))
    {
        lResult++;
    }

    return lResult;
}

void Help()
{
    printf("GPU-dpi_TG [-a AdapterIndex] [-b Bandwidth] [-f Folder] [-h] [-?] [help] [list]\n");
    printf("  -a    Set the adapter index (The default value is 0)\n");
    printf("  -b    Set the bandwidth in MB/s (The default value is 30)\n");
    printf("  -f    Set the folder where to read test file from\n");
    printf("  -h    Display this help message\n");
    printf("  -?    Display this help message\n");
    printf("  help  Display this help message\n");
    printf("  list  List the adapters\n");
}

// aC
//
// Return
//  false  C is no a blank
//  true   C is a blank
bool IsBlank(char aC)
{
    switch (aC)
    {
    case ' ' :
    case '\n' :
    case '\r' :
    case '\t' :
        return true;
    }

    return false;
}

// aC
//
// Return
//  false  C is not an hex digit
//  true   C is an hex digit
bool IsHexDigit(char aC)
{
    switch (aC)
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
    case 'A':
    case 'a':
    case 'B':
    case 'b':
    case 'C':
    case 'c':
    case 'D':
    case 'd':
    case 'E':
    case 'e':
    case 'F':
    case 'f':
        return true;
    }

    return false;
}

void List()
{
    GPU_dpi::System * lSystem = GPU_dpi::System::Create();
    assert(NULL != lSystem);

    unsigned int lCount = lSystem->Adapter_GetCount();

    printf("%u adapters\n", lCount);

    for (unsigned int i = 0; i < lCount; i++)
    {
        GPU_dpi::AdapterInfo lInfo;

        GPU_dpi::Status lStatus = lSystem->Adapter_GetInfo(i, &lInfo);
        assert(GPU_dpi::STATUS_OK == lStatus);

        printf(" %u  %s\n", i, lInfo.mName);
    }
}

// aFileName [---;R--] The packet description file name
// aSize_byte          The size of the packet
// aOut      [---;-W-] The output buffer
//
// Return
//  false  Error
//  true   OK
bool Read(const char * aFileName, unsigned int aSize_byte, unsigned char * aOut)
{
    assert(NULL != aFileName );
    assert(NULL != aSize_byte);
    assert(NULL != aOut      );

    memset(aOut, 0, aSize_byte);

    FILE * lIn;

    int lRet = fopen_s(&lIn, aFileName, "r");
    if (0 != lRet)
    {
        return false;
    }

    unsigned int lIndex  = 0;
    char         lLine[1024];
    unsigned int lLineNo = 0;

    while ((aSize_byte > lIndex) && (NULL != fgets(lLine, sizeof(lLine), lIn)))
    {
        lLineNo++;

        unsigned int i = 0;
        while ('\0' != lLine[i])
        {
            if ('#' == lLine[i]) { break; }

            if      (IsBlank   (lLine[i])) { i++; }
            else if (IsHexDigit(lLine[i]))
            {
                unsigned int lCount = CountHexDigit(lLine + i);
                unsigned int lValue = strtoul(lLine + i, NULL, 16);
                switch (lCount)
                {
                case 1:
                case 2:
                    aOut[lIndex] = lValue; lIndex++;
                    break;

                case 3:
                case 4:
                    aOut[lIndex] = (lValue >> 8) & 0x00ff; lIndex++;
                    aOut[lIndex] =  lValue       & 0x00ff; lIndex++;
                    break;

                case 5:
                case 6:
                case 7:
                case 8:
                    aOut[lIndex] = (lValue >> 24) & 0x000000ff; lIndex++;
                    aOut[lIndex] = (lValue >> 16) & 0x000000ff; lIndex++;
                    aOut[lIndex] = (lValue >>  8) & 0x000000ff; lIndex++;
                    aOut[lIndex] =  lValue        & 0x000000ff; lIndex++;
                    break;

                default:
                    fprintf(stderr, "WARNING  File %s, line %u - A value is too long - Ignored\n", aFileName, lLineNo);
                }

                i += lCount;
            }
            else
            {
                fprintf(stderr, "WARNING  File %s, line %u - Invalid charactere - Ignored\n", aFileName, lLineNo);
                i++;
            }
        }
    }

    lRet = fclose(lIn);
    assert(0 == lRet);

    return true;
}

// aAdapter
// aBandwidth_MB_s
// aFolder [--O;R--] Read the packet description file from this folder
int Run(unsigned int aAdapter, unsigned int aBandwidth_MB_s, const char * aFolder)
{
    OpenNet::System * lSystem = OpenNet::System::Create();
    assert(NULL != lSystem);

    OpenNet::Adapter * lAdapter = lSystem->Adapter_Get(aAdapter);
    if (NULL == lAdapter)
    {
        fprintf(stderr, "USER ERROR  Invalid adapter index (%u)\n", aAdapter);
        return __LINE__;
    }

    OpenNet::PacketGenerator * lGenerator = OpenNet::PacketGenerator::Create();
    assert(NULL != lGenerator);

    OpenNet::PacketGenerator::Config lConfig;

    OpenNet::Status lStatus = lGenerator->GetConfig(&lConfig);
    assert(OpenNet::STATUS_OK == lStatus);

    lConfig.mBandwidth_MiB_s = aBandwidth_MB_s;
    lConfig.mPacketSize_byte = 1024;

    lStatus = lGenerator->SetConfig(lConfig);
    assert(OpenNet::STATUS_OK == lStatus);

    lStatus = lGenerator->SetAdapter(lAdapter);
    assert(OpenNet::STATUS_OK == lStatus);

    lStatus = lGenerator->Start();
    if (OpenNet::STATUS_OK != lStatus)
    {
        fprintf(stderr, "ERROR  OpenNet::PacketGenerator::Start() failed - ");
        OpenNet::Status_Display(lStatus, stderr);
        fprintf(stderr, "\n");
    }

    if (NULL != aFolder)
    {
        return Test(lAdapter, aFolder);
    }

    KmsLib::ThreadBase::Sleep_s(3600);

    return 0;
}

// aAdapter [---;RW-] The adapter to use for sending packet
// aFolder  [---;R--] Read the packet description files for this folder
//
// Return
//      0  OK
//  Other  Error
int Test(OpenNet::Adapter * aAdapter, const char * aFolder)
{
    assert(NULL != aAdapter);
    assert(NULL != aFolder );

    unsigned int lIteration = 0;

    for (;;)
    {
        printf("Iteration %u\r", lIteration);
        lIteration++;

        unsigned int lSize_byte = 65;

        for (;;)
        {
            unsigned char lBuffer  [16 * 1024];
            char          lFileName[     1024];

            sprintf_s(lFileName, "%s" SLASH "%04u.txt", aFolder, lSize_byte);

            if (!Read(lFileName, lSize_byte, lBuffer))
            {
                break;
            }

            OpenNet::Status lStatus = aAdapter->Packet_Send(lBuffer, lSize_byte);
            if (OpenNet::STATUS_OK != lStatus)
            {
                fprintf(stderr, "WARNING  OpenNet::Adapter::Packet_Send( , %u bytes ) failed - ", lSize_byte);
                OpenNet::Status_Display(lStatus, stderr);
                fprintf(stderr, "\n");
            }

            lSize_byte++;

            KmsLib::ThreadBase::Sleep_ms(10);
        }
    }

    return 0;
}
