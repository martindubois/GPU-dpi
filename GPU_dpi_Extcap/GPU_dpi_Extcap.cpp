
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi_Extcap/GPU_dpi_Extcap.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <stdio.h>

#ifdef _KMS_WINDOWS_
    #include <Windows.h>
#endif

// ===== Import/Includes ====================================================
#include <KmsLib/Exception.h>
#include <KmsLib/ThreadBase.h>

// ===== Includes ===========================================================
#include <GPU_dpi/System.h>

// Data type
/////////////////////////////////////////////////////////////////////////////

typedef struct
{
    struct
    {
        unsigned mCapture       : 1;
        unsigned mCaptureFilter : 1;
        unsigned mConfig        : 1;
        unsigned mDLTs          : 1;
        unsigned mFifo          : 1;
        unsigned mInterface     : 1;
        unsigned mInterfaces    : 1;
        unsigned mVersion       : 1;
    }
    mPresent;

    char mCaptureFilter[2048];
    char mExecName     [ 256];
    char mFifo         [ 256];
    char mInterface    [  64];
    char mVersion      [  32];
}
Arguments;

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void ParseArguments(int aCount, const char * * aVector, Arguments * aArgs);

static void ValidateArguments(const Arguments & aArgs);

// ===== Operations =========================================================
static void Capture   (GPU_dpi::System * aSystem, const Arguments & aArgs);
static void Config    ();
static void DLTs      (const Arguments & aArgs);
static void Interfaces(GPU_dpi::System * aSystem);

// Entry point
/////////////////////////////////////////////////////////////////////////////

int main( int aCount, const char * * aVector )
{
    int               lResult =    0;
    GPU_dpi::System * lSystem = NULL;

    try
    {
        Arguments lArgs;

        ParseArguments(aCount, aVector, &lArgs);
        ValidateArguments(lArgs);

        GPU_dpi::System * lSystem = GPU_dpi::System::Create();

        if      (lArgs.mPresent.mCapture   ) { Capture   (lSystem, lArgs); }
        else if (lArgs.mPresent.mConfig    ) { Config    (); }
        else if (lArgs.mPresent.mDLTs      ) { DLTs      (lArgs); }
        else if (lArgs.mPresent.mInterfaces) { Interfaces(lSystem); }
    }
    catch (KmsLib::Exception * eE)
    {
        fprintf(stderr, "ERROR 0  Exception\n");
        eE->Write(stderr);
        lResult = __LINE__;
    }
    catch (...)
    {
        fprintf(stderr, "ERROR 1  Unknown exception\n");
        lResult = __LINE__;
    }

    if (NULL != lSystem)
    {
        lSystem->Delete();
    }

    return lResult;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void ParseArguments(int aCount, const char * * aVector, Arguments * aArgs)
{
    assert(   1 <= aCount    );
    assert(NULL != aVector   );
    assert(NULL != aVector[0]);
    assert(NULL != aArgs     );

    memset(aArgs, 0, sizeof(Arguments));

    strncpy_s(aArgs->mExecName, aVector[0], sizeof(aArgs->mExecName) - 1);

    const char * lMsg = NULL;

    for (int i = 1; i < aCount; i++)
    {
        if      (0 == strcmp("--capture"              , aVector[i])) { aArgs->mPresent.mCapture    = true; }
        else if (0 == strcmp("--extcap-config"        , aVector[i])) { aArgs->mPresent.mConfig     = true; }
        else if (0 == strcmp("--extcap-dlts"          , aVector[i])) { aArgs->mPresent.mDLTs       = true; }
        else if (0 == strcmp("--extcap-interfaces"    , aVector[i])) { aArgs->mPresent.mInterfaces = true; }
        else if (0 == strcmp("--extcap-capture-filter", aVector[i]))
        {
            aArgs->mPresent.mCaptureFilter = true;

            i++;
            if (aCount <= i) { lMsg = "ERROR 2  --extcap-capture-filter without filter"; break; }

            strncpy_s(aArgs->mCaptureFilter, aVector[i], sizeof(aArgs->mCaptureFilter) - 1);
        }
        else if (0 == strcmp("--extcap-interface", aVector[i]))
        {
            aArgs->mPresent.mInterface = true;

            i++;
            if (aCount <= i) { lMsg = "ERROR 3  --extcap-interface without interface"; break; }

            strncpy_s(aArgs->mInterface, aVector[i], sizeof(aArgs->mInterface) - 1);
        }
        else if (0 == strcmp("--fifo", aVector[i]))
        {
            aArgs->mPresent.mFifo = true;

            i++;
            if (aCount <= i) { lMsg = "ERROR 4  --fifo without fifo"; break; }

            strncpy_s(aArgs->mFifo, aVector[i], sizeof(aArgs->mFifo) - 1);
        }
        else if (0 == strncmp("--extcap-version=", aVector[i], 17))
        {
            aArgs->mPresent.mVersion = true;
            strncpy_s(aArgs->mVersion, aVector[i] + 17, sizeof(aArgs->mVersion) - 1);
        }
    }

    if (NULL != lMsg)
    {
        throw new KmsLib::Exception(KmsLib::Exception::CODE_INVALID_COMMAND_LINE,
            lMsg, NULL, __FILE__, __FUNCTION__, __LINE__, 0);
    }
}

void ValidateArguments(const Arguments & aArgs)
{
    assert(NULL != (&aArgs));

    const char * lMsg = NULL;

    if (aArgs.mPresent.mCapture)
    {
        if ( aArgs.mPresent.mConfig    ) { lMsg = "ERROR 5  --capture with --config"      ; }
        if ( aArgs.mPresent.mDLTs      ) { lMsg = "ERROR 6  --capture with --dlts"        ; }
        if (!aArgs.mPresent.mFifo      ) { lMsg = "ERROR 7  --capture wihtout --fifo"     ; }
        if (!aArgs.mPresent.mInterface ) { lMsg = "ERROR 8  --capture without --interface"; }
        if ( aArgs.mPresent.mInterfaces) { lMsg = "ERROR 9  --capture with --interfaces"  ; }
    }

    if (aArgs.mPresent.mCaptureFilter)
    {
        if (!aArgs.mPresent.mCapture) { lMsg = "ERROR 10  --extcap-capture-filter without --capture"; }
    }

    if (aArgs.mPresent.mConfig)
    {
        if ( aArgs.mPresent.mDLTs      ) { lMsg = "ERROR 11  --extcap-config with --dlts"               ; }
        if ( aArgs.mPresent.mFifo      ) { lMsg = "ERROR 12  --extcap-config wiht --fifo"               ; }
        if (!aArgs.mPresent.mInterface ) { lMsg = "ERROR 13  --extcap-config wihtout --extcap-interface"; }
        if ( aArgs.mPresent.mInterfaces) { lMsg = "ERROR 14  --extcap-config with --interfaces"         ; }
    }

    if (aArgs.mPresent.mDLTs)
    {
        if ( aArgs.mPresent.mFifo      ) { lMsg = "ERROR 15  --extcap-dlts with --fifo"               ; }
        if (!aArgs.mPresent.mInterface ) { lMsg = "ERROR 16  --extcap-dlts wihtout --extcap-interface"; }
        if ( aArgs.mPresent.mInterfaces) { lMsg = "ERROR 17  --extcap-dlts with --interfaces"         ; }
    }

    if (aArgs.mPresent.mFifo)
    {
        if (!aArgs.mPresent.mCapture) { lMsg = "ERROR 18  --fifo without --capture"; }
    }

    if (NULL != lMsg)
    {
        throw new KmsLib::Exception(KmsLib::Exception::CODE_INVALID_COMMAND_LINE,
            lMsg, NULL, __FILE__, __FUNCTION__, __LINE__, 0);
    }
}

// ===== Operations =========================================================

void Capture(GPU_dpi::System * aSystem, const Arguments & aArgs)
{
    assert(NULL != aSystem );
    assert(NULL != (&aArgs));

    unsigned int lCount = aSystem->Adapter_GetCount();
    unsigned int lIndex = strtoul(aArgs.mInterface + 8, NULL, 10);

    GPU_dpi::AdapterConfig lAC;

    aSystem->Adapter_ResetConfig(lIndex);

    GPU_dpi::Status lStatus = aSystem->Adapter_GetConfig(lIndex, &lAC);
    if (GPU_dpi::STATUS_OK != lStatus)
    {
        throw new KmsLib::Exception(KmsLib::Exception::CODE_ERROR,
            "GPU_dpi::System::Adapter_GetConfig( ,  ) failed", NULL, __FILE__, __FUNCTION__, __LINE__, lStatus);
    }

    lAC.mBufferQty      = 56;
    lAC.mOutputFileName = aArgs.mFifo;
    lAC.mOutputFormat   = GPU_dpi::OUTPUT_FORMAT_PCAP;
    lAC.mOutputType     = GPU_dpi::OUTPUT_TYPE_FILE;

    if (aArgs.mPresent.mCaptureFilter)
    {
        lAC.mFilterCode          = aArgs.mCaptureFilter        ;
        lAC.mFilterCodeSize_byte = static_cast<unsigned int>(strlen(aArgs.mCaptureFilter));
        lAC.mFilterType          = GPU_dpi::FILTER_TYPE_FILTER ;
    }
    else
    {
        lAC.mFilterType = GPU_dpi::FILTER_TYPE_ALL_PACKETS;
    }

    lStatus = aSystem->Adapter_SetConfig(lIndex, lAC);
    if (GPU_dpi::STATUS_OK != lStatus)
    {
        throw new KmsLib::Exception(KmsLib::Exception::CODE_ERROR,
            "GPU_dpi::System::Adapter_SetConfig( ,  ) failed", NULL, __FILE__, __FUNCTION__, __LINE__, lStatus);
    }

    lStatus = aSystem->Start();
    if (GPU_dpi::STATUS_OK != lStatus)
    {
        switch (lStatus)
        {
        case GPU_dpi::STATUS_INVALID_FILTER:
        case GPU_dpi::STATUS_OPEN_CL_ERROR :
            FILE * lFile;
            if (0 == fopen_s(&lFile, "C:\\Temp\\GPU_dpi_Extcap_BuildLog.txt", "w"))
            {
                GPU_dpi::Status lStatus = aSystem->WriteBuildLog(lFile);
                assert(GPU_dpi::STATUS_OK == lStatus);

                int lRet = fclose(lFile);
                assert(0 == lRet);
            }
            break;
        }

        throw new KmsLib::Exception(KmsLib::Exception::CODE_ERROR,
            "GPU_dpi::System::Start() failed", NULL, __FILE__, __FUNCTION__, __LINE__, lStatus);
    }

    lStatus = aSystem->Wait();
    assert(GPU_dpi::STATUS_OK == lStatus);
}

void Config()
{

}

// aArgs [---;R--]
void DLTs(const Arguments & aArgs)
{
	assert(NULL != (&aArgs));

	unsigned int lIndex = strtoul(aArgs.mInterface + 8, NULL, 10);

	printf("dlt {number=0}{name=GPU-dpi_%u}{display=GPU-dpi interface %u}\n", lIndex, lIndex);
}

void Interfaces(GPU_dpi::System * aSystem)
{
    assert(NULL != aSystem);

    printf("GPU_dpi_Extcap {version=1.0}{help=http://www.kms-quebec.com}\n");

    unsigned int lCount = aSystem->Adapter_GetCount();
    for (unsigned int i = 0; i < lCount; i++)
    {
        GPU_dpi::AdapterInfo lInfo;

        GPU_dpi::Status lStatus = aSystem->Adapter_GetInfo(i, &lInfo);
        if (GPU_dpi::STATUS_OK != lStatus)
        {
            throw new KmsLib::Exception(KmsLib::Exception::CODE_ERROR,
                "System::Adapter_GetInfo( ,  ) failed", NULL, __FILE__, __FUNCTION__, __LINE__, lStatus);
        }

        printf("interface {value=GPU-dpi_%u}{display=GPU-dpi interface %u}\n", i, i);
    }
}
