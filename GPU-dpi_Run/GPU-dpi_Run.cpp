
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All right reserved.
// Product    GPU-dpi
// File       GPU-dpi_Run/GPU-dpi_Run.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include <KmsBase.h>

// ===== C ==================================================================
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _KMS_WINDOWS_
    // ===== Windows ========================================================
    #include <Windows.h>
#endif

// ===== Import/Includes ====================================================
#include <KmsLib/Exception.h>
#include <KmsLib/ThreadBase.h>
#include <KmsTool.h>
#include <OpenNet/Adapter.h>
#include <OpenNet/System.h>

// ===== Includes ===========================================================
#include <GPU_dpi/System.h>

// ===== Common =============================================================
#include "../Common/Version.h"

// Static variable
/////////////////////////////////////////////////////////////////////////////

static bool sRunning = true;

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void Help();
static void Info();
static int  Run (const char * aFileName, bool lLog, bool aStatistics);
static void Wait();

// ===== Display ============================================================
static void DisplayError     (const char * aMsg, GPU_dpi::Status aStatus);
static void DisplayStatistics(GPU_dpi::System * aSystem);

// ===== Entry point ========================================================
static void OnCtrlC(int aSignal);

// Entry point
/////////////////////////////////////////////////////////////////////////////

int main(int aCount, const char * * aVector)
{
    KMS_TOOL_BANNER("GPU-dpi", "GPU-dpi_Run", VERSION_STR, VERSION_TYPE);

    const char * lFileName   = NULL ;
    bool         lLog        = false;
    bool         lStatistics = false;

    for (int i = 1; i < aCount; i++)
    {
        if (0 == strcmp("-h"  , aVector[i])) { Help(); return 0; }
        if (0 == strcmp("-?"  , aVector[i])) { Help(); return 0; }
        if (0 == strcmp("help", aVector[i])) { Help(); return 0; }
        if (0 == strcmp("info", aVector[i])) { Info(); return 0; }

        if      (0 == strcmp("-l", aVector[i])) { lLog        = true; }
        else if (0 == strcmp("-s", aVector[i])) { lStatistics = true; }
        else
        {
            if (NULL != lFileName)
            {
                fprintf(stderr, "USER ERROR  Invalid command line\n");
                return __LINE__;
            }

            lFileName = aVector[i];
        }
    }

    if (NULL == lFileName)
    {
        fprintf(stderr, "USER ERROR  No configuration file specified\n");
        return __LINE__;
    }

    int lResult;

    try
    {
        lResult = Run(lFileName, lLog, lStatistics);
    }
    catch ( KmsLib::Exception * eE )
    {
        fprintf( stderr, "UNEXPECTED EXCEPTION\n" );
        eE->Write( stderr );
        lResult = __LINE__;
    }
    catch ( ... )
    {
        fprintf( stderr, "UNKNOWN EXCEPTION\n" );
        lResult = __LINE__;
    }

    return lResult;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void Help()
{
    printf("GPU-dpi_Run -h\n");
    printf("            -l\n");
    printf("            -?\n");
    printf("            help\n");
    printf("            info\n");
    printf("            {ConfigurationFileName}\n");
    printf("  -h    Display this help message\n");
    printf("  -l    Display the build log\n");
    printf("  -?    Display this help message\n");
    printf("  help  Display this help message\n");
    printf("  info  Display system information\n");
}

void Info()
{
    OpenNet::System * lSystem = OpenNet::System::Create();
    assert(NULL != lSystem);

    unsigned int i;

    unsigned int lCount = lSystem->Adapter_GetCount();
    for (i = 0; i < lCount; i++)
    {
        OpenNet::Adapter * lAdapter = lSystem->Adapter_Get(i);
        assert(NULL != lAdapter);

        lAdapter->Display(stdout);
    }

    lCount = lSystem->Processor_GetCount();
    for (i = 0; i < lCount; i++)
    {
        OpenNet::Processor * lProcessor = lSystem->Processor_Get(i);
        assert(NULL != lProcessor);

        lProcessor->Display(stdout);
    }
}

// aFileName [---;R--] The configuration file name
// aLog
//  false  Display the build log on error
//  true   Always display the build log
// aStatistics
//  false  Do not display statistics
//  true   Display statistics
//
// Return
//      0  OK
//  Ohter  Error
int Run(const char * aFileName, bool aLog, bool aStatistics)
{
    assert(NULL != aFileName);

    GPU_dpi::System * lSystem = GPU_dpi::System::Create();
    assert(NULL != lSystem);

    unsigned int lLine;
    int lResult;

    GPU_dpi::Status lStatus = lSystem->ConfigFile_Read(aFileName, & lLine);
    if (GPU_dpi::STATUS_OK == lStatus)
    {
        lStatus = lSystem->Start();
        if (GPU_dpi::STATUS_OK == lStatus)
        {
            if ( aLog )
            {
                lStatus = lSystem->WriteBuildLog(stdout);
                assert(GPU_dpi::STATUS_OK == lStatus);
            }

            Wait();

            lStatus = lSystem->Stop();
            if (GPU_dpi::STATUS_OK == lStatus)
            {
                if (aStatistics)
                {
                    DisplayStatistics(lSystem);
                }

                lResult = 0;
            }
            else
            {
                DisplayError("GPU_dpi::System::Stop() failed", lStatus);
                lResult = __LINE__;
            }
        }
        else
        {
            DisplayError("GPU_dpi::System::Start() failed", lStatus);

            switch ( lStatus )
            {
            case GPU_dpi::STATUS_CUDA_ERROR    :
            case GPU_dpi::STATUS_INVALID_FILTER:
            case GPU_dpi::STATUS_OPEN_CL_ERROR :
                lStatus = lSystem->WriteBuildLog(stdout);
                assert(GPU_dpi::STATUS_OK == lStatus);
                break;
            }

            lResult = __LINE__;
        }
    }
    else
    {
        fprintf(stderr, "ERROR  GPU_dpi::System::ConfigFile_Read( \"%s\",  ) failed at line %u - ", aFileName, lLine);
        GPU_dpi::Status_Display(lStatus, stderr);
        fprintf(stderr, "\n");
        lResult = __LINE__;
    }

    lSystem->Delete();

    return lResult;
}

void Wait()
{
    signal(SIGINT, OnCtrlC);

    unsigned int lSeconds = 0;

    while (sRunning)
    {
        printf("%u seconds\r", lSeconds);

        KmsLib::ThreadBase::Sleep_s(1);

        lSeconds++;
    }

    printf("\n");
}

// ===== Display ============================================================

// aMsg [---;R--]
// aStatus
void DisplayError(const char * aMsg, GPU_dpi::Status aStatus)
{
    assert(NULL != aMsg);

    fprintf(stderr, "ERROR  %s - ", aMsg);
    GPU_dpi::Status_Display(aStatus, stderr);
    fprintf(stderr, "\n");
}

// aSystem [---;R--]
void DisplayStatistics(GPU_dpi::System * aSystem)
{
    assert(NULL != aSystem);

    GPU_dpi::Status lStatus;

    for (unsigned int i = 0; i < aSystem->Adapter_GetCount(); i++)
    {
        printf("----- Adapter %u -----\n", i);

        lStatus = aSystem->Adapter_DisplayStatistics(i, stdout);
        assert(GPU_dpi::STATUS_OK == lStatus);
    }

    lStatus = aSystem->DisplayStatistics(stdout);
    assert(GPU_dpi::STATUS_OK == lStatus);
}

// ===== Entry point ========================================================

void OnCtrlC(int aSignal)
{
    sRunning = false;
    printf("\nCtrl-C\n");
}
