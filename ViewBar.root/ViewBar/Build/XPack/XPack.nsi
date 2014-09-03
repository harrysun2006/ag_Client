; Writen by CHarry - hi@charry.org

!define PRODUCT_NAME "AGLOCO Viewbar"
!define PRODUCT_VERSION "1.05"
!define PRODUCT_PUBLISHER "AGLOCO.com"
!define PRODUCT_WEB_SITE "http://www.agloco.com"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"
!define ICONS_GROUP "AGLOCO Viewbar"


SetCompressor /SOLID lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "icon\install.ico"
!define MUI_UNICON "icon\uninstall.ico"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
; Interface Configuration
; Header Bitmap
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp" ; optional

;--------------------------------

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE $(MUILicense)
; Components page
;!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page

/*
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "AGLOCO Viewbar"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
;!insertmacro MUI_PAGE_STARTMENU Application ${ICONS_GROUP}
*/

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
;!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
;!define MUI_FINISHPAGE_SHOWREADME_TEXT "$(msgViewReadMe)"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "TradChinese"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; License Language String
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LicenseLangString MUILicense ${LANG_ENGLISH} "license\License_en.txt"
LicenseLangString MUILicense ${LANG_SIMPCHINESE} "license\License_gb.txt"
LicenseLangString MUILicense ${LANG_TRADCHINESE} "license\License_big5.txt"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; i18n
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LangString msgSuccess ${LANG_ENGLISH} "$(^Name) has been removed from computer."
LangString msgSuccess ${LANG_SIMPCHINESE} "$(^Name)已经完全从您的计算机上删除了"
LangString msgSuccess ${LANG_TRADCHINESE} "$(^Name) has been removed from computer."

LangString msgRemove ${LANG_ENGLISH} "Are you sure you want to remove $(^Name) and all it's components?"
LangString msgRemove ${LANG_SIMPCHINESE} "您确定要完全移除$(^Name)及其所有组件？"
LangString msgRemove ${LANG_TRADCHINESE} "Are you sure you want to remove $(^Name) and all it's components?"

LangString msgMain ${LANG_ENGLISH} "AGLOCO Viewbar Application"
LangString msgMain ${LANG_SIMPCHINESE} "AGLOCO Viewbar应用程序"
LangString msgMain ${LANG_TRADCHINESE} "AGLOCO Viewbar Application"

LangString msgViewReadMe ${LANG_ENGLISH} "View Readme"
LangString msgViewReadMe ${LANG_SIMPCHINESE} "阅读AGLOCO Viewbar说明"
LangString msgViewReadMe ${LANG_TRADCHINESE} "View Readme"

LangString msgVBRunning ${LANG_ENGLISH} "Viewbar is Running, You must quit Viewbar first!"
LangString msgVBRunning ${LANG_SIMPCHINESE} "Viewbar正在运行，请先退出Viewbar!"
LangString msgVBRunning ${LANG_TRADCHINESE} "Viewbar is Running, You must quit Viewbar first!"

LangString msgWarningForNonAdmin ${LANG_ENGLISH} "If you want to run Viewbar, you must be administrator!"
LangString msgWarningForNonAdmin ${LANG_SIMPCHINESE} "运行Viewbar必须有管理员权限"
LangString msgWarningForNonAdmin ${LANG_TRADCHINESE} "If you want to run Viewbar, you must be administrator!"
; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Setup.exe"
InstallDir "$PROGRAMFILES\AGLOCO Viewbar"
ShowInstDetails show
ShowUnInstDetails show
BrandingText "AGLOCO Viewbar - www.agloco.com"

Function .onInit
  ;!insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; General Section
; Section with '-' means hidden section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Section
	ClearErrors
	UserInfo::GetName
	IfErrors Win9x
	Pop $0
	UserInfo::GetAccountType
	Pop $1
	StrCmp $1 "Admin" 0 +3
		;MessageBox MB_OK 'User "$0" is in the Administrators group'
		Goto done
	StrCmp $1 "Power" 0 +3
		;MessageBox MB_OK 'User "$0" is in the Power Users group'
		Goto invalid_user
	StrCmp $1 "User" 0 +3
		;MessageBox MB_OK 'User "$0" is just a regular user'
		Goto invalid_user
	StrCmp $1 "Guest" 0 +3
		;MessageBox MB_OK 'User "$0" is a guest'
		Goto invalid_user
	MessageBox MB_OK "Unknown error"
	Goto done

	Win9x:
		# This one means you don't need to care about admin or
		# not admin because Windows 9x doesn't either
		MessageBox MB_OK "Error! This DLL can't run under Windows 9x!"

	invalid_user:
		MessageBox MB_OK "If you want to run Viewbar, you must be administrator!"
		Quit
	done:
SectionEnd

Section -SEC_General
  
  ; install for viewbar
  SetOutPath "$INSTDIR\"
  SetOverwrite ifnewer
  File /r "..\Release\*.*"
  
  ; MSXML DLL
  SetOutPath "$SYSDIR\"
  SetOverwrite off
  File /r "files\*.*"
  RegDLL "$SYSDIR\msxml3.dll" 


; Shortcuts
/*
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\${ICONS_GROUP}"
  CreateShortCut "$SMPROGRAMS\${ICONS_GROUP}\AGLOCO Viewbar.lnk" "$INSTDIR\Viewbar.exe"
  CreateShortCut "$DESKTOP\AGLOCO Viewbar.lnk" "$INSTDIR\Viewbar.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
  */
SectionEnd



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  addtional icons
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Section -AdditionalIcons
  SetOutPath $INSTDIR
  ;!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateDirectory "$SMPROGRAMS\${ICONS_GROUP}"
  CreateShortCut "$SMPROGRAMS\${ICONS_GROUP}\${PRODUCT_NAME}.lnk" "$INSTDIR\Viewbar.exe"
  CreateShortCut "$SMPROGRAMS\${ICONS_GROUP}\AGLOCO Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\${ICONS_GROUP}\Uninstall.lnk" "$INSTDIR\uninst.exe"
  
  CreateShortCut "$DESKTOP\AGLOCO Viewbar.lnk" "$INSTDIR\Viewbar.exe"

  ;!insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Section descriptions
;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
;!insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "$(msgMain)"
;!insertmacro MUI_FUNCTION_DESCRIPTION_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; event when uninstall successfully
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(msgSuccess)"
FunctionEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  prompt before uninstall
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
FunctionEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; uninstall
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Section Uninstall
  ;!insertmacro MUI_STARTMENU_GETFOLDER "Application" ${ICONS_GROUP}
  
  
  Delete "$INSTDIR\Viewbar.exe"
  IfErrors 0 end
	MessageBox MB_OK "$(msgVBRunning)"
	Quit
  end:
  
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\README.txt"

  Delete "$SMPROGRAMS\${ICONS_GROUP}\Uninstall.lnk"
  Delete "$SMPROGRAMS\${ICONS_GROUP}\AGLOCO Website.lnk"
  Delete "$SMPROGRAMS\${ICONS_GROUP}\${PRODUCT_NAME}.lnk"
;  Delete "$SMPROGRAMS\${ICONS_GROUP}\AGLOCO Viewbar.lnk"
  Delete "$DESKTOP\AGLOCO Viewbar.lnk"


  RMDir "$SMPROGRAMS\${ICONS_GROUP}"

  RMDir /r "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "Software\AGLOCO"

  SetAutoClose true
SectionEnd




