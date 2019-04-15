
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Filter_Internal.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <stdio.h>
#include <string.h>

// ===== CodeGen ============================================================
#include "Filter_Internal.h"

// Public
/////////////////////////////////////////////////////////////////////////////

Filter_Internal::Filter_Internal(GPU_dpi::OutputType aOutputType, GPU_dpi::ForwardType aForwardType, const char * aFunctionName)
    : mCode            (NULL)
    , mCodeMaxSize_byte(   0)
    , mCodeSize_byte   (   0)
    , mErrorMessage    (NULL)
    , mForwardType     (aForwardType )
    , mFunctionName    (aFunctionName)
    , mIndent          (   0)
    , mOutputType      (aOutputType  )
{
    assert(GPU_dpi::OUTPUT_TYPE_QTY  >  aOutputType  );
    assert(GPU_dpi::FORWARD_TYPE_QTY >  aForwardType );
    assert(NULL                      != aFunctionName);
}

// ===== CodeGen ============================================================

const char * Filter_Internal::GetCode() const
{
    return mCode;
}

unsigned int Filter_Internal::GetCodeSize() const
{
    return mCodeSize_byte;
}

const char * Filter_Internal::GetErrorMessage() const
{
    return mErrorMessage;
}

const char * Filter_Internal::GetFunctionName() const
{
    return mFunctionName;
}

void Filter_Internal::SetErrorMessage(const char * aErrMsg)
{
    assert(NULL != aErrMsg);

    assert(NULL == mErrorMessage);

    mErrorMessage = aErrMsg;
}

void Filter_Internal::Append(const char * aLine)
{
    assert(NULL != aLine);

    unsigned int lLineLen = static_cast<unsigned int>(strlen(aLine));
    assert(0 < lLineLen);

    unsigned int lSize_byte = lLineLen;

    if ('\n' != aLine[0])
    {
        lSize_byte += mIndent;
    }

    VerifySize(mCodeSize_byte + lSize_byte + 1);

    if ('\n' != aLine[0])
    {
        memset(mCode + mCodeSize_byte, ' ', mIndent);
        mCodeSize_byte += mIndent;
    }

    memcpy(mCode + mCodeSize_byte, aLine, lLineLen);
    mCodeSize_byte += lLineLen;
}

void Filter_Internal::Begin()
{
    assert(NULL != mFunctionName);

    char lLine[1024];

    Append("#include <OpenNetK/Ethernet.h>" EOL);
    Append("#include <OpenNetK/IPv4.h>"     EOL);
    Append("#include <OpenNetK/IPv6.h>"     EOL);
    Append("#include <OpenNetK/TCP.h>"      EOL);
    Append("#include <OpenNetK/UDP.h>"      EOL);
    Append(                                 EOL);

    sprintf_s(lLine, "OPEN_NET_FUNCTION_DECLARATION( %s )" EOL, mFunctionName);
    Append(lLine);

    Append("{" EOL);
    Indent_Inc();
    Append(    "OPEN_NET_FUNCTION_BEGIN" EOL);
    Indent_Inc();
    Append(                                                                                                EOL);
    Append(        "OPEN_NET_GLOBAL unsigned char * lEthernet      = Etherent_Data( lBase, lPacketInfo );" EOL);
    Append(        "unsigned short                  lEthernet_Type = Ethernet_Type( lBase, lPacketInfo );" EOL);
    Append(                                                                                                EOL);
    Append(        "OPEN_NET_GLOBAL unsigned char * lIP;"                                                  EOL);
    Append(        "unsigned char                   lIP_Protocol;"                                         EOL);
    Append(                                                                                                EOL);
    Append(        "switch ( lEthernet_Type )"                                                             EOL);
    Append(        "{"                                                                                     EOL);
    Append(        "case IPv4_ETHERNET_TYPE_nh :"                                                          EOL);
    Indent_Inc();
    Append(            "lIP          = IPv4_Data    ( lEthernet );" EOL);
    Append(            "lIP_Protocol = IPv4_Protocol( lEthernet );" EOL);
    Append(            "break; "                                    EOL);
    Indent_Dec();
    Append(        "case IPv6_ETHERNET_TYPE_nh :" EOL);
    Indent_Inc();
    Append(            "lIP          = IPv6_Data    ( lEthernet );" EOL);
    Append(            "lIP_Protocol = IPv6_Protocol( lEthernet );" EOL);
    Append(            "break; "                                    EOL);
    Indent_Dec();
    Append(        "default : lIP = NULL;" EOL);
    Append(        "}"                     EOL);
    Append(                                EOL);
    Append(        "int lR;"               EOL);
}

void Filter_Internal::End()
{
    switch (mOutputType)
    {
    case GPU_dpi::OUTPUT_TYPE_DIRECT :
        Append(        "if ( lR )" EOL);
        Append(        "{"         EOL);
        Indent_Inc();

        switch (mForwardType)
        {
        case GPU_dpi::FORWARD_TYPE_ALWAYS :
            Append(        "lPacketInfo->mSendTo = ( 1 << OUTPUT_ADAPTER ) | ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            AppendElse();
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_FITERED :
            Append(        "lPacketInfo->mSendTo = ( 1 << OUTPUT_ADAPTER ) | ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            AppendElse();
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_NEVER :
            Append(        "lPacketInfo->mSendTo = ( 1 << OUTPUT_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            AppendElse();
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_NOT_FILTERED :
            Append(        "lPacketInfo->mSendTo = ( 1 << OUTPUT_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            AppendElse();
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        default: assert(false);
        }

        Indent_Dec();
        Append(        "}" EOL);
        Indent_Dec();
        Append(                                 EOL);
        Append(    "OPEN_NET_FUNCTION_END( 0 )" EOL);
        break;

    case GPU_dpi::OUTPUT_TYPE_FILE :
        Append(        "if ( lR )" EOL);
        Append(        "{"         EOL);
        Indent_Inc();

        switch (mForwardType)
        {
        case GPU_dpi::FORWARD_TYPE_ALWAYS :
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_OUTPUT ) | OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;" EOL);
            AppendElse();
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_OUTPUT ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_FITERED :
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_OUTPUT ) | OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;" EOL);
            AppendElse();
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_NEVER :
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;" EOL);
            AppendElse();
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_NOT_FILTERED :
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;" EOL);
            AppendElse();
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        default: assert(false);
        }

        Indent_Dec();
        Append(        "}" EOL);

        // TODO  Dev  Reduction
        break;

    case GPU_dpi::OUTPUT_TYPE_NONE :
        switch (mForwardType)
        {
        case GPU_dpi::FORWARD_TYPE_ALWAYS  :
            Append(    "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_FITERED :
            Append(    "if ( lR )" EOL);
            Append(    "{"         EOL);
            Indent_Inc();
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            AppendElse();
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            Indent_Dec();
            Append(    "}"         EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_NOT_FILTERED :
            Append(    "if ( lR )" EOL);
            Append(    "{"         EOL);
            Indent_Inc();
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            AppendElse();
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            Indent_Dec();
            Append(    "}"         EOL);
            break;

        default: assert(false);
        }

        Indent_Dec();
        Append(    "OPEN_NET_FUNCTION_END( 0 )" EOL);
    }

    Indent_Dec();
    Append("}\n");

    assert(0 == mIndent);
}

void Filter_Internal::Indent_Inc()
{
    mIndent += 4;
}

void Filter_Internal::Indent_Dec()
{
    assert(4 <= mIndent);

    mIndent -= 4;
}

// Protected
/////////////////////////////////////////////////////////////////////////////

// ===== CodeGen ============================================================

Filter_Internal::~Filter_Internal()
{
    if (NULL != mCode)
    {
        delete[] mCode;
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

void Filter_Internal::AppendElse()
{
    Indent_Dec();
    Append("}"    EOL);
    Append("else" EOL);
    Append("{"    EOL);
    Indent_Inc();
}

void Filter_Internal::VerifySize(unsigned int aSize_byte)
{
    if (mCodeMaxSize_byte < aSize_byte)
    {
        unsigned int lNewCodeMaxSize_byte = aSize_byte + 2048;

        char * lNewCode = new char[lNewCodeMaxSize_byte];
        assert(NULL != lNewCode);

        memset(lNewCode, 0, lNewCodeMaxSize_byte);

        if (0 < mCodeMaxSize_byte)
        {
            assert(NULL != mCode);

            memcpy(lNewCode, mCode, mCodeSize_byte);

            delete[] mCode;
        }

        mCode             = lNewCode;
        mCodeMaxSize_byte = lNewCodeMaxSize_byte;
    }
}
