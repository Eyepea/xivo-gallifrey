# $Revision$
# $Date$

Name "XIVO Switchboard"
OutFile "switchboard_setup.exe"

InstallDir "$PROGRAMFILES\XIVO"

# Installation of executables 
Section "Prog"

SetOutPath $INSTDIR

File "C:\MinGW\bin\mingwm10.dll"
File "C:\WINDOWS\system32\libeay32.dll"
File "C:\WINDOWS\system32\ssleay32.dll"
File "C:\WINDOWS\system32\msvcr71.dll"
File "GPL_V2.txt"
File "..\switchboard\release\switchboard.exe"
 
SectionEnd

# Shortcuts 
Section "Shortcuts"

SetOutPath "$SMPROGRAMS\XIVO"
# CreateShortCut "$SMPROGRAMS\XIVO\switchboard.lnk" "$INSTDIR\switchboard.exe"
CreateShortCut "$DESKTOP\switchboard.lnk" "$INSTDIR\switchboard.exe"

MessageBox MB_OK "Installation termin�e"

SectionEnd


