
// Product      GPU-dpi

/// \author     KMS - Martin Dubois, ing.
/// \copyright  Copyright &copy; 2019 KMS. All rights reserved.
/// \file       Includes/GPU_dpi/Status.h
/// \brief      GPU_dpi::Status (SDK)

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== C ==================================================================
#include <stdio.h>

// ===== Includes/OpenNet ===================================================
#include <GPU_dpi/GPU_dpi.h>

namespace GPU_dpi
{

    // Data type
    /////////////////////////////////////////////////////////////////////////

    /// \cond en
    /// \brief  The status codes
    /// \todo   Document values
    /// \endcond
    /// \cond fr
    /// \brief  Les code de status
    /// \todo   Documenter les valeurs
    /// \endcond
    typedef enum
    {
        STATUS_OK = 0,

        STATUS_FILE_ERROR                      ,
        STATUS_INVALID_ADAPTER_INDEX           ,
        STATUS_INVALID_BUTTON_INDEX            ,
        STATUS_INVALID_COMMAND_INDEX           ,
        STATUS_INVALID_CONFIGURATION           ,
        STATUS_INVALID_FILTER_CODE_SIZE        ,
        STATUS_INVALID_FILTER_FILE_NAME        ,
        STATUS_INVALID_FILTER_TYPE             ,
        STATUS_INVALID_FORWARD_ADAPTER         ,
        STATUS_INVALID_FORWARD_TYPE            , // 10
        STATUS_INVALID_INDEX                   ,
        STATUS_INVALID_OUTPUT_FORMAT           ,
        STATUS_INVALID_OUTPUT_PACKET_SIZE      ,
        STATUS_INVALID_OUTPUT_TYPE             ,
        STATUS_INVALID_OUTPUT_ADAPTER          ,
        STATUS_INVALID_OUTPUT_FILE_NAME        ,
        STATUS_INVALID_PAGE_INDEX              ,
        STATUS_INVALID_PROCESSOR_INDEX         ,
        STATUS_INVALID_REFERENCE               ,
        STATUS_IO_ERROR                        , // 20
        STATUS_NO_ACTIVE_ADAPTER               ,
        STATUS_NO_ADAPTER                      ,
        STATUS_NO_FILTER_DATA                  ,
        STATUS_NO_FILTER_FUNCTION_NAME         ,
        STATUS_NO_OUTPUT_FILE_NAME             ,
        STATUS_NOT_ADMINISTRATOR               ,
        STATUS_NOT_ENOUGH_BUFFER               ,
        STATUS_NOT_ALLOWED_NULL_ARGUMENT       ,
        STATUS_OPEN_CL_ERROR                   ,
        STATUS_REBOOT_REQUIRED                 , // 30
        STATUS_SYSTEM_NOT_STARTED              ,
        STATUS_UNEXPECTED_EXCEPTION_CODE       ,
        STATUS_UNEXPECTED_OPEN_NET_STATUS      ,
        STATUS_UNKNOWN_EXCEPTION               ,
        STATUS_UNSUPPORTED_OUTPUT_FORMAT       ,
        STATUS_USELESS_FILTER_CODE_OR_FILE_NAME,
        STATUS_USELESS_OUTPUT_FILE_NAME        ,

        STATUS_QTY
    }
    Status;

    // Function
    /////////////////////////////////////////////////////////////////////////

    /// \cond en
    /// \brief  This methode returns the description of an status code.
    /// \return The address of a constant
    /// \endcond
    /// \cond fr
    /// \brief  Cette m&eacute;thode retourne la descritption d'un code de
    ///         status.
    /// \return Un pointeur sur une constantes.
    /// \endcond
    extern GPU_DPI_PUBLIC const char * Status_GetDescription(Status aStatus);

    /// \cond en
    /// \brief  This methode returns the name of an status code.
    /// \return The address of a constant
    /// \endcond
    /// \cond fr
    /// \brief  Cette m&eacute;thode retourne le nom d'un code de status.
    /// \return Un pointeur sur une constantes.
    /// \endcond
    extern GPU_DPI_PUBLIC const char * Status_GetName(Status aStatus);

    /// \cond en
    /// \brief  Display
    /// \param  aStatus  The Status to display
    /// \param  aOut     The output stream
    /// \endcond
    /// \cond fr
    /// \brief  Afficher le Status
    /// \param  aStatus  Le Status &agarave; afficher
    /// \param  aOut     Le fichier de sortie
    /// \endcond
    /// \retval STATUS_OK
    /// \retval STATUS_NOT_ALLOWED_NULL_ARGUMENT
    extern GPU_DPI_PUBLIC Status Status_Display(Status aStatus, FILE * aOut);

}
