
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/System_Internal.h

#define __CLASS__ "System_Internal::"

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <stdint.h>
#include <string.h>

#ifdef _KMS_LINUX_

    #include <fcntl.h>

    // ===== System =========================================================
    #include <sys/stat.h>

#endif

#ifdef _KMS_WINDOWS_

    #include <io.h>

    // ===== Windows ========================================================
    #include <Windows.h>

#endif

// ===== Import/Includes ====================================================
#include <KmsLib/Exception.h>
#include <KmsLib/FileHandle.h>
#include <KmsLib/ThreadBase.h>
#include <OpenNet/Adapter.h>
#include <OpenNet/Buffer.h>
#include <OpenNet/Function.h>
#include <OpenNet/Kernel.h>

// ===== Common =============================================================
#include "../Common/Filter.h"

// ===== GPU_dpi ============================================================
#include "AdapterConfig.h"
#include "CallbackCaller.h"
#include "FileWriter_PCAP.h"
#include "Utils.h"

#include "System_Internal.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#ifdef _KMS_LINUX_
    #define DEBUG_LOG_FOLDER "/tmp/GPU-dpiDebugLog"
#endif

#ifdef _KMS_WINDOWS_
    #define DEBUG_LOG_FOLDER "K:\\Dossiers_Actifs\\GPU-dpi\\DebugLog"
#endif

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static GPU_dpi::Status ConvertException    (const KmsLib::Exception * aE);

static bool IsBlank(char aC);

static char * ReadCodeFile(const char * aFileName, unsigned int * aSize_byte);

// ===== Adapter ============================================================
static void Adapter_Config(OpenNet::Adapter * aAdapter, const GPU_dpi::AdapterConfig & aConfig);

// ===== ConfigFile =========================================================

static GPU_dpi::Status ConfigFile_ConvertFilterType  (const char * aStr, GPU_dpi::FilterType   * aOut);
static GPU_dpi::Status ConfigFile_ConvertForwardType (const char * aStr, GPU_dpi::ForwardType  * aOut);
static GPU_dpi::Status ConfigFile_ConvertOutputFormat(const char * aStr, GPU_dpi::OutputFormat * aOut);
static GPU_dpi::Status ConfigFile_ConvertOutputType  (const char * aStr, GPU_dpi::OutputType   * aOut);

static bool ConfigFile_ReadLine    (FILE * aFile, char * aLine, unsigned int * aNo);
static bool ConfigFile_ReadLine_Raw(FILE * aFile, char * aLine, unsigned int * aNo);

// ===== Processor ==========================================================
static void Processor_Config(OpenNet::Processor * aProcessor, const GPU_dpi::AdapterConfig & aConfig);

// Public
/////////////////////////////////////////////////////////////////////////////

// NOT TESTED  GPU_dpi.System_Internal.Error
//             OpenNet::System::Create fails
System_Internal::System_Internal() : mDebugLog( DEBUG_LOG_FOLDER, "System"), mState(STATE_INIT)
{
    memset( & mAdapter_Data, 0, sizeof( mAdapter_Data ) );

    for (unsigned int i = 0; i < ADAPTER_QTY; i++)
    {
        AdapterConfig_Reset(mAdapter_Configs + i);
    }

    mSystem = OpenNet::System::Create();
    if (NULL == mSystem)
    {
        throw new KmsLib::Exception(KmsLib::Exception::CODE_ERROR,
            "OpenNet::System::Create() failed", NULL, __FILE__, __FUNCTION__, __LINE__, 0);
    }

    mAdapter_Count = mSystem->Adapter_GetCount();
    assert(ADAPTER_QTY >= mAdapter_Count);

    mProcessor_Count = mSystem->Processor_GetCount();

    if (mDebugLog.IsEnabled())
    {
        char lMsg[1024];
        sprintf_s(lMsg, "%u adapters, %u processors\n", mAdapter_Count, mProcessor_Count);
        mDebugLog.Log(lMsg);
    }
}

// ===== GPU_dpi::System ====================================================

GPU_dpi::Status System_Internal::Adapter_GetConfig(unsigned int aIndex, GPU_dpi::AdapterConfig * aConfig) const
{
    assert(ADAPTER_QTY >= mAdapter_Count);
    assert(NULL        != mSystem       );

    if (NULL == aConfig)
    {
        return GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT;
    }

    if (mAdapter_Count <= aIndex)
    {
        return GPU_dpi::STATUS_INVALID_ADAPTER_INDEX;
    }

    memcpy(aConfig, mAdapter_Configs + aIndex, sizeof(GPU_dpi::AdapterConfig));

    return GPU_dpi::STATUS_OK;
}

GPU_dpi::Status System_Internal::Adapter_DisplayStatistics(unsigned int aIndex, FILE * aOut) const
{
    assert(NULL != mSystem);

    if (NULL == aOut)
    {
        return GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT;
    }

    if (mAdapter_Count <= aIndex)
    {
        return GPU_dpi::STATUS_INVALID_ADAPTER_INDEX;
    }

    OpenNet::Adapter * lA = mSystem->Adapter_Get(aIndex);
    assert(NULL != lA);

    unsigned int lInfo_byte;
    unsigned int lS[128];

    OpenNet::Status lStatus = lA->GetStatistics(lS, sizeof(lS), &lInfo_byte);
    assert(OpenNet::STATUS_OK == lStatus);

    lStatus = lA->DisplayStatistics(lS, lInfo_byte, aOut);
    assert(OpenNet::STATUS_OK == lStatus);

    EventProcessor * lEP = mAdapter_Data[aIndex].mEventProcessor;
    if (NULL != lEP)
    {
        lEP->DisplayStatistics(aOut);
    }

    return GPU_dpi::STATUS_OK;
}

unsigned int System_Internal::Adapter_GetCount() const
{
    assert(ADAPTER_QTY >= mAdapter_Count);

    return mAdapter_Count;
}

GPU_dpi::Status System_Internal::Adapter_GetInfo(unsigned int aIndex, GPU_dpi::AdapterInfo * aInfo) const
{
    assert(ADAPTER_QTY >= mAdapter_Count);
    assert(NULL        != mSystem       );

    if (NULL == aInfo)
    {
        return GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT;
    }

    if (mAdapter_Count <= aIndex)
    {
        return GPU_dpi::STATUS_INVALID_ADAPTER_INDEX;
    }

    OpenNet::Adapter * lAdapter = mSystem->Adapter_Get(aIndex);
    assert(NULL != lAdapter);

    OpenNet::Adapter::Info lInfo;

    OpenNet::Status lStatus = lAdapter->GetInfo(&lInfo);
    if (OpenNet::Status::STATUS_OK != lStatus)
    {
        return Utl_ConvertOpenNetStatus(lStatus);
    }

    memset(aInfo, 0, sizeof(GPU_dpi::AdapterInfo));

    strncpy_s(aInfo->mName, lInfo.mComment, sizeof(aInfo->mName) - 1);

    return GPU_dpi::STATUS_OK;
}

GPU_dpi::Status System_Internal::Adapter_ResetConfig(unsigned int aIndex)
{
    assert(ADAPTER_QTY >= mAdapter_Count);

    if (mAdapter_Count <= aIndex)
    {
        return GPU_dpi::STATUS_INVALID_ADAPTER_INDEX;
    }

    AdapterConfig_Reset(mAdapter_Configs + aIndex);

    return GPU_dpi::STATUS_OK;
}

GPU_dpi::Status System_Internal::Adapter_SetConfig(unsigned int aIndex, const GPU_dpi::AdapterConfig & aConfig)
{
    assert(ADAPTER_QTY >= mAdapter_Count);

    if (NULL == (&aConfig))
    {
        return GPU_dpi::STATUS_INVALID_REFERENCE;
    }

    if (mAdapter_Count <= aIndex)
    {
        return GPU_dpi::STATUS_INVALID_ADAPTER_INDEX;
    }

    GPU_dpi::Status lResult = AdapterConfig_Verify(aConfig);
    if (GPU_dpi::STATUS_OK == lResult)
    {
        memcpy(mAdapter_Configs + aIndex, &aConfig, sizeof(GPU_dpi::AdapterConfig));
    }

    return lResult;
}

GPU_dpi::Status System_Internal::ConfigFile_Read(const char * aFileName, unsigned int * aLine)
{
    if (NULL == aFileName)
    {
        return GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT;
    }

    if (NULL != aLine)
    {
        (*aLine) = 0;
    }

    FILE * lFile;

    int lRet = fopen_s(&lFile, aFileName, "r");
    if (0 != lRet)
    {
        return GPU_dpi::STATUS_CANNOT_OPEN_CONFIG_FILE;
    }

    GPU_dpi::Status lResult = ConfigFile_Read(lFile, aLine);

    lRet = fclose(lFile);
    assert(0 == lRet);

    return lResult;
}

GPU_dpi::Status System_Internal::Display(FILE * aOut) const
{
    if (NULL == aOut)
    {
        return GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT;
    }

    return GPU_dpi::STATUS_OK;
}

GPU_dpi::Status System_Internal::DisplayStatistics(FILE * aOut) const
{
    assert(NULL != mSystem);

    if (NULL == aOut)
    {
        return GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT;
    }

    unsigned int lCount = mSystem->Kernel_GetCount();
    for (unsigned int i = 0; i < lCount; i++)
    {
        OpenNet::Kernel * lK = mSystem->Kernel_Get(i);
        assert(NULL != lK);

        unsigned int lInfo_byte;
        unsigned int lS[128];

        OpenNet::Status lStatus = lK->GetStatistics(lS, sizeof(lS), &lInfo_byte, false);
        assert(OpenNet::STATUS_OK == lStatus);

        fprintf(aOut, "----- Kernel %u -----\n", i);

        lStatus = lK->DisplayStatistics(lS, lInfo_byte, aOut);
        assert(OpenNet::STATUS_OK == lStatus);
    }

    return GPU_dpi::STATUS_OK;
}

unsigned int System_Internal::Processor_GetCount() const
{
    return mProcessor_Count;
}

GPU_dpi::Status System_Internal::Start()
{
    mDebugLog.Log(__FILE__, __CLASS__ "Start", __LINE__);

    assert(ADAPTER_QTY >= mAdapter_Count);

    if (0 >= mAdapter_Count)
    {
        return GPU_dpi::STATUS_NO_ADAPTER;
    }

    Adapters_Cleanup();

    mFilterErrors.clear();

    GPU_dpi::Status lResult = GPU_dpi::STATUS_ASSERT_FAILED;

    switch (mState)
    {
    case STATE_INIT   :
    case STATE_STOPPED:
        mState = STATE_STARTING;

        lResult = Adapters_Connect();
        if (GPU_dpi::STATUS_OK == lResult)
        {
            lResult = Adapters_Prepare();
            if (GPU_dpi::STATUS_OK == lResult)
            {
                OpenNet::Status lStatus = mSystem->Start(0);
                lResult = Utl_ConvertOpenNetStatus(lStatus);
            }
        }

        mState = (GPU_dpi::STATUS_OK == lResult) ? mState = STATE_RUNNING : mState = STATE_INIT;
        break;

    case STATE_RUNNING : lResult = GPU_dpi::STATUS_SYSTEM_RUNNING ; break;
    case STATE_STARTING: lResult = GPU_dpi::STATUS_SYSTEM_STARTING; break;
    case STATE_STOPPING: lResult = GPU_dpi::STATUS_SYSTEM_STOPPING; break;

    default: assert(false);
    }

    return lResult;
}

GPU_dpi::Status System_Internal::Stop()
{
    mDebugLog.Log(__FILE__, __CLASS__ "Stop", __LINE__);

    assert(NULL != mSystem);

    GPU_dpi::Status lResult = GPU_dpi::STATUS_ASSERT_FAILED;

    switch (mState)
    {
    case STATE_INIT   : lResult = GPU_dpi::STATUS_SYSTEM_NOT_STARTED; break;

    case STATE_RUNNING:
        mState = STATE_STOPPING;

        OpenNet::Status lStatus;

        lStatus = mSystem->Stop();
        lResult = Utl_ConvertOpenNetStatus(lStatus);

        mState = STATE_STOPPED;
        break;

    case STATE_STARTING: lResult = GPU_dpi::STATUS_SYSTEM_STARTING; break;
    case STATE_STOPPED : lResult = GPU_dpi::STATUS_SYSTEM_STOPPED ; break;
    case STATE_STOPPING: lResult = GPU_dpi::STATUS_SYSTEM_STOPPING; break;

    default: assert(false);
    }

    return lResult;
}

GPU_dpi::Status System_Internal::Wait()
{
    for(;;)
    {
        KmsLib::ThreadBase::Sleep_ms(500);

        switch (mState)
        {
        case STATE_RUNNING :
            for (unsigned int i = 0; i < mAdapter_Count; i++)
            {
                EventProcessor * lEP = mAdapter_Data[i].mEventProcessor;
                if (NULL != lEP)
                {
                    if (0 < lEP->GetErrorCount())
                    {
                        return Stop();
                    }
                }
            }
            break;

        case STATE_STARTING:
        case STATE_STOPPING:
            break;

        case STATE_STOPPED: return GPU_dpi::STATUS_OK;

        default:
            assert(false);
            return GPU_dpi::STATUS_ASSERT_FAILED;
        }
    }
}

GPU_dpi::Status System_Internal::WriteBuildLog(FILE * aOut)
{
    if (NULL == aOut)
    {
        return GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT;
    }

    unsigned int lIndex = 0;

    for (std::list<std::string>::iterator lIt = mFilterErrors.begin(); lIt != mFilterErrors.end(); lIt++)
    {
        assert( NULL != lIt->c_str() );

        fprintf(aOut, "\n\n========== Filter error %u ==========\n", lIndex);
        fprintf(aOut, "%s\n", lIt->c_str());

        lIndex++;
    }

    unsigned int lCount = mSystem->Kernel_GetCount();
    for (unsigned int i = 0; i < lCount; i++)
    {
        fprintf(aOut, "\n\n========== %u ==========\n\n", i);
        
        OpenNet::Kernel * lKernel = mSystem->Kernel_Get(i);
        assert(NULL != lKernel);

        // lKernel->Display(aOut);

        const char * lLog = lKernel->GetBuildLog();
        assert( NULL != lLog );

        fprintf(aOut, "%s", lLog );
    }

    return GPU_dpi::STATUS_OK;
}

// Protected
/////////////////////////////////////////////////////////////////////////////

System_Internal::~System_Internal()
{
    assert(NULL != mSystem);

    Adapters_Cleanup();

    ConfigFile_ReleaseBuffers();

    mSystem->Delete();
}

// Private
/////////////////////////////////////////////////////////////////////////////

// Is the adapter active?
//
// aIndex  The index of the adapter
bool System_Internal::Adapter_IsActive(unsigned int aIndex) const
{
    assert(mAdapter_Count > aIndex);

    assert(ADAPTER_QTY >= mAdapter_Count);

    const GPU_dpi::AdapterConfig & lAC = mAdapter_Configs[aIndex];

    if (AdapterConfig_IsCapturing(lAC))
    {
        return true;
    }

    for (unsigned int i = 0; i < mAdapter_Count; i++)
    {
        if (AdapterConfig_IsSendingTo(lAC, i))
        {
            return true;
        }
    }

    return false;
}

// aIndex  Index of adapter to cleanup
//
// Adapter_Prepare ==> Adapter_Cleanup
void System_Internal::Adapter_Cleanup(unsigned int aIndex)
{
    assert(ADAPTER_QTY > aIndex);

    Adapter_Data * lAD = mAdapter_Data + aIndex;

    if (NULL != lAD->mKernel)
    {
        // new ==> delete  See Adapter_Prepare_User
        delete lAD->mKernel;
    }

    if (NULL != lAD->mFunction)
    {
        // new ==> delete  See Adapter_Prepare_...
        delete lAD->mFunction;
    }

    if (NULL != lAD->mEventProcessor)
    {
        delete lAD->mEventProcessor;
    }

    memset(lAD, 0, sizeof(Adapter_Data));
}

// aIndex  The index of the adapter to prepare
//
// Adapter_Prepare ==> Adapter_Cleanup
GPU_dpi::Status System_Internal::Adapter_Prepare(unsigned int aIndex)
{
    mDebugLog.Log(__FILE__, __CLASS__ "Adapter_Prepare", __LINE__);

    assert(ADAPTER_QTY > aIndex);

    const GPU_dpi::AdapterConfig & lAC = mAdapter_Configs[aIndex];
    const Code_Info              & lCI = CODES[lAC.mFilterType][lAC.mForwardType][lAC.mOutputType];

    assert(GPU_dpi::FILTER_TYPE_QTY  > lAC.mFilterType );
    assert(GPU_dpi::FORWARD_TYPE_QTY > lAC.mForwardType);
    assert(GPU_dpi::OUTPUT_TYPE_QTY  > lAC.mOutputType );

    OpenNet::SourceCode * lSC = NULL;
    GPU_dpi::Status       lResult;

    switch (lCI.mType)
    {
    case CODE_CONSTANT : lResult = Adapter_Prepare_Constant (aIndex, lCI, &lSC); break;
    case CODE_GENERATED: lResult = Adapter_Prepare_Generated(aIndex, lCI, &lSC); break;
    case CODE_USER     : lResult = Adapter_Prepare_User     (aIndex, lCI, &lSC); break;

    default: assert(false);
    }

    if (GPU_dpi::STATUS_OK == lResult)
    {
        ReplaceAdapterNos(lSC, lAC, lCI);

        OpenNet::Adapter * lA = mSystem->Adapter_Get(aIndex);
        assert(NULL != lA);

        OpenNet::Status lStatus = lA->SetInputFilter(lSC);
        if (OpenNet::STATUS_OK == lStatus)
        {
            Adapter_Data * lAD = mAdapter_Data + aIndex;

            switch ( lAC.mOutputType )
            {
            case GPU_dpi::OUTPUT_TYPE_CALLBACK:
                lAD->mEventProcessor = new CallbackCaller(lAC.mOutputCallback, lAC.mOutputCallback_Context);
                break;

            case GPU_dpi::OUTPUT_TYPE_DIRECT: break;

            case GPU_dpi::OUTPUT_TYPE_FILE :
                switch (lAC.mOutputFormat)
                {
                case GPU_dpi::OUTPUT_FORMAT_PCAP:
                    mDebugLog.LogTime();
                    mDebugLog.Log(__FILE__, __CLASS__ "Adapter_Prepare", __LINE__);
                    mDebugLog.Log(lAC.mOutputFileName);

                    try
                    {
                        lAD->mEventProcessor = new FileWriter_PCAP(lAC.mOutputFileName);
                    }
                    catch (KmsLib::Exception * eE)
                    {
                        mDebugLog.Log(__FILE__, __CLASS__ "Adapter_Prepare", __LINE__);
                        mDebugLog.Log(eE);

                        lResult = ConvertException(eE);
                    }
                    break;

                default: assert(false);
                }
                break;

            default: assert(false);
            }

            if (GPU_dpi::STATUS_OK == lResult)
            {
                if (NULL != lAD->mEventProcessor)
                {
                    lStatus = lA->Event_RegisterCallback(lAD->mEventProcessor->GetEventCallback(), lAD->mEventProcessor);
                    assert(OpenNet::STATUS_OK == lStatus);
                }

                lStatus = lA->ResetStatistics();
                assert(OpenNet::STATUS_OK == lStatus);
            }
        }
        else
        {
            lResult = Utl_ConvertOpenNetStatus(lStatus);
        }
    }

    return lResult;
}

// Prepare an adapter using constant code
//
// aIndex        Index of the adapter to prepare using constant code
// aCI [---;R--] Code information
// aSC [---;-W-] The methode return the adresse of the new
//               OpenNet::SourceCode instance here
GPU_dpi::Status System_Internal::Adapter_Prepare_Constant(unsigned int aIndex, const Code_Info & aCI, OpenNet::SourceCode * * aSC)
{
    mDebugLog.Log(__FILE__, __CLASS__ "Adapter_Prepare_Constant", __LINE__);

    assert(ADAPTER_QTY >  aIndex);
    assert(NULL        != (&aCI));
    assert(NULL        != aSC   );

    assert(NULL != mSystem);

    const GPU_dpi::AdapterConfig & lAC = mAdapter_Configs[aIndex];
    Adapter_Data                 * lAD = mAdapter_Data  + aIndex ;

    // new ==> delete  See Adapter_Cleanup
    lAD->mFunction = new OpenNet::Function();
    assert(NULL != lAD->mFunction);

    OpenNet::Status lStatus = lAD->mFunction->SetCode(aCI.mCode, aCI.mCodeSize_byte);
    assert(OpenNet::STATUS_OK == lStatus);

    lStatus = lAD->mFunction->SetFunctionName(aCI.mFunctionName);
    assert(OpenNet::STATUS_OK == lStatus);

    (*aSC) = lAD->mFunction;

    return GPU_dpi::STATUS_OK;
}

// aIndex        Index of the adapter to prepare using generated code
// aCI [---;R--] Code information
// aSC [---;-W-] The methode return the adresse of the new
//               OpenNet::SourceCode instance here
GPU_dpi::Status System_Internal::Adapter_Prepare_Generated(unsigned int aIndex, const Code_Info & aCI, OpenNet::SourceCode * * aSC)
{
    mDebugLog.Log(__FILE__, __CLASS__ "Adapter_Prepare_Generated", __LINE__);

    assert(ADAPTER_QTY >  aIndex);
    assert(NULL        != (&aCI));
    assert(NULL        != aSC   );

    const GPU_dpi::AdapterConfig & lAC = mAdapter_Configs[aIndex];
    Adapter_Data                 * lAD = mAdapter_Data  + aIndex ;

    const char * lIn;
    unsigned int lInSize_byte;

    if (NULL != lAC.mFilterCode)
    {
        assert(0 < lAC.mFilterCodeSize_byte);

        lIn          = lAC.mFilterCode         ;
        lInSize_byte = lAC.mFilterCodeSize_byte;
    }
    else
    {
        lIn = ReadCodeFile(lAC.mFilterFileName, &lInSize_byte);
        assert(NULL != lIn         );
        assert(   0 <  lInSize_byte);
    }

    Filter * lFilter = Filter::Create(lAC.mOutputType, lAC.mForwardType, lIn);

    if (lAC.mFilterCode != lIn)
    {
        delete[] lIn;
    }

    GPU_dpi::Status lResult;

    const char * lErrMsg = lFilter->GetErrorMessage();
    if (NULL == lErrMsg)
    {
        lAD->mFunction = new OpenNet::Function();
        assert(NULL != lAD->mFunction);

        OpenNet::Status lStatus = lAD->mFunction->SetCode(lFilter->GetCode(), lFilter->GetCodeSize());
        if (OpenNet::STATUS_OK == lStatus)
        {
            lStatus = lAD->mFunction->SetFunctionName(lFilter->GetFunctionName());
            assert(OpenNet::STATUS_OK == lStatus);

            (*aSC) = lAD->mFunction;

            lResult = GPU_dpi::STATUS_OK;
        }
        else
        {
            delete lAD->mFunction;
            lAD->mFunction = NULL;

            lResult = Utl_ConvertOpenNetStatus(lStatus);
        }
    }
    else
    {
        mFilterErrors.push_back(lErrMsg);
        lResult = GPU_dpi::STATUS_INVALID_FILTER;
    }

    lFilter->Delete();

    return lResult;
}

// aIndex        Index of the adapter to prepare using user provided code
// aCI [---;R--] Code information
// aSC [---;-W-] The methode return the adresse of the new
//               OpenNet::SourceCode instance here
GPU_dpi::Status System_Internal::Adapter_Prepare_User(unsigned int aIndex, const Code_Info & aCI, OpenNet::SourceCode * * aSC)
{
    mDebugLog.Log(__FILE__, __CLASS__ "Adapter_Prepare_User", __LINE__);

    assert(ADAPTER_QTY >  aIndex);
    assert(NULL        != (&aCI));
    assert(NULL        != aSC   );

    assert(NULL != mSystem);

    const GPU_dpi::AdapterConfig & lAC = mAdapter_Configs[aIndex];
    Adapter_Data                 * lAD = mAdapter_Data  + aIndex ;
    OpenNet::SourceCode          * lSC = NULL;

    OpenNet::Status lStatus;

    switch (lAC.mFilterType)
    {
    case GPU_dpi::FILTER_TYPE_OPEN_NET_FUNCTION :
        assert(NULL != lAC.mFilterFunctionName);

        // new ==> delete  See Adapter_Cleanup
        lAD->mFunction = new OpenNet::Function();
        assert(NULL != lAD->mFunction);
        
        lStatus = lAD->mFunction->SetFunctionName(lAC.mFilterFunctionName);
        assert(OpenNet::STATUS_OK == lStatus);

        lSC = lAD->mFunction;
        break;

    case GPU_dpi::FILTER_TYPE_OPEN_NET_KERNEL :
        // new ==> delete  See Adapter_Cleanup
        lAD->mKernel = new OpenNet::Kernel();
        assert(NULL != lAD->mKernel);

        lSC = lAD->mKernel;
        break;

    default: assert(false);
    }

    if (NULL != lAC.mFilterCode)
    {
        assert(0 < lAC.mFilterCodeSize_byte);

        lStatus = lSC->SetCode(lAC.mFilterCode, lAC.mFilterCodeSize_byte);
    }
    else
    {
        assert(NULL != lAC.mFilterFileName);

        lStatus = lSC->SetCode(lAC.mFilterFileName);
    }

    (*aSC) = lSC;

    return GPU_dpi::STATUS_OK;
}

// aConfig  [---;R--]  The configuration to verify
GPU_dpi::Status System_Internal::AdapterConfig_Verify(const GPU_dpi::AdapterConfig & aConfig) const
{
    assert(NULL != (&aConfig));

    assert(0 < mAdapter_Count);

    GPU_dpi::Status lStatus = ::AdapterConfig_Verify(aConfig);
    if (GPU_dpi::STATUS_OK != lStatus)
    {
        return lStatus;
    }

    if (mAdapter_Count   <= aConfig.mForwardAdapter) { return GPU_dpi::STATUS_INVALID_FORWARD_ADAPTER; }
    if (mAdapter_Count   <= aConfig.mOutputAdapter ) { return GPU_dpi::STATUS_INVALID_OUTPUT_ADAPTER ; }
    if (mProcessor_Count <= aConfig.mProcessorIndex) { return GPU_dpi::STATUS_INVALID_PROCESSOR_INDEX; }

    return GPU_dpi::STATUS_OK;
}

// Call Adapter_Cleanup for all active adapters
void System_Internal::Adapters_Cleanup()
{
    assert(ADAPTER_QTY >= mAdapter_Count);

    for (unsigned int i = 0; i < mAdapter_Count; i++)
    {
        if (Adapter_IsActive(i))
        {
            Adapter_Cleanup(i);
        }
    }
}

// Connect all active adapters
GPU_dpi::Status System_Internal::Adapters_Connect()
{
    assert(ADAPTER_QTY >  mAdapter_Count);
    assert(NULL        != mSystem       );

    unsigned int lActive = 0;

    for (unsigned int i = 0; i < mAdapter_Count; i++)
    {
        if (Adapter_IsActive(i))
        {
            mDebugLog.Log(__FILE__, __CLASS__ "Adapters_Connect", __LINE__);

            lActive++;

            const GPU_dpi::AdapterConfig & lAC = mAdapter_Configs[i];

            OpenNet::Adapter * lA = mSystem->Adapter_Get(i);
            assert(NULL != lA);

            OpenNet::Status lStatus;

            if (AdapterConfig_IsCapturing(lAC))
            {
                OpenNet::Processor * lP = mSystem->Processor_Get(lAC.mProcessorIndex);
                assert(NULL != lP);

                lStatus = lA->SetProcessor(lP);
                assert(OpenNet::STATUS_OK == lStatus);

                Adapter_Config(lA, lAC);
                Processor_Config(lP, lAC);
            }

            lStatus = mSystem->Adapter_Connect(lA);
            if (OpenNet::STATUS_OK != lStatus)
            {
                return Utl_ConvertOpenNetStatus(lStatus);
            }
        }
    }

    if (0 >= lActive)
    {
        return GPU_dpi::STATUS_NO_ACTIVE_ADAPTER;
    }

    return GPU_dpi::STATUS_OK;
}

// Call Adapter_Prepare for all active adapters
GPU_dpi::Status System_Internal::Adapters_Prepare()
{
    for (unsigned int i = 0; i < mAdapter_Count; i++)
    {
        if (Adapter_IsActive(i))
        {
            GPU_dpi::Status lStatus = Adapter_Prepare(i);
            if (GPU_dpi::STATUS_OK != lStatus)
            {
                return lStatus;
            }
        }
    }

    return GPU_dpi::STATUS_OK;
}

void System_Internal::ConfigFile_Duplicate(const char * aStr, char * * aOut)
{
    assert(NULL != aStr);
    assert(NULL != aOut);

    unsigned int lSize_byte = static_cast<unsigned int>(strlen(aStr) + 1);
    char       * lPtr       = new char[lSize_byte];

    assert(   2 <= lSize_byte);
    assert(NULL != lPtr      );

    strcpy_s(lPtr SIZE_INFO(lSize_byte), aStr);

    mConfigFile_Buffers.push_back(lPtr);

    (*aOut) = lPtr;
}

GPU_dpi::Status System_Internal::ConfigFile_Read(FILE * aFile, unsigned int * aLine)
{
    assert(NULL != aFile);

    if (NULL != aLine)
    {
        (*aLine) = 0;
    }

    char lLine[1024];

    GPU_dpi::AdapterConfig lAC;
    unsigned int           lAI = 0xffffffff;
    GPU_dpi::Status        lResult = GPU_dpi::STATUS_OK;

    while ((lResult == GPU_dpi::STATUS_OK) && ConfigFile_ReadLine(aFile, lLine, aLine))
    {
        char         lStr[1024];
        unsigned int lStr_byte = sizeof(lStr);
        unsigned int lValue;

        if (1 == sscanf_s(lLine, "Adapter %u", &lValue))
        {
            if (0xffffffff != lAI)
            {
                lResult = Adapter_SetConfig(lAI, lAC);
                lAI     = 0xffffffff;
            }

            if (GPU_dpi::STATUS_OK == lResult)
            {
                lAI     = lValue;
                lResult = Adapter_GetConfig(lAI, &lAC);
            }
        }
        else
        {
            if (0xffffffff == lAI)
            {
                return GPU_dpi::STATUS_NO_ACTIVE_ADAPTER;
            }

            if      (1 == sscanf_s(lLine, "FilterCode = %[^\n\r]"           , lStr SIZE_INFO(lStr_byte))) { ConfigFile_Duplicate(lStr , const_cast<char * *>(&lAC.mFilterCode        )); lAC.mFilterCodeSize_byte = static_cast<unsigned int>(strlen(lStr)); }
            else if (1 == sscanf_s(lLine, "FilterFunctionName = %[^ \n\r\t]", lStr SIZE_INFO(lStr_byte))) { ConfigFile_Duplicate(lStr , const_cast<char * *>(&lAC.mFilterFunctionName)); lAC.mFilterType          = GPU_dpi::FILTER_TYPE_OPEN_NET_FUNCTION; }
            else if (1 == sscanf_s(lLine, "OutputFileName = %[^\n\r\t]"     , lStr SIZE_INFO(lStr_byte))) { ConfigFile_Duplicate(lStr , const_cast<char * *>(&lAC.mOutputFileName    )); lAC.mOutputType          = GPU_dpi::OUTPUT_TYPE_FILE; }
            else if (1 == sscanf_s(lLine, "FilterFileName = %[^\n\r\t]"     , lStr SIZE_INFO(lStr_byte))) { ConfigFile_Duplicate(lStr , const_cast<char * *>(&lAC.mFilterFileName    )); }
            else if (0 == strncmp (lLine, "FilterCode"                      , 10                       )) { lResult = ConfigFile_ReadCode(aFile, const_cast<char * *>(&lAC.mFilterCode), &lAC.mFilterCodeSize_byte, aLine); }
            else if (1 == sscanf_s(lLine, "ProfilingEnable = %u"            , &lValue                  )) { lAC.mFlags.mProfilingEnabled = (0 != lValue); }
            else if (1 == sscanf_s(lLine, "BufferQty = %u"                  , &lValue                  )) { lAC.mBufferQty               =       lValue ; }
            else if (1 == sscanf_s(lLine, "ForwardAdapter = %u"             , &lValue                  )) { lAC.mForwardAdapter          =       lValue ; }
            else if (1 == sscanf_s(lLine, "OutputAdapter = %u"              , &lValue                  )) { lAC.mOutputAdapter           =       lValue ; lAC.mOutputType = GPU_dpi::OUTPUT_TYPE_DIRECT; }
            else if (1 == sscanf_s(lLine, "OutputPacketSize = %u"           , &lValue                  )) { lAC.mOutputPacketSize_byte   =       lValue ; }
            else if (1 == sscanf_s(lLine, "ProcessorIndex = %u"             , &lValue                  )) { lAC.mProcessorIndex          =       lValue ; }
            else if (1 == sscanf_s(lLine, "FilterType = %[_A-Z]"            , lStr SIZE_INFO(lStr_byte))) { lResult = ConfigFile_ConvertFilterType  (lStr, &lAC.mFilterType  ); }
            else if (1 == sscanf_s(lLine, "ForwardType = %[_A-Z]"           , lStr SIZE_INFO(lStr_byte))) { lResult = ConfigFile_ConvertForwardType (lStr, &lAC.mForwardType ); }
            else if (1 == sscanf_s(lLine, "OutputFormat = %[_A-Z]"          , lStr SIZE_INFO(lStr_byte))) { lResult = ConfigFile_ConvertOutputFormat(lStr, &lAC.mOutputFormat); }
            else if (1 == sscanf_s(lLine, "OutputType = %[_A-Z]"            , lStr SIZE_INFO(lStr_byte))) { lResult = ConfigFile_ConvertOutputType  (lStr, &lAC.mOutputType  ); }
            else
            {
                return GPU_dpi::STATUS_INVALID_CONFIG_FILE;
            }
        }
    }

    if ((GPU_dpi::STATUS_OK == lResult) && (0xffffffff != lAI))
    {
        lResult = Adapter_SetConfig(lAI, lAC);
    }

    return lResult;
}

GPU_dpi::Status System_Internal::ConfigFile_ReadCode(FILE * aFile, char * * aCode, unsigned int * aCodeSize_byte, unsigned int * aLine)
{
    assert(NULL != aFile         );
    assert(NULL != aCode         );
    assert(NULL != aCodeSize_byte);

    char            lLine[1024];
    char          * lPtr       = new char[16384];
    GPU_dpi::Status lResult    = GPU_dpi::STATUS_OK;
    unsigned int    lSize_byte = 0;

    assert(NULL != lPtr);

    memset(lPtr, 0, 16384);

    while ((GPU_dpi::STATUS_OK == lResult) && ConfigFile_ReadLine_Raw(aFile, lLine, aLine))
    {
        if (0 == strncmp("END", lLine, 3))
        {
            break;
        }

        unsigned int lLineSize_byte = static_cast<unsigned int>(strlen(lLine));
        unsigned int lNewSize_byte  = lSize_byte + lLineSize_byte;

        if (16384 <= lNewSize_byte)
        {
            lResult = GPU_dpi::STATUS_CODE_TOO_LONG;
            break;
        }

        memcpy(lPtr + lSize_byte, lLine, lLineSize_byte);
        lSize_byte = lNewSize_byte;
    }

    if (GPU_dpi::STATUS_OK == lResult)
    {
        (*aCode         ) = lPtr      ;
        (*aCodeSize_byte) = lSize_byte;

        mConfigFile_Buffers.push_back(lPtr);
    }
    else
    {
        delete[] lPtr;
    }

    return lResult;
}

void System_Internal::ConfigFile_ReleaseBuffers()
{
    for (BufferList::iterator lIt = mConfigFile_Buffers.begin(); lIt != mConfigFile_Buffers.end(); lIt++)
    {
        delete[](*lIt);
    }

    mConfigFile_Buffers.clear();
}

// aSC   [---;RW-] The OpenNet::SourceCode instance
// aText [---;R--] The text to replace
// aAdapter        Replace the text using the numero of this adapter
void System_Internal::ReplaceAdapterNo(OpenNet::SourceCode * aSC, const char * aText, unsigned int aAdapter)
{
    assert(NULL != mSystem);

    OpenNet::Adapter * lA = mSystem->Adapter_Get(aAdapter);
    assert(NULL != lA);

    unsigned int lNo;

    OpenNet::Status lStatus = lA->GetAdapterNo(&lNo);
    assert(OpenNet::STATUS_OK == lStatus);

    char lNoStr[16];

    sprintf_s(lNoStr, "%u", lNo);

    unsigned int lCount = aSC->Edit_Replace(aText, lNoStr);
    assert(0 < lCount);
}

// Replace the adapter no into the code, if needed
//
// aSC [---;RW-] The OpenNet::SourceCode instance
// aAC [---;R--] The configuration of the adapter
// aCI [---;R--] The code information
void System_Internal::ReplaceAdapterNos(OpenNet::SourceCode * aSC, const GPU_dpi::AdapterConfig & aAC, const Code_Info & aCI)
{
    assert(NULL !=   aSC );
    assert(NULL != (&aAC));
    assert(NULL != (&aCI));

    if (aCI.mReplaceForwardAdapter)
    {
        ReplaceAdapterNo(aSC, "FORWARD_ADAPTER", aAC.mForwardAdapter);
    }

    if (aCI.mReplaceOutputAdapter)
    {
        ReplaceAdapterNo(aSC, "OUTPUT_ADAPTER", aAC.mOutputAdapter);
    }
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

// aE  [---;R--]  The exception to convert
GPU_dpi::Status ConvertException(const KmsLib::Exception * aE)
{
    assert(NULL != aE);

    KmsLib::Exception::Code lCode = aE->GetCode();
    switch (lCode)
    {
    case KmsLib::Exception::CODE_IO_ERROR: return GPU_dpi::STATUS_IO_ERROR;
    }

    fprintf(stderr, "%s (0x%08x) ==> GPU_dpi::STATUS_UNEXPECTED_EXCEPTION_CODE\n", KmsLib::Exception::GetCodeName(lCode), lCode);

    aE->Write(stderr);

    return GPU_dpi::STATUS_UNEXPECTED_EXCEPTION_CODE;
}

// aC
//
// Return
//  false  C is not a blank
//  true   C is a blank
bool IsBlank(char aC)
{
    switch (aC)
    {
    case ' '  :
    case '\n' :
    case '\r' :
    case '\t' :
        return true;
    }

    return false;
}

// aFileName  [---;R--]
// aSize_byte [---;-W-]
//
// Return  This function return the address of the read data. The caller is
//         responsible for releasing the memory using the delete [] operator.
char * ReadCodeFile(const char * aFileName, unsigned int * aSize_byte)
{
    assert(NULL != aFileName );
    assert(NULL != aSize_byte);

    unsigned int lInSize_byte;

    #ifdef _KMS_LINUX_

        int lHandle = open( aFileName, O_RDONLY );
        assert( 0 <= lHandle );

        struct stat lStat;

        int lRet = fstat( lHandle, & lStat );
        assert( 0 == lRet );

        lInSize_byte = lStat.st_size;

    #endif

    #ifdef _KMS_WINDOWS_

        HANDLE lHandle = CreateFile(aFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        assert(INVALID_HANDLE_VALUE != lHandle);

        LARGE_INTEGER lSize_byte;

        BOOL lRetB = GetFileSizeEx(lHandle, &lSize_byte);
        assert(lRetB);

        lInSize_byte = static_cast<unsigned int>(lSize_byte.QuadPart);

    #endif

    char * lResult = new char[lInSize_byte];
    assert(NULL != lResult);

    lResult[lInSize_byte] = '\0';

    #ifdef _KMS_LINUX_

        ssize_t lInfo_byte = read( lHandle, lResult, lInSize_byte );
        assert( lInSize_byte == lInfo_byte );

        lRet = close( lHandle );
        assert( 0 == lRet );

    #endif

    #ifdef _KMS_WINDOWS_

        DWORD lInfo_byte;

        lRetB = ReadFile(lHandle, lResult, lInSize_byte, &lInfo_byte, NULL);
        assert(lRetB                            );
        assert(lSize_byte.QuadPart == lInfo_byte);

        lRetB = CloseHandle(lHandle);
        assert(lRetB);

    #endif

    (*aSize_byte) = lInSize_byte;

    return lResult;
}

// ===== Adapter ============================================================

void Adapter_Config(OpenNet::Adapter * aAdapter, const GPU_dpi::AdapterConfig & aConfig)
{
    assert(NULL != aAdapter  );
    assert(NULL != (&aConfig));

    OpenNet::Adapter::Config lConfig;

    OpenNet::Status lStatus = aAdapter->GetConfig(&lConfig);
    assert(OpenNet::STATUS_OK == lStatus);

    lConfig.mBufferQty = aConfig.mBufferQty;

    lStatus = aAdapter->SetConfig(lConfig);
    assert(OpenNet::STATUS_OK == lStatus);
}

// ===== ConfigFile =========================================================

// aStr [---;R--] The string to convert
// aOut [---;RW-] The output buffer
//
// Return
//  STATUS_OK
//  STATUS_INVALID_FILTER_TYPE
GPU_dpi::Status ConfigFile_ConvertFilterType(const char * aStr, GPU_dpi::FilterType * aOut)
{
    assert(NULL != aStr);
    assert(NULL != aOut);

    if (0 == strncmp("ALL_PACKETS"      , aStr, 11)) { (*aOut) = GPU_dpi::FILTER_TYPE_ALL_PACKETS      ; return GPU_dpi::STATUS_OK; }
    if (0 == strncmp("FILTER"           , aStr,  6)) { (*aOut) = GPU_dpi::FILTER_TYPE_FILTER           ; return GPU_dpi::STATUS_OK; }
    if (0 == strncmp("NO_PACKET"        , aStr,  9)) { (*aOut) = GPU_dpi::FILTER_TYPE_NO_PACKET        ; return GPU_dpi::STATUS_OK; }
    if (0 == strncmp("OPEN_NET_FUNCTION", aStr, 17)) { (*aOut) = GPU_dpi::FILTER_TYPE_OPEN_NET_FUNCTION; return GPU_dpi::STATUS_OK; }
    if (0 == strncmp("OPEN_NET_KERNEL"  , aStr, 15)) { (*aOut) = GPU_dpi::FILTER_TYPE_OPEN_NET_KERNEL  ; return GPU_dpi::STATUS_OK; }

    return GPU_dpi::STATUS_INVALID_FILTER_TYPE;
}

// aStr [---;R--] The string to convert
// aOut [---;RW-] The output buffer
//
// Return
//  STATUS_OK
//  STATUS_INVALID_FORWARD_TYPE
GPU_dpi::Status ConfigFile_ConvertForwardType(const char * aStr, GPU_dpi::ForwardType * aOut)
{
    assert(NULL != aStr);
    assert(NULL != aOut);

    if (0 == strncmp("ALWAYS"      , aStr,  6)) { (*aOut) = GPU_dpi::FORWARD_TYPE_ALWAYS      ; return GPU_dpi::STATUS_OK; }
    if (0 == strncmp("FILTERED"    , aStr,  8)) { (*aOut) = GPU_dpi::FORWARD_TYPE_FILTERED    ; return GPU_dpi::STATUS_OK; }
    if (0 == strncmp("NEVER"       , aStr,  5)) { (*aOut) = GPU_dpi::FORWARD_TYPE_NEVER       ; return GPU_dpi::STATUS_OK; }
    if (0 == strncmp("NOT_FILTERED", aStr, 12)) { (*aOut) = GPU_dpi::FORWARD_TYPE_NOT_FILTERED; return GPU_dpi::STATUS_OK; }

    return GPU_dpi::STATUS_INVALID_FORWARD_TYPE;
}

// aStr [---;R--] The string to convert
// aOut [---;RW-] The output buffer
//
// Return
//  STATUS_OK
//  STATUS_INVALID_OUTPUT_FORMAT
GPU_dpi::Status ConfigFile_ConvertOutputFormat(const char * aStr, GPU_dpi::OutputFormat * aOut)
{
    assert(NULL != aStr);
    assert(NULL != aOut);

    if (0 == strncmp("NONE", aStr, 4)) { (*aOut) = GPU_dpi::OUTPUT_FORMAT_NONE; return GPU_dpi::STATUS_OK; }
    if (0 == strncmp("PCAP", aStr, 4)) { (*aOut) = GPU_dpi::OUTPUT_FORMAT_PCAP; return GPU_dpi::STATUS_OK; }

    return GPU_dpi::STATUS_INVALID_OUTPUT_FORMAT;
}

// aStr [---;R--] The string to convert
// aOut [---;RW-] The output buffer
//
// Return
//  STATUS_OK
//  STATUS_INVALID_OUTPUT_TYPE
GPU_dpi::Status ConfigFile_ConvertOutputType(const char * aStr, GPU_dpi::OutputType * aOut)
{
    assert(NULL != aStr);
    assert(NULL != aOut);

    if (0 == strncmp("DIRECT", aStr, 6)) { (*aOut) = GPU_dpi::OUTPUT_TYPE_DIRECT; return GPU_dpi::STATUS_OK; }
    if (0 == strncmp("FILE"  , aStr, 4)) { (*aOut) = GPU_dpi::OUTPUT_TYPE_FILE  ; return GPU_dpi::STATUS_OK; }

    return GPU_dpi::STATUS_INVALID_OUTPUT_TYPE;
}

// aFile [---;RW-] The stream to read from
// aLine [---;-W-] The output buffer
// aNo   [--O;RW-] The line number to increment
//
// Return
//  false  No more line
//  true   OK
bool ConfigFile_ReadLine(FILE * aFile, char * aLine, unsigned int * aNo)
{
    assert(NULL != aFile);
    assert(NULL != aLine);

    for(;;)
    {
        char lLine[1024];

        if (!ConfigFile_ReadLine_Raw(aFile, lLine, aNo))
        {
            return false;
        }

        unsigned int lIndex = 0;
        while (IsBlank(lLine[lIndex]))
        {
            lIndex++;
        }

        if (('\0' != lLine[lIndex]) && ('#' != lLine[lIndex]) && ('\n' != lLine[lIndex]) && ('\r' != lLine[lIndex]))
        {
            char * lPtr = strpbrk(aLine, "#\n\r");
            if (NULL == lPtr)
            {
                strcat_s(lLine, "\n");
            }
            else
            {
                strcpy_s(lPtr SIZE_INFO(2), "\n");
            }

            strcpy_s(aLine SIZE_INFO( 1024 ), lLine + lIndex);

            break;
        }
    }

    return true;
}

// aFile [---;RW-] The stream to read from
// aLine [---;-W-] The output buffer
// aNo   [--O;RW-] The line number to increment
//
// Return
//  false  No more line
//  true   OK
bool ConfigFile_ReadLine_Raw(FILE * aFile, char * aLine, unsigned int * aNo)
{
    assert(NULL != aFile);
    assert(NULL != aLine);

    if (NULL == fgets(aLine, 1024, aFile))
    {
        return false;
    }

    if (NULL != aNo)
    {
        (*aNo)++;
    }

    return true;
}

// ===== Processor ==========================================================

void Processor_Config(OpenNet::Processor * aProcessor, const GPU_dpi::AdapterConfig & aConfig)
{
    assert(NULL != aProcessor);
    assert(NULL != (&aConfig));

    OpenNet::Processor::Config lConfig;

    OpenNet::Status lStatus = aProcessor->GetConfig(&lConfig);
    assert(OpenNet::STATUS_OK == lStatus);

    lConfig.mFlags.mProfilingEnabled = aConfig.mFlags.mProfilingEnabled;

    lStatus = aProcessor->SetConfig(lConfig);
    assert(OpenNet::STATUS_OK == lStatus);
}
