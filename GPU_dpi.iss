
; Author     KMS - Martin Dubois, ing.
; Copyright  (C) 2019 KMS. All rights reserved.
; Product    GPU-dpi
; File       GPU_dpi.iss

; CODE REVIEW  2019-07-26  KMS - Martin Dubois, ing.

[Setup]
AllowNetworkDrive=no
AllowUNCPath=no
AppCopyright=Copyright (C) 2019 KMS.
AppName=GPU-dpi
AppPublisher=KMS
AppPublisherURL=http://www.kms-quebec.com
AppSupportURL=http://www.kms-quebec.com
AppVersion=1.0.2
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
DefaultDirName={pf}\GPU-dpi
LicenseFile=License.txt
MinVersion=10.0
OutputBaseFilename=GPU-dpi_1.0.2
OutputDir=Installer

[Files]
Source: "_DocUser\GPU-dpi.ReadMe.txt"                              ; DestDir: "{app}"; Flags: isreadme
Source: "GPU_dpi\_DocUser\GPU-dpi.GPU-dpi.ReadMe.txt"              ; DestDir: "{app}"
Source: "GPU_dpi_Extcap\_DocUser\GPU-dpi.GPU-dpi_Extcap.ReadMe.txt"; DestDir: "{app}"
Source: "GPU-dpi_Run\_DocUser\GPU-dpi.GPU-dpi_Run.ReadMe.txt"      ; DestDir: "{app}"
Source: "GPU_dpi_Setup\_DocUser\GPU-dpi.GPU-dpi_Setup.ReadMe.txt"  ; DestDir: "{app}"
Source: "GPU_dpi_TG\_DocUser\GPU-dpi.GPU-dpi_TG.ReadMe.txt"        ; DestDir: "{app}"
Source: "Import\Binaries\Release_64\OpenNet.dll"                   ; DestDir: "{app}"
Source: "Import\Binaries\Release_64\OpenNet_Tool.exe"              ; DestDir: "{app}"
Source: "Import\Drivers\Release_64\ONK_Hardware\onk_pro1000.cat"   ; DestDir: "{app}\Drivers\ONK_Hardware"
Source: "Import\Drivers\Release_64\ONK_Hardware\ONK_Pro1000.inf"   ; DestDir: "{app}\Drivers\ONK_Hardware"
Source: "Import\Drivers\Release_64\ONK_Hardware\ONK_Pro1000.sys"   ; DestDir: "{app}\Drivers\ONK_Hardware"
Source: "Import\Includes\OpenNetK\ARP.h"                           ; DestDir: "{app}\Includes\OpenNetK"
Source: "Import\Includes\OpenNetK\ByteOrder.h"                     ; DestDir: "{app}\Includes\OpenNetK"
Source: "Import\Includes\OpenNetK\Ethernet.h"                      ; DestDir: "{app}\Includes\OpenNetK"
Source: "Import\Includes\OpenNetK\IPv4.h"                          ; DestDir: "{app}\Includes\OpenNetK"
Source: "Import\Includes\OpenNetK\IPv6.h"                          ; DestDir: "{app}\Includes\OpenNetK"
Source: "Import\Includes\OpenNetK\Kernel.h"                        ; DestDir: "{app}\Includes\OpenNetK"
Source: "Import\Includes\OpenNetK\Kernel_OpenCL.h"                 ; DestDir: "{app}\Includes\OpenNetK"
Source: "Import\Includes\OpenNetK\RegEx.h"                         ; DestDir: "{app}\Includes\OpenNetK"
Source: "Import\Includes\OpenNetK\TCP.h"                           ; DestDir: "{app}\Includes\OpenNetK"
Source: "Import\Includes\OpenNetK\Types.h"                         ; DestDir: "{app}\Includes\OpenNetK"
Source: "Import\Includes\OpenNetK\UDP.h"                           ; DestDir: "{app}\Includes\OpenNetK"
Source: "Includes\GPU_dpi\GPU_dpi.h"                               ; DestDir: "{app}\Includes\GPU_dpi"
Source: "Includes\GPU_dpi\SetupTool.h"                             ; DestDir: "{app}\Includes\GPU_dpi"
Source: "Includes\GPU_dpi\Status.h"                                ; DestDir: "{app}\Includes\GPU_dpi"
Source: "Includes\GPU_dpi\System.h"                                ; DestDir: "{app}\Includes\GPU_dpi"
Source: "Includes\GPU_dpi\Types.h"                                 ; DestDir: "{app}\Includes\GPU_dpi"
Source: "Scripts\GPU_dpi_Extcap.cmd"                               ; DestDir: "{app}"
Source: "x64\Release\GPU_dpi.dll"                                  ; DestDir: "{app}"
Source: "x64\Release\GPU_dpi.lib"                                  ; DestDir: "{app}\Libraries"
Source: "x64\Release\GPU_dpi_Extcap.exe"                           ; DestDir: "{app}"
Source: "x64\Release\GPU-dpi_Run.exe"                              ; DestDir: "{app}"
Source: "x64\Release\GPU_dpi_Setup.exe"                            ; DestDir: "{app}"
Source: "x64\Release\GPU_dpi_TG.exe"                               ; DestDir: "{app}"

[Run]
Filename: "{app}\GPU_dpi_Setup.exe"; Parameters: "install"

[UninstallRun]
Filename: "{app}\GPU_dpi_Setup.exe"; Parameters: "uninstall"
