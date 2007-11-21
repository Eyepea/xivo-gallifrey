# $Revision$
# $Date$
#
# NullSoft Installation System
#

Name "XIVO Client"
OutFile "xivoclient-setup.exe"
InstallDir "$PROGRAMFILES\XIVO"
LicenseText "XIVO Client est distribu� sous licence GPL."
# ComponentText "(Choix des composants / sections)"
# DirText "(Choix du r�pertoire d'installation)"

LicenseData "GPL_V2.txt"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\French.nlf"

# Installation of executables
Section "Prog"
SetOutPath $INSTDIR
File "C:\MinGW\bin\mingwm10.dll"
File "C:\WINDOWS\system32\libeay32.dll"
File "C:\WINDOWS\system32\ssleay32.dll"
File "C:\WINDOWS\system32\msvcr71.dll"
File "GPL_V2.txt"
File "..\xivoclient\release\xivoclient.exe"
SectionEnd

# Shortcuts
Section "Shortcuts"
# SetOutPath "$SMPROGRAMS\XIVO"
# CreateShortCut "$SMPROGRAMS\XIVO\xivoclient.lnk" "$INSTDIR\xivoclient.exe"
CreateShortCut "$DESKTOP\xivoclient.lnk" "$INSTDIR\xivoclient.exe"
MessageBox MB_OK "Installation termin�e."
SectionEnd
