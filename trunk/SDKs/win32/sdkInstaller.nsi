; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define BUILD_DIR "tmp"
!define PRODUCT_NAME "OpenGUI SDK"
!define PRODUCT_VERSION "0.8"
!define PRODUCT_CODENAME "Kunzite"
!define PRODUCT_WEB_SITE "http://opengui.sourceforge.net/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\OpenGUI_SDK"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor bzip2

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
;!insertmacro MUI_PAGE_LICENSE "OpenGUI\LICENSE.TXT"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
;!define MUI_FINISHPAGE_RUN "$INSTDIR\Demos\OpenGL\Demo1.exe"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\CHANGELOG.txt"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION} [${PRODUCT_CODENAME}]"
OutFile "OpenGUI_SDK_${PRODUCT_VERSION}.exe"
InstallDir "c:\OpenGUI_SDK"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show


Section -AdditionalIcons
  CreateDirectory "$SMPROGRAMS\OpenGUI SDK"
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\OpenGUI SDK\OpenGUI Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\OpenGUI SDK\Uninstall.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$SMPROGRAMS\OpenGUI SDK\Open Directory.lnk" "$INSTDIR\"
SectionEnd


Section "VS SDK" SEC01
  SetOutPath "$INSTDIR"
  File /r "${BUILD_DIR}\Demos"
  File /r "${BUILD_DIR}\OpenGUI"
  CreateShortCut "$SMPROGRAMS\OpenGUI SDK\API Documentation.lnk" "$INSTDIR\OpenGUI\OpenGUI.chm"  
  File /r "${BUILD_DIR}\Renderer_Ogre"
  CreateShortCut "$SMPROGRAMS\OpenGUI SDK\Ogre Documentation.lnk" "$INSTDIR\Renderer_Ogre\Renderer_Ogre.chm"
  File /r "${BUILD_DIR}\Renderer_OpenGL"
SectionEnd


Section "Source" SEC02
  SetOutPath "$INSTDIR"
  File /r "${BUILD_DIR}\src"
SectionEnd


Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Demo1.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
;  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Demo1.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Precompiled SDKs for Visual Studio"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "Compilable Source. Requires SCons. Ogre optional."
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  RMDir /r "$SMPROGRAMS\OpenGUI SDK"
  RMDir /r "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd