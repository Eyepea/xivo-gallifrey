# $Revision$
# $Date$
#
# NullSoft Installation System
#

Name "XIVO Client"
OutFile "xivoclient-setup-win32.exe"
InstallDir "$PROGRAMFILES\XIVO"
InstallDirRegKey HKLM "Software\XIVO\xivoclient" "Install_Dir" 
LicenseText "XIVO Client est distribu� sous licence GNU General Public License v2 avec une exception sp�ciale vous autorisant � le lier � OpenSSL, sous certaines conditions."
# ComponentText "(Choix des composants / sections)"
# DirText "(Choix du r�pertoire d'installation)"

LicenseData "LICENSE"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\French.nlf"

# Installation of executables
Section "Prog"
SetOutPath $INSTDIR
File "C:\MinGW\bin\mingwm10.dll"
File "C:\cygwin\home\Administrateur\xivo-trunk\openssl-0.9.8g-mingw\openssl-0.9.8g\cryptoeay32-0.9.8.dll"
File "C:\cygwin\home\Administrateur\xivo-trunk\openssl-0.9.8g-mingw\openssl-0.9.8g\ssleay32-0.9.8.dll"
File "LICENSE"
File "GPL_V2.txt"
File "OpenSSL.LICENSE.txt"
File "Qt.GPL.Exception.txt"
File "Qt.GPL.Exception.Addendum.txt"
File "..\xivoclient\release\xivoclient.exe"
# Write keys in Registry in order for the applications to appear in Add/Remove Programs
WriteRegStr HKLM "Software\XIVO\xivoclient" "Install_Dir" "$INSTDIR"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\xivoclient" "DisplayName" "XIVO Client"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\xivoclient" "UninstallString" '"$INSTDIR\uninstall-xivoclient.exe"'
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\xivoclient" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\xivoclient" "NoRepair" 1
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
# Do not remove the common CTI files if switchboard is installed
IfFileExists "$INSTDIR\switchboard.exe" OnlyUninstallXC
Delete "$INSTDIR\GPL_V2.txt"
Delete "$INSTDIR\OpenSSL.LICENSE.txt"
Delete "$INSTDIR\Qt.GPL.Exception.txt"
Delete "$INSTDIR\Qt.GPL.Exception.Addendum.txt"
Delete "$INSTDIR\LICENSE"
Delete "$INSTDIR\mingwm10.dll"
Delete "$INSTDIR\cryptoeay32-0.9.8.dll"
Delete "$INSTDIR\ssleay32-0.9.8.dll"
OnlyUninstallXC:
Delete "$INSTDIR\xivoclient.exe"
Delete "$INSTDIR\uninstall-xivoclient.exe"
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\xivoclient"
DeleteRegKey HKLM "Software\XIVO\xivoclient"
RmDir "$INSTDIR"

Delete "$DESKTOP\xivoclient.lnk"

Delete "$SMPROGRAMS\XIVO\xivoclient.lnk"
Delete "$SMPROGRAMS\XIVO\D�sinstaller xivoclient.lnk"
RmDir "$SMPROGRAMS\XIVO"

SectionEnd
