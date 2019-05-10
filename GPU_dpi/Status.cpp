
// Author     KMS - Martin Dubois. ing.
// Copyright  (C) 2019 KMS. All rights reserved.
// Product    GPU-dpi
// File       GPU_dpi/Status.cpp

// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Component.h"

// ===== Includes ===========================================================
#include <GPU_dpi/Status.h>

// Data type
/////////////////////////////////////////////////////////////////////////////

typedef struct
{
    const char * mName;
    const char * mDescription;
}
StatusInfo;

// Constants
/////////////////////////////////////////////////////////////////////////////

static const StatusInfo STATUS_INFO[GPU_dpi::STATUS_QTY] =
{
    { "OK", "Success" },

    { "ASSERT_FAILED"                   , "Assertion failed"                 },
    { "CANNOT_OPEN_CONFIG_FILE"         , "Cannot open configuration file"   },
    { "CODE_TOO_LONG"                   , "The code is too long"             },
    { "FILE_ERROR"                      , "File error"                       },
    { "INVALID_ADAPTER_INDEX"           , "Invalid adapter index"            },
    { "INVALID_BINARY_SEARCH_STRING"    , "Invalid binary search string"     },
    { "INVALID_BUTTON_INDEX"            , "Invalid button index"             },
    { "INVALID_COMMAND_INDEX"           , "Invalid command index"            },
    { "INVALID_CONFIG_FILE"             , "Invalid configuration file"       },
    { "INVALID_CONFIGURATION"           , "Invalid configuration"            }, // 10 
    { "INVALID_FILTER"                  , "Invalid filter"                   },
    { "INVALID_FILTER_CODE_SIZE"        , "Invalid filter code size"         },
    { "INVALID_FILTER_FILE_NAME"        , "Invalid filter file name"         },
    { "INVALID_FILTER_TYPE"             , "Invalid filter type"              },
    { "INVALID_FORWARD_ADAPTER"         , "Invalid forward adapter"          },
    { "INVALID_FORWARD_TYPE"            , "Invalid forward type"             },
    { "INVALID_INDEX"                   , "Invalid index"                    },
    { "INVALID_OUTPUT_FORMAT"           , "Invalid output format"            },
    { "INVALID_OUTPUT_PACKET_SIZE"      , "Invalid output packet size"       },
    { "INVALID_OUTPUT_TYPE"             , "Invalid output type"              }, // 20
    { "INVALID_OUTPUT_ADAPTER"          , "Invalid output adapter"           }, 
    { "INVALID_OUTPUT_FILE_NAME"        , "Invalid output file name"         },
    { "INVALID_PAGE_INDEX"              , "Invalid page index"               },
    { "INVALID_PROCESSOR_INDEX"         , "Invalid proessor index"           },
    { "INVALID_REFERENCE"               , "Invalid reference"                },
    { "INVALID_SEARCH_ELEMENT"          , "Invalid search element"           },
    { "INVALID_SEARCH_ELEMENT_TYPE"     , "Invalid search element type"      },
    { "INVALID_STATUS"                  , "Invalid status"                   }, 
    { "IO_ERROR"                        , "IO error"                         }, 
    { "NO_ACTIVE_ADAPTER"               , "No active adapter"                }, // 30 
    { "NO_ADAPTER"                      , "No adapter"                       }, 
    { "NO_FILTER_DATA"                  , "No filter data"                   }, 
    { "NO_FILTER_FUNCTION_NAME"         , "No filter function name"          }, 
    { "NO_OUTPUT_CALLBACK"              , "No output callback"               },
    { "NO_OUTPUT_FILE_NAME"             , "No output file name"              }, 
    { "NOT_ADMINISTRATOR"               , "Not run by an adaministrator"     }, 
    { "NOT_ENOUGH_BUFFER"               , "Not enough buffer"                }, 
    { "NOT_ALLOWED_NULL_ARGUMENT"       , "Not allowed NULL argument"        }, 
    { "OPEN_CL_ERROR"                   , "OpenCL error"                     }, 
    { "REBOOT_REQUIRED"                 , "A computer reboot required"       }, // 40 
    { "SYSTEM_NOT_STARTED"              , "The system is not started"        }, 
    { "SYSTEM_RUNNING"                  , "The system is running"            }, 
    { "SYSTEM_STARTING"                 , "The system is starting"           }, 
    { "SYSTEM_STOPPED"                  , "The system is stopped"            },
    { "SYSTEM_STOPPING"                 , "The system is stopping"           }, 
    { "UNEXPECTED_EXCEPTION_CODE"       , "Unexpected exception code"        }, 
    { "UNEXPECTED_OPEN_NET_STATUS"      , "Unexpected OpenNet status"        }, 
    { "UNKNOWN_EXCEPTION"               , "Unknown exception"                },
    { "UNSUPPORTED_OUTPUT_FORMAT"       , "Unsupported output format"        }, 
    { "USELESS_FILTER_CODE_OR_FILE_NAME", "Useless filter code or file name" }, // 50 
    { "USELESS_OUTPUT_CALLBACK"         , "Useless output callback"          }, 
    { "USELESS_OUTPUT_CALLBACK_CONTEXT" , "Useless output callback context"  }, 
    { "USELESS_OUTPUT_FILE_NAME"        , "Useless output file name"         }, // 53
};

namespace GPU_dpi
{

    // Functions
    /////////////////////////////////////////////////////////////////////////

    const char * Status_GetDescription(Status aStatus)
    {
        if (STATUS_QTY <= aStatus)
        {
            return NULL;
        }

        return STATUS_INFO[aStatus].mDescription;
    }

    const char * Status_GetName(Status aStatus)
    {
        if (STATUS_QTY <= aStatus)
        {
            return NULL;
        }

        return STATUS_INFO[aStatus].mName;
    }

    Status Status_Display(Status aStatus, FILE * aOut)
    {
        if (NULL == aOut)
        {
            return STATUS_NOT_ALLOWED_NULL_ARGUMENT;
        }

        if (STATUS_QTY <= aStatus)
        {
            fprintf_s(aOut, "%u - Invalid status code\n", aStatus);
            return STATUS_INVALID_STATUS;
        }

        const StatusInfo & lSI = STATUS_INFO[aStatus];

        fprintf_s(aOut, "%u = %s - %s\n", aStatus, lSI.mName, lSI.mDescription);

        return STATUS_OK;
    }

}