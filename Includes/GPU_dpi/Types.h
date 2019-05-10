
// Product   GPU-dpi

/// \author  KMS - Martin Dubois, ing.
/// \file    Includes/GPU_dpi/Types.h
/// \brief   GPU-dpi:: (SDK)

#pragma once

namespace GPU_dpi
{

    // Data types
    /////////////////////////////////////////////////////////////////////////

    /// \cond en
    /// \brief  Adapter information
    /// \todo   Document members of GPU_dpi::System::Adapter_Info
    /// \endcond
    /// \cond fr
    /// \brief  Information au sujet d'un adaptateur
    /// \todo   Documenter les membres de GPU_dpi::System::Adapter_Info
    /// \endcond
    typedef struct
    {
        char mName[64];

        unsigned char mReserved0[64];
    }
    AdapterInfo;

    /// \cond en
    /// \brief  Filter type
    /// \todo   Document members of GPU_dpi::System::FilterType
    /// \endcond
    /// \cond fr
    /// \brief  Type de filtre
    /// \todo   Documenter les membres de GPU_dpi::System::FilterType
    /// \endcond
    typedef enum
    {
        FILTER_TYPE_NO_PACKET = 0,

        FILTER_TYPE_ALL_PACKETS      ,
        FILTER_TYPE_FILTER           ,
        FILTER_TYPE_OPEN_NET_FUNCTION,
        FILTER_TYPE_OPEN_NET_KERNEL  ,

        FILTER_TYPE_QTY
    }
    FilterType;

    /// \cond en
    /// \brief  Forward type
    /// \todo   Document members of GPU_dpi::System::ForwardType
    /// \endcond
    /// \cond fr
    /// \brief  Type de retransmission
    /// \todo   Documenter les membres de GPU_dpi::System::ForwardType
    /// \endcond
    typedef enum
    {
        FORWARD_TYPE_NEVER = 0,

        FORWARD_TYPE_ALWAYS      ,
        FORWARD_TYPE_FILTERED    ,
        FORWARD_TYPE_NOT_FILTERED,

        FORWARD_TYPE_QTY
    }
    ForwardType;

    /// \cond en
    /// \brief  Output callback
    /// \endcond
    /// \cond fr
    /// \brief  Fonction de sortie
    /// \endcond
    typedef void(*OutputCallback)(void * aContext, const void * aData, unsigned int aSize_byte);

    /// \cond en
    /// \brief  Output format
    /// \todo   Document members of GPU_dpi::System::OutputFormat
    /// \endcond
    /// \cond fr
    /// \brief  Format de sortie
    /// \todo   Documenter les membres de GPU_dpi::System::OutputFormat
    /// \endcond
    typedef enum
    {
        OUTPUT_FORMAT_NONE = 0,

        OUTPUT_FORMAT_PCAP,

        OUTPUT_FORMAT_QTY
    }
    OutputFormat;

    /// \cond en
    /// \brief  Output type
    /// \todo   Document members of GPU_dpi::System::OutputType
    /// \endcond
    /// \cond fr
    /// \brief  Type de sortie
    /// \todo   Documenter les membres de GPU_dpi::System::OutputType
    /// \endcond
    typedef enum
    {
        OUTPUT_TYPE_NONE = 0,

        OUTPUT_TYPE_CALLBACK,
        OUTPUT_TYPE_DIRECT  ,
        OUTPUT_TYPE_FILE    ,

        OUTPUT_TYPE_QTY
    }
    OutputType;

    /// \cond en
    /// \brief  Adapter config
    /// \todo   Document members of GPU_dpi::System::Adapter_Config
    /// \endcond
    /// \cond fr
    /// \brief  Configuration d'un adaptateur
    /// \todo   Documenter les membres de GPU_dpi::System::Adapter_Config
    /// \endcond
    typedef struct
    {
        const char   * mFilterCode            ;
        const char   * mFilterFunctionName    ;
        const char   * mFilterFileName        ;
        OutputCallback mOutputCallback        ;
        void         * mOutputCallback_Context;
        const char   * mOutputFileName        ;

        struct
        {
            unsigned mProfilingEnabled : 1;

            unsigned mReserved0 : 31;
        }
        mFlags;

        unsigned int mBufferQty            ;
        unsigned int mFilterCodeSize_byte  ;
        FilterType   mFilterType           ;
        unsigned int mForwardAdapter       ;
        ForwardType  mForwardType          ;
        unsigned int mOutputAdapter        ;
        OutputFormat mOutputFormat         ;
        unsigned int mOutputPacketSize_byte;
        OutputType   mOutputType           ;
        unsigned int mProcessorIndex       ;

        unsigned char mReserved0[48];
    }
    AdapterConfig;

}
