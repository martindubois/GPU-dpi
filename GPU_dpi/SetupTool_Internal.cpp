
// Author     KMS - Martin Dubois, ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/SetupTool_Internal.cpp

#define __CLASS__ "SetupTool_Internal::"

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== C ==================================================================
#include <memory.h>

#ifdef _KMS_WINDOWS_

    #include <io.h>

    // ===== Windows ========================================================
    #include <Windows.h>

#endif

// ===== Import/Includes ====================================================
#include <KmsLib/Exception.h>

// ===== GPU_dpi ============================================================
#include "Utils.h"

#include "SetupTool_Internal.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#ifdef _KMS_LINUX_
    #define EXT_CAP_FILE      "GPU-dpi_Extcap.sh"
    #define WIRESHARK_INSTALL "/usr/lib/x86_64-linux-gnu/wireshark"
#endif

#ifdef _KMS_WINDOWS_
    #define EXT_CAP_FILE      "GPU_dpi_Extcap.cmd"
    #define WIRESHARK_INSTALL "C:\\Program Files\\Wireshark"
#endif

// Static function declaration
/////////////////////////////////////////////////////////////////////////////

static bool IsWiresharkInstalled();

// Public
/////////////////////////////////////////////////////////////////////////////

SetupTool_Internal::SetupTool_Internal(bool aDebug) : mExtCap(false), mOpenNet(OpenNet::SetupTool::Create(aDebug)), mWireshark(IsWiresharkInstalled())
{
    if (NULL == mOpenNet)
    {
        throw new KmsLib::Exception(KmsLib::Exception::CODE_ERROR,
            "OpenNet::SetupTool::Create(  ) failed", NULL, __FILE__, __CLASS__ "SetupTool_Internal", __LINE__, aDebug);
    }

    if (mWireshark)
    {
        sprintf_s(mExtCap_Dst, WIRESHARK_INSTALL SLASH "extcap" SLASH EXT_CAP_FILE);

        if (mOpenNet->IsDebugEnabled()) { printf("! %s\n", mExtCap_Dst); }

        mExtCap = ExtCap_IsInstalled();
    }
}

// ===== OpenNet::SetupTool =================================================

const char * SetupTool_Internal::GetBinaryFolder() const
{
    assert(NULL != mOpenNet);
    
    return mOpenNet->GetBinaryFolder();
}

const char * SetupTool_Internal::GetIncludeFolder() const
{
    assert(NULL != mOpenNet);

    return mOpenNet->GetIncludeFolder();
}

const char * SetupTool_Internal::GetInstallFolder() const
{
    assert(NULL != mOpenNet);

    return mOpenNet->GetInstallFolder();
}

const char * SetupTool_Internal::GetLibraryFolder() const
{
    assert(NULL != mOpenNet);

    return mOpenNet->GetLibraryFolder();
}

bool SetupTool_Internal::IsDebugEnabled() const
{
    assert(NULL != mOpenNet);

    return mOpenNet->IsDebugEnabled();
}

GPU_dpi::Status SetupTool_Internal::Install()
{
    assert(NULL != mOpenNet);

    OpenNet::Status lStatus = mOpenNet->Install();

    return Utl_ConvertOpenNetStatus(lStatus);
}

GPU_dpi::Status SetupTool_Internal::Uninstall()
{
    assert(NULL != mOpenNet);

    GPU_dpi::Status lResult = GPU_dpi::STATUS_OK;

    if (mExtCap)
    {
        lResult = ExtCap_Uninstall();
    }

    OpenNet::Status lStatus = mOpenNet->Uninstall();
    switch (lStatus)
    {
    case OpenNet::STATUS_OK: break;

    case OpenNet::STATUS_REBOOT_REQUIRED:
        if (GPU_dpi::STATUS_OK == lResult)
        {
            lResult = GPU_dpi::STATUS_REBOOT_REQUIRED;
        }
        break;

    default: lResult = Utl_ConvertOpenNetStatus(lStatus);
    }

    return lResult;
}

GPU_dpi::Status SetupTool_Internal::Interactif_ExecuteCommand(unsigned int aCommand)
{
    assert(NULL != mOpenNet);

    unsigned int lCount = mOpenNet->Interactif_GetCommandCount();
    if (lCount > aCommand)
    {
        OpenNet::Status lStatus = mOpenNet->Interactif_ExecuteCommand(aCommand);

        return Utl_ConvertOpenNetStatus(lStatus);
    }

    unsigned int lCommand = aCommand - lCount;

    if ( mWireshark)
    {
        if (0 == lCommand)
        {
            return ExtCap_Change();
        }

        lCommand--;
    }

    return GPU_dpi::STATUS_INVALID_COMMAND_INDEX;
}

unsigned int SetupTool_Internal::Interactif_GetCommandCount()
{
    assert(NULL != mOpenNet);

    unsigned int lResult = mOpenNet->Interactif_GetCommandCount();

    if (mWireshark)
    {
        lResult++;
    }

    return lResult;
}

const char * SetupTool_Internal::Interactif_GetCommandText(unsigned int aCommand)
{
    assert(NULL != mOpenNet);

    unsigned int lCount = mOpenNet->Interactif_GetCommandCount();
    if (lCount > aCommand)
    {
        return mOpenNet->Interactif_GetCommandText(aCommand);
    }

    unsigned int lCommand = aCommand - lCount;

    if (mWireshark)
    {
        if (0 == lCommand)
        {
            if (mExtCap)
            {
                return "Uninstall the Wireshark integration";
            }

            return "Install the wireshark integration";
        }

        lCommand--;
    }

    return NULL;
}

GPU_dpi::Status SetupTool_Internal::Wizard_ExecutePage(unsigned int * aPage, unsigned int aButton)
{
    assert(NULL != aPage);

    assert(mOpenNet);

    unsigned int lCount = mOpenNet->Wizard_GetPageCount();
    unsigned int lPage  = (*aPage);

    if (lCount > lPage)
    {
        OpenNet::Status lStatus = mOpenNet->Wizard_ExecutePage(aPage, aButton);

        return Utl_ConvertOpenNetStatus(lStatus);
    }

    lPage -= lCount;

    if (mWireshark)
    {
        if (0 == lPage)
        {
            GPU_dpi::Status lStatus;

            switch (aButton)
            {
            case 0: lStatus = ExtCap_Change(); break;
            case 1: lStatus = GPU_dpi::STATUS_OK; break;

            default: return GPU_dpi::STATUS_INVALID_BUTTON_INDEX;
            }

            (*aPage)++;

            return lStatus;
        }

        lPage--;
    }

    return GPU_dpi::STATUS_INVALID_PAGE_INDEX;
}

unsigned int SetupTool_Internal::Wizard_GetPageButtonCount(unsigned int aPage)
{
    assert(mOpenNet);

    unsigned int lCount = mOpenNet->Wizard_GetPageCount();
    if (lCount > aPage)
    {
        return mOpenNet->Wizard_GetPageButtonCount(aPage);
    }

    unsigned int lPage = aPage - lCount;

    if (mWireshark)
    {
        if (0 == lPage)
        {
            return 2;
        }

        lPage--;
    }

    return 0;
}

const char * SetupTool_Internal::Wizard_GetPageButtonText(unsigned int aPage, unsigned int aButton)
{
    assert(mOpenNet);

    unsigned int lCount = mOpenNet->Wizard_GetPageCount();
    if (lCount > aPage)
    {
        return mOpenNet->Wizard_GetPageButtonText(aPage, aButton);
    }

    unsigned int lPage = aPage - lCount;

    if (mWireshark)
    {
        if (0 == lPage)
        {
            switch (aButton)
            {
            case 0 :
                if (mExtCap)
                {
                    return "Uninstall";
                }

                return "Install";

            case 1: return "Skip";

            default: return NULL;
            }
        }

        lPage--;
    }

    return NULL;
}

unsigned int SetupTool_Internal::Wizard_GetPageCount()
{
    assert(mOpenNet);

    unsigned int lResult = mOpenNet->Wizard_GetPageCount();

    if (mWireshark)
    {
        lResult++;
    }

    return lResult;
}

const char * SetupTool_Internal::Wizard_GetPageText(unsigned int aPage)
{
    assert(mOpenNet);

    unsigned int lCount = mOpenNet->Wizard_GetPageCount();
    if (lCount > aPage)
    {
        return mOpenNet->Wizard_GetPageText(aPage);
    }

    unsigned int lPage = aPage - lCount;

    if (mWireshark)
    {
        if (0 == lPage)
        {
            if (mExtCap)
            {
                return "The Wireshark integration is installed.\n"
                       "\n"
                       "You can capture network packet using GPU-dpi directly\n"
                       "from the Wireshark interface.\n"
                       "\n"
                       "Do you want to uninstall the Wireshark integration?";
            }

            return "The wireshark integration is not installed.\n"
                   "\n"
                   "Installing it will allow you to use GPU-dpi for capturing\n"
                   "network packet directly from the Wireshark interface."
                   "\n"
                   "Do not forget to restart Wireshark after installing the\n"
                   "integration.\n"
                   "\n"
                   "Do you want to install the Wireshark integration?";
        }

        lPage--;
    }

    return NULL;
}

const char * SetupTool_Internal::Wizard_GetPageTitle(unsigned int aPage)
{
    assert(mOpenNet);

    unsigned int lCount = mOpenNet->Wizard_GetPageCount();
    if (lCount > aPage)
    {
        return mOpenNet->Wizard_GetPageTitle(aPage);
    }

    unsigned int lPage = aPage - lCount;

    if (mWireshark)
    {
        if (0 == lPage)
        {
            if (mExtCap)
            {
                return "Uninstall Wireshark integration";
            }

            return "Install Wireshark integration";
        }

        lPage--;
    }

    return NULL;
}

// Protected
/////////////////////////////////////////////////////////////////////////////

// ===== OpenNet::SetupTool =================================================

SetupTool_Internal::~SetupTool_Internal()
{
    assert(NULL != mOpenNet);

    mOpenNet->Delete();
}

// Private
/////////////////////////////////////////////////////////////////////////////

// ===== ExtCap =============================================================

GPU_dpi::Status SetupTool_Internal::ExtCap_Change()
{
    GPU_dpi::Status lResult;

    if (mExtCap)
    {
        lResult = ExtCap_Uninstall();
    }
    else
    {
        lResult = ExtCap_Install();
    }

    if (GPU_dpi::STATUS_OK == lResult)
    {
        mExtCap = !mExtCap;
    }

    return lResult;
}

GPU_dpi::Status SetupTool_Internal::ExtCap_Install()
{
    char lSrc[1024];

    sprintf_s(lSrc, "%s" SLASH EXT_CAP_FILE, mOpenNet->GetInstallFolder());

    if (mOpenNet->IsDebugEnabled()) { printf("! %s ==> %s\n", lSrc, mExtCap_Dst); }

    #ifdef _KMS_LINUX_
    #endif

    #ifdef _KMS_WINDOWS_
        if (!CopyFile(lSrc, mExtCap_Dst, FALSE))
        {
            fprintf(stderr, "ERROR  CopyFile( \"%s\", \"%s\",  )  failed - 0x%08x\n", lSrc, mExtCap_Dst, GetLastError());
            return GPU_dpi::STATUS_FILE_ERROR;
        }
    #endif

    return GPU_dpi::STATUS_OK;
}

bool SetupTool_Internal::ExtCap_IsInstalled()
{
    return (0 == _access(mExtCap_Dst, 06));
}

GPU_dpi::Status SetupTool_Internal::ExtCap_Uninstall()
{
    if (mOpenNet->IsDebugEnabled()) { printf("! %s\n", mExtCap_Dst); }

    int lRet = _unlink(mExtCap_Dst);
    if (0 != lRet)
    {
        fprintf(stderr, "ERROR  _unlink( \"%s\" )  failed - %d\n", mExtCap_Dst, lRet);
        return GPU_dpi::STATUS_FILE_ERROR;
    }

    return GPU_dpi::STATUS_OK;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

bool IsWiresharkInstalled()
{
    return (0 == _access(WIRESHARK_INSTALL, 06));
}
