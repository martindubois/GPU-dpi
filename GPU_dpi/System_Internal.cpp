
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/System_Internal.h

#define __CLASS__ "System_Internal::"

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <io.h>
#include <stdint.h>

#ifdef _KMS_WINDOWS_
    // ===== Windows ========================================================
    #include <Windows.h>
#endif

// ===== Import/Includes ====================================================
#include <KmsLib/Exception.h>
#include <KmsLib/FileHandle.h>
#include <OpenNet/Adapter.h>
#include <OpenNet/Function.h>
#include <OpenNet/Kernel.h>

// ===== GPU_dpi ============================================================
#include "AdapterConfig.h"
#include "FileWriter_PCAP.h"
#include "Utils.h"

#include "System_Internal.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static GPU_dpi::Status ConvertException    (const KmsLib::Exception * aE);

static char * ReadCodeFile(const char * aFileName, unsigned int * aSize_byte);

// ===== Adapter ============================================================
static void Adapter_Config(OpenNet::Adapter * aAdapter, const GPU_dpi::AdapterConfig & aConfig);

// ===== Generate ===========================================================
static const char * GenerateFromPatterList             (const GPU_dpi::AdapterConfig & aConfig, const char * aIn, unsigned int aInSize_byte, unsigned int * aCodeSize_byte, const char * * aFunctionName);
static const char * GenerateFromBinaryPatternList      (const GPU_dpi::AdapterConfig & aConfig, const char * aIn, unsigned int aInSize_byte, unsigned int * aCodeSize_byte, const char * * aFunctionName);
static const char * GenerateFromRegExp                 (const GPU_dpi::AdapterConfig & aConfig, const char * aIn, unsigned int aInSize_byte, unsigned int * aCodeSize_byte, const char * * aFunctionName);
static const char * GenerateFromWiresharkFilter        (const GPU_dpi::AdapterConfig & aConfig, const char * aIn, unsigned int aInSize_byte, unsigned int * aCodeSize_byte, const char * * aFunctionName);
static const char * GenerateFromCompiledWiresharkFilter(const GPU_dpi::AdapterConfig & aConfig, const char * aIn, unsigned int aInSize_byte, unsigned int * aCodeSize_byte, const char * * aFunctionName);

// Public
/////////////////////////////////////////////////////////////////////////////

// NOT TESTED  GPU_dpi.System_Internal.Error
//             OpenNet::System::Create fails
System_Internal::System_Internal() : mDebugLog("K:\\Dossiers_Actifs\\GPU-dpi\\DebugLog", "System")
{
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

GPU_dpi::Status System_Internal::Display(FILE * aOut) const
{
    if (NULL == aOut)
    {
        return GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT;
    }

    return GPU_dpi::STATUS_OK;
}

unsigned int System_Internal::Processor_GetCount() const
{
    return mProcessor_Count;
}

GPU_dpi::Status System_Internal::Start()
{
    assert(ADAPTER_QTY >= mAdapter_Count);

    if (0 >= mAdapter_Count)
    {
        return GPU_dpi::STATUS_NO_ADAPTER;
    }

    mDebugLog.Log(__FILE__, __CLASS__ "Start", __LINE__);

    GPU_dpi::Status lResult = Adapters_Connect();
    if (GPU_dpi::STATUS_OK == lResult)
    {
        lResult = Adapters_Prepare();
        if (GPU_dpi::STATUS_OK == lResult)
        {
            OpenNet::Status lStatus = mSystem->Start(0);
            if (OpenNet::STATUS_OK != lStatus)
            {
                lResult = Utl_ConvertOpenNetStatus(lStatus);
            }
        }
    }

    return lResult;
}

GPU_dpi::Status System_Internal::Stop()
{
    assert(NULL != mSystem);

    OpenNet::Status lStatus = mSystem->Stop();
    if (OpenNet::STATUS_OK != lStatus)
    {
        return Utl_ConvertOpenNetStatus(lStatus);
    }

    mDebugLog.Log(__FILE__, __CLASS__ "Stop", __LINE__);

    Adapters_Cleanup();

    return GPU_dpi::STATUS_OK;
}

GPU_dpi::Status System_Internal::WriteBuildLog(FILE * aOut)
{
    if (NULL == aOut)
    {
        return GPU_dpi::STATUS_NOT_ALLOWED_NULL_ARGUMENT;
    }

    unsigned int lCount = mSystem->Kernel_GetCount();
    for (unsigned int i = 0; i < lCount; i++)
    {
        fprintf(aOut, "\n\n========== %u ==========\n\n", i);
        
        OpenNet::Kernel * lKernel = mSystem->Kernel_Get(i);
        assert(NULL != lKernel);

        lKernel->Display(aOut);

        // fprintf(aOut, "%s", lKernel->GetBuildLog());
    }

    return GPU_dpi::STATUS_OK;
}

// Protected
/////////////////////////////////////////////////////////////////////////////

System_Internal::~System_Internal()
{
    assert(NULL != mSystem);

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
            if (GPU_dpi::OUTPUT_TYPE_FILE == lAC.mOutputType)
            {
                Adapter_Data * lAD = mAdapter_Data + aIndex;

                switch (lAC.mOutputFormat)
                {
                case GPU_dpi::OUTPUT_FORMAT_PCAP:
                    mDebugLog.LogTime();
                    mDebugLog.Log(__FILE__, __CLASS__ "Adapter_Prepare", __LINE__);
                    mDebugLog.Log(lAC.mOutputFileName);

                    try
                    {
                        lAD->mFileWriter = new FileWriter_PCAP(lAC.mOutputFileName);
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

                if (GPU_dpi::STATUS_OK == lResult)
                {
                    assert(NULL != lAD->mFileWriter);

                    lStatus = lA->Event_RegisterCallback(lAD->mFileWriter->GetEventCallback(), lAD->mFileWriter);
                    assert(OpenNet::STATUS_OK == lStatus);
                }
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

    const char * lCode          = NULL;
    unsigned int lCodeSize_byte =    0;
    const char * lFunctionName  = NULL;

    switch (lAC.mFilterType)
    {
    case GPU_dpi::FILTER_TYPE_PATTERN_LIST       : lCode = GenerateFromPatterList             (lAC, lIn, lInSize_byte, &lCodeSize_byte, &lFunctionName); break;
    case GPU_dpi::FILTER_TYPE_PATTERN_LIST_BINARY: lCode = GenerateFromBinaryPatternList      (lAC, lIn, lInSize_byte, &lCodeSize_byte, &lFunctionName); break;
    case GPU_dpi::FILTER_TYPE_REG_EXP            : lCode = GenerateFromRegExp                 (lAC, lIn, lInSize_byte, &lCodeSize_byte, &lFunctionName); break;
    case GPU_dpi::FILTER_TYPE_WIRESHARK          : lCode = GenerateFromWiresharkFilter        (lAC, lIn, lInSize_byte, &lCodeSize_byte, &lFunctionName); break;
    case GPU_dpi::FILTER_TYPE_WIRESHARK_COMPILED : lCode = GenerateFromCompiledWiresharkFilter(lAC, lIn, lInSize_byte, &lCodeSize_byte, &lFunctionName); break;

    default: assert(false);
    };

    lAD->mFunction = new OpenNet::Function();
    assert(NULL != lAD->mFunction);

    OpenNet::Status lStatus = lAD->mFunction->SetCode(lCode, lCodeSize_byte);
    assert(OpenNet::STATUS_OK == lStatus);

    lStatus = lAD->mFunction->SetFunctionName(lFunctionName);
    assert(OpenNet::STATUS_OK == lStatus);

    delete[] lCode;

    if (lAC.mFilterCode != lIn)
    {
        delete[] lIn;
    }

    (*aSC) = lAD->mFunction;

    return GPU_dpi::STATUS_OK;
}

// aIndex        Index of the adapter to prepare using user provided code
// aCI [---;R--] Code information
// aSC [---;-W-] The methode return the adresse of the new
//               OpenNet::SourceCode instance here
GPU_dpi::Status System_Internal::Adapter_Prepare_User(unsigned int aIndex, const Code_Info & aCI, OpenNet::SourceCode * * aSC)
{
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

// aFileName  [---;R--]
// aSize_byte [---;-W-]
//
// Return  This function return the address of the read data. The caller is
//         responsible for releasing the memory using the delete [] operator.
char * ReadCodeFile(const char * aFileName, unsigned int * aSize_byte)
{
    assert(NULL != aFileName );
    assert(NULL != aSize_byte);

    HANDLE lHandle = CreateFile(aFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    assert(INVALID_HANDLE_VALUE != lHandle);

    LARGE_INTEGER lSize_byte;

    BOOL lRetB = GetFileSizeEx(lHandle, &lSize_byte);
    assert(lRetB);

    unsigned int lInSize_byte = static_cast<unsigned int>(lSize_byte.QuadPart);

    char * lResult = new char[lInSize_byte];
    assert(NULL != lResult);

    lResult[lInSize_byte] = '\0';

    DWORD lInfo_byte;

    lRetB = ReadFile(lHandle, lResult, lInSize_byte, &lInfo_byte, NULL);
    assert(lRetB                            );
    assert(lSize_byte.QuadPart == lInfo_byte);

    lRetB = CloseHandle(lHandle);
    assert(lRetB);

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

// ===== Generate ===========================================================
// aConfig        [---;R--]
// aIn            [---;R--]
// aInSize_byte
// aCodeSize_byte [---;-W-]
// aFunctionName  [---;-W-]
//
// Return  These function return the generated code. The caller is
//         responsible for releasing this code using delete[].

const char * GenerateFromPatterList(const GPU_dpi::AdapterConfig & aConfig, const char * aIn, unsigned int aInSize_byte, unsigned int * aCodeSize_byte, const char * * aFunctionName)
{
    assert(NULL != (&aConfig)    );
    assert(NULL != aIn           );
    assert(   0 <  aInSize_byte  );
    assert(NULL != aCodeSize_byte);
    assert(NULL != aFunctionName );

    return NULL;
}

const char * GenerateFromBinaryPatternList(const GPU_dpi::AdapterConfig & aConfig, const char * aIn, unsigned int aInSize_byte, unsigned int * aCodeSize_byte, const char * * aFunctionName)
{
    assert(NULL != (&aConfig)    );
    assert(NULL != aIn           );
    assert(   0 <  aInSize_byte  );
    assert(NULL != aCodeSize_byte);
    assert(NULL != aFunctionName );

    return NULL;
}

const char * GenerateFromRegExp(const GPU_dpi::AdapterConfig & aConfig, const char * aIn, unsigned int aInSize_byte, unsigned int * aCodeSize_byte, const char * * aFunctionName)
{
    assert(NULL != (&aConfig)    );
    assert(NULL != aIn           );
    assert(   0 <  aInSize_byte  );
    assert(NULL != aCodeSize_byte);
    assert(NULL != aFunctionName );

    return NULL;
}

const char * GenerateFromWiresharkFilter(const GPU_dpi::AdapterConfig & aConfig, const char * aIn, unsigned int aInSize_byte, unsigned int * aCodeSize_byte, const char * * aFunctionName)
{
    assert(NULL != (&aConfig)    );
    assert(NULL != aIn           );
    assert(   0 <  aInSize_byte  );
    assert(NULL != aCodeSize_byte);
    assert(NULL != aFunctionName );

    return NULL;
}

const char * GenerateFromCompiledWiresharkFilter(const GPU_dpi::AdapterConfig & aConfig, const char * aIn, unsigned int aInSize_byte, unsigned int * aCodeSize_byte, const char * * aFunctionName)
{
    assert(NULL != (&aConfig)    );
    assert(NULL != aIn           );
    assert(   0 <  aInSize_byte  );
    assert(NULL != aCodeSize_byte);
    assert(NULL != aFunctionName );

    return NULL;
}
