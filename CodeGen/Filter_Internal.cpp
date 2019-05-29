
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

void Filter_Internal::SetErrorMessage(const char * aErrMsg)
{
    assert(NULL != aErrMsg);

    assert(NULL == mErrorMessage);

    if (NULL != mCode)
    {
        delete[] mCode;
        mCode             = NULL;
        mCodeMaxSize_byte =    0;
        mCodeSize_byte    =    0;
    }

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

void Filter_Internal::AppendCode(const char * aCode)
{
    assert(NULL != aCode);

    unsigned int lCodeLen = static_cast<unsigned int>(strlen(aCode));
    assert(0 < lCodeLen);

    VerifySize(mCodeSize_byte + lCodeLen + 1);

    memcpy(mCode + mCodeSize_byte, aCode, lCodeLen);
    mCodeSize_byte += lCodeLen;
}

void Filter_Internal::Begin()
{
    assert(NULL != mFunctionName);

    char lLine[1024];

    Append("#include <OpenNetK/ARP.h>"      EOL);
    Append("#include <OpenNetK/Ethernet.h>" EOL);
    Append("#include <OpenNetK/IPv4.h>"     EOL);
    Append("#include <OpenNetK/IPv6.h>"     EOL);
    Append("#include <OpenNetK/RegEx.h>"    EOL);
    Append("#include <OpenNetK/TCP.h>"      EOL);
    Append("#include <OpenNetK/UDP.h>"      EOL);
    Append(                                 EOL);

    // TODO  CodeGen.Filter_Internal
    //       Move these 2 function into OpenNetK/Utils.h

    Append(                                                  EOL);
    Append("unsigned short SwapUInt16( unsigned short aIn )" EOL);
    Append("{"                                               EOL);
    Indent_Inc();
        Append("return ( ( aIn >> 8 ) | ( aIn << 8 ) );" EOL);
    Indent_Dec();
    Append("}" EOL);

    Append(                                              EOL);
    Append("unsigned int SwapUInt32( unsigned int aIn )" EOL);
    Append("{"                                           EOL);
    Indent_Inc();
        Append("return ( ( aIn >> 24 ) | ( ( aIn >> 8 ) & 0x0000ff00 ) | ( ( aIn << 8 ) & 0x00ff0000 ) | ( aIn << 24 ) );" EOL);
    Indent_Dec();
    Append("}" EOL);
    Append(    EOL);

    sprintf_s(lLine, "OPEN_NET_FUNCTION_DECLARE( %s )" EOL, mFunctionName);
    Append(lLine);

    Append("{" EOL);
    Indent_Inc();

	Append(    "OPEN_NET_FUNCTION_BEGIN" EOL);
    Indent_Inc();
    Append(                                                                                                   EOL);
    Append(        "OPEN_NET_GLOBAL unsigned char * lEthernet         = Ethernet_Data( lBase, lPacketInfo );" EOL);
    Append(        "unsigned short                  lEthernet_Type_nh = Ethernet_Type( lBase, lPacketInfo );" EOL);
    Append(                                                                                                   EOL);
    Append(        "OPEN_NET_GLOBAL unsigned char * lIP;"                                                     EOL);
    Append(        "unsigned int                    lIP_DataSize_byte;"                                       EOL);
    Append(        "unsigned char                   lIP_Protocol;"                                            EOL);
    Append(                                                                                                   EOL);
    C_switch("lEthernet_Type_nh");
		C_case("IPv4_ETHERNET_TYPE_nh");
			Append(    "lIP               = IPv4_Data    ( lEthernet );" EOL);
            Append(    "lIP_DataSize_byte = IPv4_DataSize( lEthernet );" EOL);
			Append(    "lIP_Protocol      = IPv4_Protocol( lEthernet );" EOL);
		C_break();
		C_case("IPv6_ETHERNET_TYPE_nh");
			Append(    "lIP               = IPv6_Data    ( lEthernet );" EOL);
            Append(    "lIP_DataSize_byte = IPv6_DataSize( lEthernet );" EOL);
			Append(    "lIP_Protocol      = IPv6_Protocol( lEthernet );" EOL);
		C_break();
		Append(    "default : lIP = 0;" EOL);
    C_switch_End();
    Append(                             EOL);
    Append(        "int lR;"            EOL);
}

void Filter_Internal::End()
{
    switch (mOutputType)
    {
    case GPU_dpi::OUTPUT_TYPE_DIRECT :
        C_if("lR");

        switch (mForwardType)
        {
        case GPU_dpi::FORWARD_TYPE_ALWAYS :
            Append(        "lPacketInfo->mSendTo = ( 1 << OUTPUT_ADAPTER ) | ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            C_else();
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_FILTERED :
            Append(        "lPacketInfo->mSendTo = ( 1 << OUTPUT_ADAPTER ) | ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            C_else();
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_NEVER :
            Append(        "lPacketInfo->mSendTo = ( 1 << OUTPUT_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            C_else();
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_NOT_FILTERED :
            Append(        "lPacketInfo->mSendTo = ( 1 << OUTPUT_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            C_else();
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        default: assert(false);
        }

        C_if_End();
        break;

    case GPU_dpi::OUTPUT_TYPE_CALLBACK:
    case GPU_dpi::OUTPUT_TYPE_FILE    :
        C_if("lR");

        switch (mForwardType)
        {
        case GPU_dpi::FORWARD_TYPE_ALWAYS :
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_OUTPUT ) | OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;" EOL);
            Append(        "lEvents |= OPEN_NET_BUFFER_EVENT;"                                                                   EOL);
            C_else();
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_OUTPUT ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_FILTERED :
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_OUTPUT ) | OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;" EOL);
            Append(        "lEvents |= OPEN_NET_BUFFER_EVENT;"                                                                   EOL);
            C_else();
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_NEVER :
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;" EOL);
            Append(        "lEvents |= OPEN_NET_BUFFER_EVENT;"                                         EOL);
            C_else();
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_NOT_FILTERED :
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;" EOL);
            Append(        "lEvents |= OPEN_NET_BUFFER_EVENT;"                                         EOL);
            C_else();
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        default: assert(false);
        }

        C_if_End();
        break;

    case GPU_dpi::OUTPUT_TYPE_NONE :
        switch (mForwardType)
        {
        case GPU_dpi::FORWARD_TYPE_ALWAYS  :
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            break;

        case GPU_dpi::FORWARD_TYPE_FILTERED :
            C_if("lR");
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            C_else();
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            C_if_End();
            break;

        case GPU_dpi::FORWARD_TYPE_NOT_FILTERED :
            C_if("lR");
            Append(        "lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;" EOL);
            C_else();
            Append(        "lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;" EOL);
            C_if_End();
            break;

        default: assert(false);
        }
    }

    Indent_Dec();
    Append(                            EOL);
    Append(    "OPEN_NET_FUNCTION_END" EOL);
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

// ===== C ==================================================================

void Filter_Internal::C_break()
{
    Append("break;" EOL);
    Indent_Dec();
}

void Filter_Internal::C_case(const char * aValue)
{
    assert(NULL != aValue);

    char lLine[1024];

    sprintf_s(lLine, "case %s :" EOL, aValue);
    Append(lLine);

    Indent_Inc();
}

void Filter_Internal::C_else()
{
    Indent_Dec();
    Append("}"    EOL);
    Append("else" EOL);
    Append("{"    EOL);
    Indent_Inc();
}

void Filter_Internal::C_for_End()
{
    Indent_Dec();
    Append("}" EOL);
}

void Filter_Internal::C_if(const char * aCondition)
{
    assert(NULL != aCondition);

    char lLine[1024];

    sprintf_s(lLine, "if ( %s )" EOL, aCondition);
    Append(lLine);

    Append(          "{" EOL);
    Indent_Inc();
}

void Filter_Internal::C_if_End()
{
    Indent_Dec();
    Append("}" EOL);
}

void Filter_Internal::C_switch(const char * aValue)
{
    assert(NULL != aValue);

    char lLine[1024];

    sprintf_s(lLine, "switch( %s )" EOL, aValue);
    Append(lLine);

    Append(          "{" EOL);
}

void Filter_Internal::C_switch_End()
{
    Append("}" EOL);
}

// ===== Filter =============================================================

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
