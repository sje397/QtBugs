; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Bugs"

; The file to write
OutFile "bugs.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\Bugs"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Bugs" "Install_Dir"

!define QTDIR	$%QTDIR%

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Bugs (required)"
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR  
  ; Put files there
  File "..\release\bugs.exe"
  File "${QTDIR}\bin\QtCore4.dll"
  File "${QTDIR}\bin\QtGui4.dll"
  ; mingw runtime
  File "${QTDIR}\bin\mingwm10.dll"
	
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\BUGS "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Bugs" "DisplayName" "Bugs"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Bugs" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Bugs" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Bugs" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
SectionEnd

; Optional section (can be disabled by the user)
Section "Desktop Shortcut"

;create desktop shortcut
  CreateShortCut "$DESKTOP\Bugs.lnk" "$INSTDIR\bugs.exe" ""

SectionEnd

Section "Start Menu Shortcuts"

; start menu shortcuts
  CreateDirectory "$SMPROGRAMS\Bugs"
  CreateShortCut "$SMPROGRAMS\Bugs\Bugs.lnk" "$INSTDIR\bugs.exe" "" "$INSTDIR\bugs.exe" 0
  CreateShortCut "$SMPROGRAMS\Bugs\Uninstall Bugs.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Bugs"
  DeleteRegKey HKLM SOFTWARE\BUGS

  ; Remove files and uninstaller
  Delete "$INSTDIR\*.*"

  ; Remove shortcuts, if any
  Delete "$DESKTOP\Bugs.lnk"
  Delete "$SMPROGRAMS\Bugs\Bugs.lnk"
  Delete "$SMPROGRAMS\Bugs\Uninstall Bugs.lnk"
  RMDir "$SMPROGRAMS\Bugs"

  ; Remove directories used
  RMDir "$INSTDIR"
  RMDir "$PROGRAMFILES\Bugs"
 
SectionEnd
