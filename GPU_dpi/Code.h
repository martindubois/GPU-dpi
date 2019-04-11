
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/Code.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Includes ===========================================================
#include <GPU_dpi/Types.h>

// Data types
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    CODE_CONSTANT ,
    CODE_GENERATED,
    CODE_INVALID  ,
    CODE_USER     ,
}
Code_Type;

typedef struct
{
    Code_Type mType;

    const char * mCode                 ;
    unsigned int mCodeSize_byte        ;
    const char * mFunctionName         ;
    bool         mOutput               ;
    bool         mReplaceForwardAdapter;
    bool         mReplaceOutputAdapter ;
}
Code_Info;

// Constants
/////////////////////////////////////////////////////////////////////////////

extern const Code_Info CODES[GPU_dpi::FILTER_TYPE_QTY][GPU_dpi::FORWARD_TYPE_QTY][GPU_dpi::OUTPUT_TYPE_QTY];
