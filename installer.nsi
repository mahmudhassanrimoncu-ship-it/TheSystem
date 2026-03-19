; ============================================================
;  THE SYSTEM — Windows Installer Script
;  Generates: TheSystem-Setup.exe
;  Used by: .github/workflows/build-windows.yml
; ============================================================

!define APP_NAME      "The System"
!define APP_EXE       "TheSystem.exe"
!define APP_VERSION   "1.0"
!define INSTALL_DIR   "$PROGRAMFILES64\TheSystem"

Name            "${APP_NAME}"
OutFile         "TheSystem-Setup.exe"
InstallDir      "${INSTALL_DIR}"
InstallDirRegKey HKLM "Software\TheSystem" "Install_Dir"
RequestExecutionLevel admin
SetCompressor    lzma

; ── Installer pages ─────────────────────────────────────────
Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

; ── Install section ─────────────────────────────────────────
Section "Install" SEC01
  SetOutPath "$INSTDIR"

  ; Copy everything from the deploy folder (app + all Qt DLLs)
  File /r "deploy\*"

  ; Write uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Desktop shortcut
  CreateShortcut "$DESKTOP\${APP_NAME}.lnk" \
    "$INSTDIR\${APP_EXE}" "" \
    "$INSTDIR\${APP_EXE}" 0

  ; Start Menu shortcuts
  CreateDirectory "$SMPROGRAMS\${APP_NAME}"
  CreateShortcut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" \
    "$INSTDIR\${APP_EXE}"
  CreateShortcut "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk" \
    "$INSTDIR\Uninstall.exe"

  ; Register with Windows Add/Remove Programs
  WriteRegStr HKLM \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheSystem" \
    "DisplayName" "${APP_NAME}"
  WriteRegStr HKLM \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheSystem" \
    "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheSystem" \
    "DisplayVersion" "${APP_VERSION}"
  WriteRegStr HKLM \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheSystem" \
    "Publisher" "THE SYSTEM"
  WriteRegDWORD HKLM \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheSystem" \
    "NoModify" 1
  WriteRegDWORD HKLM \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheSystem" \
    "NoRepair" 1
SectionEnd

; ── Uninstall section ────────────────────────────────────────
Section "Uninstall"
  Delete "$INSTDIR\*.*"
  RMDir  /r "$INSTDIR"
  Delete "$DESKTOP\${APP_NAME}.lnk"
  Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
  Delete "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk"
  RMDir  "$SMPROGRAMS\${APP_NAME}"
  DeleteRegKey HKLM \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\TheSystem"
  DeleteRegKey HKLM "Software\TheSystem"
SectionEnd
