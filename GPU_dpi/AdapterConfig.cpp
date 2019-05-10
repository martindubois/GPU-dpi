
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/AdapterConfig.h

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <io.h>
#include <memory.h>
#include <string.h>

// ===== GPU_dpi ============================================================
#include "Code.h"

#include "AdapterConfig.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#define PACKET_SIZE_MAX_byte ( 16 * 1024 )

// Functions
/////////////////////////////////////////////////////////////////////////////

bool AdapterConfig_IsCapturing(const GPU_dpi::AdapterConfig & aThis)
{
    assert(NULL != (&aThis));

    return ((GPU_dpi::FILTER_TYPE_NO_PACKET != aThis.mFilterType )
        ||  (GPU_dpi::FORWARD_TYPE_NEVER    != aThis.mForwardType));

}

bool AdapterConfig_IsSendingTo(const GPU_dpi::AdapterConfig & aThis, unsigned int aTo)
{
    if (!AdapterConfig_IsCapturing(aThis))
    {
        return false;
    }

    switch (aThis.mForwardType)
    {
    case GPU_dpi::FORWARD_TYPE_ALWAYS      :
    case GPU_dpi::FORWARD_TYPE_FILTERED    :
    case GPU_dpi::FORWARD_TYPE_NOT_FILTERED:
        if (aThis.mForwardAdapter == aTo)
        {
            return true;
        }
        break;

    case GPU_dpi::FORWARD_TYPE_NEVER : break;

    default: assert(false);
    }

    switch (aThis.mOutputType)
    {
    case GPU_dpi::OUTPUT_TYPE_DIRECT :
        if (aThis.mOutputAdapter == aTo)
        {
            return true;
        }
        break;

    case GPU_dpi::OUTPUT_TYPE_CALLBACK:
    case GPU_dpi::OUTPUT_TYPE_FILE    :
    case GPU_dpi::OUTPUT_TYPE_NONE    :
        break;

    default: assert(false);
    }

    return false;
}

void AdapterConfig_Reset(GPU_dpi::AdapterConfig * aThis)
{
    assert(NULL != aThis);

    memset(aThis, 0, sizeof(GPU_dpi::AdapterConfig));

    aThis->mOutputPacketSize_byte = PACKET_SIZE_MAX_byte;
}

GPU_dpi::Status AdapterConfig_Verify(const GPU_dpi::AdapterConfig & aThis)
{
    assert(NULL != (&aThis));

    if (CODE_INVALID == CODES[aThis.mFilterType][aThis.mForwardType][aThis.mOutputType].mType)
    {
        return GPU_dpi::STATUS_INVALID_CONFIGURATION;
    }

    if (NULL == aThis.mFilterCode)
    {
        if (0 != aThis.mFilterCodeSize_byte) { return GPU_dpi::STATUS_INVALID_FILTER_CODE_SIZE; }
    }
    else
    {
        if (   0 >= aThis.mFilterCodeSize_byte) { return GPU_dpi::STATUS_INVALID_FILTER_CODE_SIZE        ; }
        if (NULL != aThis.mFilterFileName     ) { return GPU_dpi::STATUS_USELESS_FILTER_CODE_OR_FILE_NAME; }
    }

    if (NULL != aThis.mFilterFileName)
    {
        if (0 != _access(aThis.mFilterFileName, 04)) { return GPU_dpi::STATUS_INVALID_FILTER_FILE_NAME; }
    }

    switch (aThis.mFilterType)
    {
    case GPU_dpi::FILTER_TYPE_ALL_PACKETS:
    case GPU_dpi::FILTER_TYPE_NO_PACKET  :
        break;

    case GPU_dpi::FILTER_TYPE_FILTER         :
    case GPU_dpi::FILTER_TYPE_OPEN_NET_KERNEL:
        if ((NULL == aThis.mFilterCode) && (NULL == aThis.mFilterFileName)) { return GPU_dpi::STATUS_NO_FILTER_DATA; }
        break;

    case GPU_dpi::FILTER_TYPE_OPEN_NET_FUNCTION  :
        if (NULL == aThis.mFilterFunctionName) { return GPU_dpi::STATUS_NO_FILTER_FUNCTION_NAME; }
        break;

    default: return GPU_dpi::STATUS_INVALID_FILTER_TYPE;
    }

    if ((0 > aThis.mForwardType) || (GPU_dpi::FORWARD_TYPE_QTY <= aThis.mForwardType))
    {
        return GPU_dpi::STATUS_INVALID_FORWARD_TYPE;
    }

    if (NULL != aThis.mOutputFileName)
    {
        if (0 >= strlen(aThis.mOutputFileName)) { return GPU_dpi::STATUS_INVALID_OUTPUT_FILE_NAME; }
    }

    if ((0 > aThis.mOutputFormat) || (GPU_dpi::OUTPUT_FORMAT_QTY <= aThis.mOutputFormat))
    {
        return GPU_dpi::STATUS_INVALID_OUTPUT_ADAPTER;
    }

    if (PACKET_SIZE_MAX_byte < aThis.mOutputPacketSize_byte) { return GPU_dpi::STATUS_INVALID_OUTPUT_PACKET_SIZE; }

    switch (aThis.mOutputType)
    {
    case GPU_dpi::OUTPUT_TYPE_CALLBACK :
        if (NULL                        == aThis.mOutputCallback        ) { return GPU_dpi::STATUS_NO_OUTPUT_CALLBACK             ; }
        if (NULL                        != aThis.mOutputFileName        ) { return GPU_dpi::STATUS_USELESS_OUTPUT_FILE_NAME       ; }
        if (GPU_dpi::OUTPUT_FORMAT_NONE != aThis.mOutputFormat          ) { return GPU_dpi::STATUS_UNSUPPORTED_OUTPUT_FORMAT      ; }
        break;

    case GPU_dpi::OUTPUT_TYPE_DIRECT :
        if (NULL                        != aThis.mOutputCallback        ) { return GPU_dpi::STATUS_USELESS_OUTPUT_CALLBACK        ; }
        if (NULL                        != aThis.mOutputCallback_Context) { return GPU_dpi::STATUS_USELESS_OUTPUT_CALLBACK_CONTEXT; }
        if (NULL                        != aThis.mOutputFileName        ) { return GPU_dpi::STATUS_USELESS_OUTPUT_FILE_NAME       ; }
        if (GPU_dpi::OUTPUT_FORMAT_NONE != aThis.mOutputFormat          ) { return GPU_dpi::STATUS_UNSUPPORTED_OUTPUT_FORMAT      ; }
        break;

    case GPU_dpi::OUTPUT_TYPE_FILE :
        if (NULL                        != aThis.mOutputCallback        ) { return GPU_dpi::STATUS_USELESS_OUTPUT_CALLBACK        ; }
        if (NULL                        != aThis.mOutputCallback_Context) { return GPU_dpi::STATUS_USELESS_OUTPUT_CALLBACK_CONTEXT; }
        if (NULL                        == aThis.mOutputFileName        ) { return GPU_dpi::STATUS_NO_OUTPUT_FILE_NAME            ; }
        if (GPU_dpi::OUTPUT_FORMAT_PCAP != aThis.mOutputFormat          ) { return GPU_dpi::STATUS_UNSUPPORTED_OUTPUT_FORMAT      ; }
        break;

    case GPU_dpi::OUTPUT_TYPE_NONE   :
        if (NULL                        != aThis.mOutputCallback        ) { return GPU_dpi::STATUS_USELESS_OUTPUT_CALLBACK        ; }
        if (NULL                        != aThis.mOutputCallback_Context) { return GPU_dpi::STATUS_USELESS_OUTPUT_CALLBACK_CONTEXT; }
        if (NULL                        != aThis.mOutputFileName        ) { return GPU_dpi::STATUS_USELESS_OUTPUT_FILE_NAME       ; }
        if (GPU_dpi::OUTPUT_FORMAT_NONE != aThis.mOutputFormat          ) { return GPU_dpi::STATUS_UNSUPPORTED_OUTPUT_FORMAT      ; }
        break;

    default: return GPU_dpi::STATUS_INVALID_OUTPUT_TYPE;
    }

    if (0 >= aThis.mOutputPacketSize_byte) { return GPU_dpi::STATUS_INVALID_OUTPUT_PACKET_SIZE; }

    if (AdapterConfig_IsCapturing(aThis))
    {
        if (4 > aThis.mBufferQty) { return GPU_dpi::STATUS_NOT_ENOUGH_BUFFER; }
    }

    return GPU_dpi::STATUS_OK;
}
