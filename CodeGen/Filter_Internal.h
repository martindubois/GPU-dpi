
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Filter_Internal.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Common =============================================================
#include "../Common/Filter.h"

// Class
/////////////////////////////////////////////////////////////////////////////

class Filter_Internal : public Filter
{

public:

    Filter_Internal(GPU_dpi::OutputType aOutputType, GPU_dpi::ForwardType aForwardType, const char * aFunctionName);

    void SetErrorMessage(const char * aErrMsg);

    void Append     (const char * aLine);
    void AppendCode (const char * aCode);
    void Begin      ();
    void End        ();

    void Indent_Inc();
    void Indent_Dec();

    // ===== C ==============================================================
    void C_break     ();
    void C_case      (const char * aValue    );
    void C_else      ();
    void C_for_End   ();
    void C_if        (const char * aCondition);
    void C_if_End    ();
    void C_switch    (const char * aValue    );
    void C_switch_End();

    // ===== Filter =========================================================
    virtual const char * GetCode        () const;
    virtual unsigned int GetCodeSize    () const;
    virtual const char * GetErrorMessage() const;
    virtual const char * GetFunctionName() const;

protected:

    // ===== Filter =========================================================
    virtual ~Filter_Internal();

private:

    void VerifySize(unsigned int aSize_byte);

    char       * mCode            ;
    unsigned int mCodeMaxSize_byte;
    unsigned int mCodeSize_byte   ;

    const char * mErrorMessage;
    const char * mFunctionName;

    GPU_dpi::ForwardType mForwardType;
    GPU_dpi::OutputType  mOutputType ;

    unsigned int mIndent;

};
