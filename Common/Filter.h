
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       Common/Filter.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Includes ===========================================================
#include <GPU_dpi/Types.h>

// Class
/////////////////////////////////////////////////////////////////////////////

class Filter
{

public:

    static Filter * Create(GPU_dpi::OutputType aOutputType, GPU_dpi::ForwardType aForwardType, const char * aIn);

    virtual void Delete();

    virtual const char * GetCode        () const = 0;
    virtual unsigned int GetCodeSize    () const = 0;
    virtual const char * GetErrorMessage() const = 0;
    virtual const char * GetFunctionName() const = 0;

protected:

    Filter();

    virtual ~Filter();

private:

    Filter(const Filter &);

    const Filter & operator = (const Filter &);

};
