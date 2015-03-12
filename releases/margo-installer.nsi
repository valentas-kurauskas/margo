; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Margo"

; The file to write
OutFile "margo-0.76-standalone.exe"

; The default installation directory
InstallDir C:\Margo

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Margo" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

;-------------------------------
; Test if Visual Studio Redistributables 2008+ SP1 installed
; Returns -1 if there is no VC redistributables intstalled
Function CheckVCRedist
   Push $R0
   ClearErrors
   ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{9BE518E6-ECC6-35A9-88E4-87755C07200F}" "Version"



   ; if VS 2008+ redist SP1 not installed, install it
   IfErrors 0 VSRedistInstalled

   SetOutPath $INSTDIR\prerequisites
   MessageBox MB_YESNO "Margo requires Microsoft Visual C++ 2008 Service Pack 1 Redistributable Package MFC Security Update. Click yes to install automatically." /SD IDYES IDNO VSRedistInstalled
   File "prerequisites\vcredist_x86.exe"
   ExecWait "$INSTDIR\prerequisites\vcredist_x86.exe"

   StrCpy $R0 "-1"

VSRedistInstalled:
   Exch $R0
FunctionEnd


; The stuff to install

Section -Prerequisites
Call CheckVCRedist 
;  SetOutPath $INSTDIR\prerequisites
;  MessageBox MB_YESNO "Margo requires Microsoft Visual C++ 2008 Service Pack 1 Redistributable Package MFC Security ;Update.;Click yes to install this update automatically. Alternatively, install the redistributables manually from ;http://www.microsoft.com/en-us/download/details.aspx?id=26368" /SD IDYES IDNO endRedist
;    File "prerequisites\vcredist_x86.exe"
;    ExecWait "$INSTDIR\prerequisites\vcredist_x86.exe"
;    Goto endRedist
;  endRedist:
SectionEnd


Section "Margo (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File README.md
  File readme.txt
  FILE LICENSE
  File /r bin
;  File /r data
  File /r doc
  File /r gui-bin
;  File /r src
  File /r templates


  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Margo "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Margo" "DisplayName" "Margo"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Margo" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Margo" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Margo" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  SetOutPath "$INSTDIR\gui-bin\"
  CreateDirectory "$SMPROGRAMS\Margo"
  CreateShortCut "$SMPROGRAMS\Margo\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Margo\Margo GUI.lnk" "$INSTDIR\gui-bin\explore.exe" "" "$INSTDIR\gui-bin\explore.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Margo"
  DeleteRegKey HKLM SOFTWARE\Margo

  ; Remove files and uninstaller


  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Margo\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Margo"
  RMDir /r "$INSTDIR"

SectionEnd
