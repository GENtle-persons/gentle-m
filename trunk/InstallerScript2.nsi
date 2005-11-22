; GENtle.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install GENtle.exe into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "GENtle"

; The file to write
OutFile "GENtleSetup.exe"

;Section
;SectionEnd

; The default installation directory
InstallDir $PROGRAMFILES\GENtle

SetCompressor "lzma"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\GENtle" "Install_Dir"

;--------------------------------

LicenseData "gpl.txt"
DirText "Choose a directory to install in to:"
LicenseText "GENtle is released under the General Public License (GPL)"


; Pages

Page components
Page license
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "GENtle (required)"

  SectionIn RO



  MessageBox MB_YESNO "Install or update GENtle?" IDYES yeslabel IDNO nolabel
  yeslabel:
;  MessageBox MB_OK "You pressed yes"
  Goto done
  nolabel:
;  MessageBox MB_OK "You pressed no"
  Quit ;
  done:


  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "GENtle.exe"
;  File "blastcl3.exe"
;  File "clustalw.exe"
;  File "sqlite.exe"
  File "sqlite.dll"
  File "sqlite3.dll"
  File "libmySQL.dll"
  File "commonvectors.db"
  File "codon_catalog.csv"
  File "default.tab"
  File "GENtle.ico"
  File "variables.csv"
  File "blank.db"
  File "gpl.txt"
  File "MySQL template.txt"

  SetOutPath "$INSTDIR\wav"
  File "wav\*.wav"
  File "wav\*.txt"

  SetOutPath "$INSTDIR\help"
  File "help\*.htm"
  File "help\*.png"
  File "help\*.css"
  
  SetOutPath "$INSTDIR\bitmaps"
  File "bitmaps\*.bmp"
  
  SetOutPath "$INSTDIR"

;create desktop shortcut
  CreateShortCut "$DESKTOP\GENtle.lnk" "$INSTDIR\GENtle.exe" ""
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\GENtle "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GENtle" "DisplayName" "GENtle"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GENtle" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GENtle" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GENtle" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\GENtle"
  CreateShortCut "$SMPROGRAMS\GENtle\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\GENtle\GENtle.lnk" "$INSTDIR\GENtle.exe" "" "$INSTDIR\GENtle.exe" 0
  
  Exec "$INSTDIR\GENtle.exe"
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GENtle"
  DeleteRegKey HKLM SOFTWARE\NSIS_GENtle

  ; Remove files and uninstaller
;  Delete $INSTDIR\GENtle.exe
;  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\help\*.*
  Delete $INSTDIR\bitmaps\*.*
  RmDir $INSTDIR\help
  RmDir $INSTDIR\bitmaps
  Delete $INSTDIR\*.*
  RmDir $INSTDIR

  Delete "$DESKTOP\GENtle.lnk"
  Delete "$SMPROGRAMS\GENtle\*.*"
  RmDir  "$SMPROGRAMS\GENtle"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\GENtle\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\GENtle"

;Delete Files 
  RMDir /r "$INSTDIR\*.*"    
 
;Remove the installation directory
  RMDir "$INSTDIR"


SectionEnd