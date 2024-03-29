
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/System_Internal.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== C++ ================================================================
#include <list>

// ===== Import/Includes ====================================================
#include <KmsLib/DebugLog.h>
#include <OpenNet/Function.h>
#include <OpenNet/System.h>

// ===== Includes ===========================================================
#include <GPU_dpi/System.h>

// ===== GPU_dpi ============================================================
#include "CallbackCaller.h"
#include "Code.h"
#include "FileWriter.h"

// Class
/////////////////////////////////////////////////////////////////////////////

class System_Internal : public GPU_dpi::System
{

public:

    enum
    {
        ADAPTER_QTY = 30,
    };

    System_Internal();

    // ===== GPU_dpi::System ================================================

    virtual GPU_dpi::Status Adapter_DisplayStatistics(unsigned int aIndex, FILE * aOut) const;
    virtual unsigned int    Adapter_GetCount   () const;
    virtual GPU_dpi::Status Adapter_GetConfig  (unsigned int aIndex,       GPU_dpi::AdapterConfig * aConfig) const;
    virtual GPU_dpi::Status Adapter_GetInfo    (unsigned int aIndex,       GPU_dpi::AdapterInfo   * aInfo  ) const;
    virtual GPU_dpi::Status Adapter_ResetConfig(unsigned int aIndex);
    virtual GPU_dpi::Status Adapter_SetConfig  (unsigned int aIndex, const GPU_dpi::AdapterConfig & aConfig);

    virtual GPU_dpi::Status ConfigFile_Read(const char * aFileName, unsigned int * aLine);

    virtual GPU_dpi::Status Display(FILE * aOut) const;
    virtual GPU_dpi::Status DisplayStatistics(FILE * aOut) const;

    virtual unsigned int Processor_GetCount() const;

    virtual GPU_dpi::Status Start();
    virtual GPU_dpi::Status Stop ();
    virtual GPU_dpi::Status Wait ();

    virtual GPU_dpi::Status WriteBuildLog(FILE * aOut);

protected:

    // ===== GPU_dpi::System ================================================
    virtual ~System_Internal();

private:

    typedef enum
    {
        STATE_INIT    ,
        STATE_STARTING,
        STATE_RUNNING ,
        STATE_STOPPING,
        STATE_STOPPED ,

        STATE_QTY
    }
    State;

    typedef struct
    {
        EventProcessor    * mEventProcessor;
        OpenNet::Function * mFunction      ;
        OpenNet::Kernel   * mKernel        ;
    }
    Adapter_Data;

    typedef std::list<char *> BufferList;

    bool            Adapter_IsActive         (unsigned int aIndex) const;
    void            Adapter_Cleanup          (unsigned int aIndex);
    GPU_dpi::Status Adapter_Prepare          (unsigned int aIndex);
    GPU_dpi::Status Adapter_Prepare_Constant (unsigned int aIndex, const Code_Info & aCI, OpenNet::SourceCode * * aSC);
    GPU_dpi::Status Adapter_Prepare_Generated(unsigned int aIndex, const Code_Info & aCI, OpenNet::SourceCode * * aSC);
    GPU_dpi::Status Adapter_Prepare_User     (unsigned int aIndex, const Code_Info & aCI, OpenNet::SourceCode * * aSC);

    GPU_dpi::Status AdapterConfig_Verify(const GPU_dpi::AdapterConfig & aConfig) const;

    void            Adapters_Cleanup();
    GPU_dpi::Status Adapters_Connect();
    GPU_dpi::Status Adapters_Prepare();

    void            ConfigFile_Duplicate     (const char * aStr, char * * aOut);
    GPU_dpi::Status ConfigFile_Read          (FILE * aFile, unsigned int * aLine);
    GPU_dpi::Status ConfigFile_ReadCode      (FILE * aFile, char * * aCode, unsigned int * aCodeSize_byte, unsigned int * aLine);
    void            ConfigFile_ReleaseBuffers();

    void ReplaceAdapterNo (OpenNet::SourceCode * aSC, const char * aText, unsigned int aAdapter);
    void ReplaceAdapterNos(OpenNet::SourceCode * aSC, const GPU_dpi::AdapterConfig & aC, const Code_Info & aCI );

    unsigned int           mAdapter_Count  ;
    GPU_dpi::AdapterConfig mAdapter_Configs[ADAPTER_QTY];
    Adapter_Data           mAdapter_Data   [ADAPTER_QTY];
    BufferList             mConfigFile_Buffers;
    KmsLib::DebugLog       mDebugLog       ;
    std::list<std::string> mFilterErrors   ;
    unsigned int           mProcessor_Count;
    State                  mState          ;
    OpenNet::System      * mSystem         ;

};
