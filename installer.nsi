; THE SYSTEM — Windows Installer Script
; Generates: TheSystem-Setup.exe

!define APP_NAME      "The System"
!define APP_EXE       "TheSystem.exe"
!define APP_VERSION   "1.0"
!define INSTALL_DIR   "$PROGRAMFILES64\TheSystem"

Name            "${APP_NAME}"
OutFile         "TheSystem-Setup.exe"
InstallDir      "${INSTALL_DIR}"
InstallDirRegKey HKLM "Software\TheSystem" "Install_Dir"
RequestExecutionLevel admin

; ── Pages ──────────────────────────────────────────────────
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

; ── Install ────────────────────────────────────────────────
Section "MainSection" SEC01
  SetOutPath "$INSTDIR"

  ; Copy all files from the deploy folder
  File /r "deploy\*.*"

  ; Start Menu shortcut
  CreateDirectory "$SMPROGRAMS\${APP_NAME}"
  CreateShortcut  "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" \
                  "$INSTDIR\${APP_EXE}"
  CreateShortcut  "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk" \
                  "$INSTDIR\Uninstall.exe"

  ; Desktop shortcut
  CreateShortcut  "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}"

  ; Write uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Add to Windows Add/Remove Programs
  WriteRegStr HKLM \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheSystem" \
    "DisplayName" "${APP_NAME}"
  WriteRegStr HKLM \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheSystem" \
    "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheSystem" \
    "DisplayVersion" "${APP_VERSION}"
SectionEnd

; ── Uninstall ──────────────────────────────────────────────
Section "Uninstall"
  Delete "$INSTDIR\*.*"
  RMDir  /r "$INSTDIR"
  Delete "$SMPROGRAMS\${APP_NAME}\*.*"
  RMDir  "$SMPROGRAMS\${APP_NAME}"
  Delete "$DESKTOP\${APP_NAME}.lnk"
  DeleteRegKey HKLM \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheSystem"
SectionEnd