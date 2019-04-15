
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       CodeGen/Phase3.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== CodeGen ============================================================
class Filter_Internal;
class TokenList      ;

// Function
/////////////////////////////////////////////////////////////////////////////

extern void Phase3(const TokenList & aIn, Filter_Internal * aOut);
