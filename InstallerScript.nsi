; Turn off old selected section
; 10 02 2003: Tom van Gemert
; Template voor het genereren van een installer.
; speciaal voor het genereren van EasyPlayer installers.
; Trimedia Interactive Projects


; -------------------------------
; Start


  !define MUI_PRODUCT "GENtle"
  !define MUI_FILE "GENtle"
  !define MUI_VERSION "0.1beta"
  !define MUI_BRANDINGTEXT "GENtle 0.1beta"
  CRCCheck On

  ; Bij deze moeten we waarschijnlijk een absoluut pad gaan gebruiken
  ; dit moet effe uitgetest worden.
  !include "${NSISDIR}\Contrib\Modern UI\System.nsh"

;--------------------------------
;General

  OutFile "GENtleSetup.exe"
  ShowInstDetails "nevershow"
  ShowUninstDetails "nevershow"
  SetCompressor "bzip2"
 
  !define MUI_ICON "icon.ico"
  !define MUI_UNICON "icon.ico"
  !define MUI_SPECIALBITMAP "Bitmap.bmp"


;--------------------------------
;Folder selection page

  InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"


;--------------------------------
;Modern UI Configuration

  !define MUI_WELCOMEPAGE  
  !define MUI_LICENSEPAGE
  !define MUI_DIRECTORYPAGE
  !define MUI_ABORTWARNING
  !define MUI_UNINSTALLER
  !define MUI_UNCONFIRMPAGE
  !define MUI_FINISHPAGE  


  

;--------------------------------
;Language

  ;!insertmacro MUI_LANGUAGE "English"


;-------------------------------- 
;Modern UI System

  
;--------------------------------
;Data

  LicenseData "gpl.txt"
  DirText "Choose a directory to install in to:"
  LicenseText "GENtle is released under the General Public License (GPL)"

;-------------------------------- 
;Installer Sections     
Section "install" Installation info

;Add files
  SetOutPath "$INSTDIR"
  
  File "${MUI_FILE}.exe"
  File "blastcl3.exe"
  File "clustalw.exe"
;  File "sqlite.exe"
  File "sqlite.dll"
  File "GENtle.ico"
  File "variables.csv"
  File "blank.db"
  File "gpl.txt"


  SetOutPath "$INSTDIR\help"
  File "help\*.htm"
  File "help\*.png"
  File "help\*.css"
  SetOutPath "$INSTDIR\bitmaps"
  File "bitmaps\*.bmp"
  SetOutPath "$INSTDIR"
  
;create desktop shortcut
  CreateShortCut "$DESKTOP\${MUI_PRODUCT}.lnk" "$INSTDIR\${MUI_FILE}.exe" ""
 
;create start-menu items
  CreateDirectory "$SMPROGRAMS\${MUI_PRODUCT}"
  CreateShortCut "$SMPROGRAMS\${MUI_PRODUCT}\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\${MUI_PRODUCT}\${MUI_PRODUCT}.lnk" "$INSTDIR\${MUI_FILE}.exe" "" "$INSTDIR\${MUI_FILE}.exe" 0
 
;write uninstall information to the registry
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayName" "${MUI_PRODUCT} (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "UninstallString" "$INSTDIR\Uninstall.exe"
 
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd


;--------------------------------    
;Uninstaller Section  
Section "Uninstall"
   
;Delete Files 
  RMDir /r "$INSTDIR\*.*"    
 
;Remove the installation directory
  RMDir "$INSTDIR"
  
;Delete Start Menu Shortcuts
  Delete "$DESKTOP\${MUI_PRODUCT}.lnk"
  Delete "$SMPROGRAMS\${MUI_PRODUCT}\*.*"
  RmDir  "$SMPROGRAMS\${MUI_PRODUCT}"
  
;Delete Uninstaller And Unistall Registry Entries
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\${MUI_PRODUCT}"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}"  
  
SectionEnd

;--------------------------------    
;MessageBox Section


;Function that calls a messagebox when installation finished correctly
Function .onInstSuccess
;  MessageBox MB_OK "You have successfully installed ${MUI_PRODUCT}. Use the desktop icon to start the program."
FunctionEnd


Function un.onUninstSuccess
  MessageBox MB_OK "${MUI_PRODUCT} was successfully uninstalled."
FunctionEnd


;eof
