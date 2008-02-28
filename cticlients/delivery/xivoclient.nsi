# $Revision$
# $Date$
#
# NullSoft Installation System
#

Name "XIVO Client"
OutFile "xivoclient-setup.exe"
InstallDir "$PROGRAMFILES\XIVO"
LicenseText "XIVO Client est distribu� sous licence GNU General Public License v2 avec une exception sp�ciale vous autorisant � le lier � OpenSSL, sous certaines conditions."
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
File "OpenSSL.LICENCE.txt"
File "Qt.GPL.Exception.txt"
File "..\xivoclient\release\xivoclient.exe"
WriteUninstaller "$INSTDIR\uninstall-xivoclient.exe"
SectionEnd

# Shortcuts
Section "Shortcuts"
SetOutPath "$SMPROGRAMS\XIVO"
CreateShortCut "$SMPROGRAMS\XIVO\xivoclient.lnk"  "$INSTDIR\xivoclient.exe"
CreateShortCut "$SMPROGRAMS\XIVO\D�sinstaller xivoclient.lnk"  "$INSTDIR\uninstall-xivoclient.exe"
CreateShortCut "$DESKTOP\xivoclient.lnk"  "$INSTDIR\xivoclient.exe"
MessageBox MB_OK "Installation termin�e."
SectionEnd

# Uninstall
Section "Uninstall"
Delete "$INSTDIR\GPL_V2.txt"
Delete "$INSTDIR\OpenSSL.LICENCE.txt"
Delete "$INSTDIR\Qt.GPL.Exception.txt"
Delete "$INSTDIR\mingwm10.dll"
Delete "$INSTDIR\libeay32.dll"
Delete "$INSTDIR\ssleay32.dll"
Delete "$INSTDIR\msvcr71.dll"
Delete "$INSTDIR\xivoclient.exe"
Delete "$INSTDIR\uninstall-xivoclient.exe"
RmDir "$INSTDIR"

Delete "$DESKTOP\xivoclient.lnk"

Delete "$SMPROGRAMS\XIVO\xivoclient.lnk"
Delete "$SMPROGRAMS\XIVO\D�sinstaller xivoclient.lnk"
RmDir "$SMPROGRAMS\XIVO"

SectionEnd
