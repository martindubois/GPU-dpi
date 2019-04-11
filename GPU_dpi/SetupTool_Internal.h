
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/SetupTool_Internal.h

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Import/Includes ====================================================
#include <OpenNet/SetupTool.h>

// ===== Includes ===========================================================
#include <GPU_dpi/SetupTool.h>

// Class
/////////////////////////////////////////////////////////////////////////////

class SetupTool_Internal : public GPU_dpi::SetupTool
{

public:

    SetupTool_Internal(bool aDebug);

    // ===== OpenNet::SetupTool =============================================

    virtual const char * GetBinaryFolder () const;
    virtual const char * GetIncludeFolder() const;
    virtual const char * GetInstallFolder() const;
    virtual const char * GetLibraryFolder() const;

    virtual bool IsDebugEnabled() const;

    virtual GPU_dpi::Status Install  ();
    virtual GPU_dpi::Status Uninstall();

    virtual GPU_dpi::Status Interactif_ExecuteCommand (unsigned int aCommand);
    virtual unsigned int    Interactif_GetCommandCount();
    virtual const char    * Interactif_GetCommandText (unsigned int aCommand);

    virtual GPU_dpi::Status Wizard_ExecutePage       (unsigned int * aPage, unsigned int aButton);
    virtual unsigned int    Wizard_GetPageButtonCount(unsigned int   aPage);
    virtual const char    * Wizard_GetPageButtonText (unsigned int   aPage, unsigned int aButton);
    virtual unsigned int    Wizard_GetPageCount      ();
    virtual const char    * Wizard_GetPageText       (unsigned int   aPage);
    virtual const char    * Wizard_GetPageTitle      (unsigned int   aPage);

protected:

    // ===== OpenNet::SetupTool =============================================
    virtual ~SetupTool_Internal();

private:

    // ===== ExtCap =========================================================
    GPU_dpi::Status ExtCap_Change     ();
    GPU_dpi::Status ExtCap_Install    ();
    bool            ExtCap_IsInstalled();
    GPU_dpi::Status ExtCap_Uninstall  ();

    bool mExtCap;
    char mExtCap_Dst[1024];

    bool mWireshark;

    OpenNet::SetupTool * mOpenNet;

};
