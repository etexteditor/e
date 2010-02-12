; -- e(beta).iss --

; ATTENTION: Remember before building installer:
;   1. Enable XCrash lib in debug build.
;   2. Set correct version name/# in eApp::OnInit
;   3. Set correct version # in AppVerName
;   4. Set correct version # "Software\e" registry key
;   (5. Update beta_intro.txt)

[Setup]
AppName=e
AppVerName=e - v1.0.43
AppMutex=eApp
OutputBaseFilename=e_setup
AppPublisherURL=http://www.e-texteditor.com/
AppSupportURL=http://www.e-texteditor.com/
AppUpdatesURL=http://www.e-texteditor.com/
DefaultDirName={pf}\e
DisableProgramGroupPage=yes
; ^ since no icons will be created in "{group}", we don't need the wizard
;   to ask for a group name.
UninstallDisplayIcon={app}\e.exe
WizardImageFile=installer\e_setup.bmp
WizardSmallImageFile=installer\e_logo_new.bmp
LicenseFile=installer\eula.txt
DisableReadyPage=yes
Compression=lzma/ultra
InternalCompressLevel=ultra
SolidCompression=yes
ChangesAssociations=yes
ChangesEnvironment=yes

;[InstallDelete]
; Files to delete before starting installation
;Type: files; Name: "{app}\e.dat"; Flags: postinstall

[Files]
; Copy old settings to AppData for Vista compatibility
Source: "{app}\e.db"; DestDir: "{userappdata}\e"; Flags: external skipifsourcedoesntexist onlyifdoesntexist
Source: "{app}\config.db"; DestDir: "{userappdata}\e"; Flags: external skipifsourcedoesntexist onlyifdoesntexist
Source: "{app}\license_key"; DestDir: "{userappdata}\e"; Flags: external skipifsourcedoesntexist onlyifdoesntexist
Source: "{app}\Bundles\local\*.*"; DestDir: "{userappdata}\e\Bundles"; Flags: external skipifsourcedoesntexist recursesubdirs createallsubdirs onlyifdoesntexist
Source: "{app}\Themes\local\*.*"; DestDir: "{userappdata}\e\Themes"; Flags: external skipifsourcedoesntexist recursesubdirs createallsubdirs onlyifdoesntexist
Source: "{app}\.df\*.*"; DestDir: "{userappdata}\e\.df"; Flags: external skipifsourcedoesntexist recursesubdirs createallsubdirs onlyifdoesntexist
Source: "{app}\.wf\*.*"; DestDir: "{userappdata}\e\.wf"; Flags: external skipifsourcedoesntexist recursesubdirs createallsubdirs onlyifdoesntexist

; Installation
Source: "Release\e.exe"; DestDir: "{app}"
Source: "..\docs\release_notes.txt"; DestDir: "{app}"
Source: "XCrashReport.exe"; DestDir: "{app}"
Source: "installer\dbghelp.dll"; DestDir: "{app}"
Source: "e-exe\Debug\Bundles\*.*"; DestDir: "{app}\Bundles"; Excludes: "\local"; Flags: recursesubdirs createallsubdirs
Source: "e-exe\Debug\Support\*.*"; DestDir: "{app}\Support"; Excludes: "\bin\CocoaDialog.app"; Flags: recursesubdirs createallsubdirs
Source: "e-exe\Debug\Themes\*.*"; DestDir: "{app}\Themes"; Excludes: "\local"; Flags: recursesubdirs createallsubdirs

; Install emate command
Source: "cmd\e.exe"; DestDir: "{app}\cmd"

; Install shortcuts (cygwin)
Source: "cmd\mate.exe.lnk"; DestDir: "{app}\Support\bin"; Attribs: readonly
Source: "cmd\CocoaDialog.exe.lnk"; DestDir: "{app}\Support\bin\CocoaDialog.app\Contents\MacOS"; Attribs: readonly

[Icons]
Name: "{commonprograms}\e - texteditor"; Filename: "{app}\e.exe"
Name: "{app}\cmd\mate.exe"; Filename: "{app}\cmd\e.exe"
Name: "{userdesktop}\e - texteditor"; Filename: "{app}\e.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\e - texteditor"; Filename: "{app}\e.exe"; Tasks: quicklaunchicon

[Registry]
Root: HKCU; Subkey: "Software\e"; ValueType: string; ValueName: "version"; ValueData: "1.0.43 (211)"
Root: HKCU; Subkey: "Software\e"; ValueType: string; ValueName: "path"; ValueData: "{app}\e.exe"
Root: HKCR; Subkey: "txtfile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\e.exe,1"; Tasks: associate
Root: HKCR; Subkey: "txtfile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\e.exe"" ""%1"""; Tasks: associate
Root: HKCR; Subkey: "*\shell\edit_e"; ValueType: string; ValueName: ""; ValueData: "&Edit with e"; Tasks: contextmenu; Flags: uninsdeletekey
Root: HKCR; Subkey: "*\shell\edit_e\command"; ValueType: string; ValueName: ""; ValueData: """{app}\e.exe"" ""%1"""; Tasks: contextmenu; Flags: uninsdeletekey
Root: HKCR; Subkey: "directory\shell\open_e_project"; ValueType: string; ValueName: ""; ValueData: "Open as e Project"; Tasks: contextmenu; Flags: uninsdeletekey
Root: HKCR; Subkey: "directory\shell\open_e_project\command"; ValueType: string; ValueName: ""; ValueData: """{app}\e.exe"" ""%1"""; Tasks: contextmenu; Flags: uninsdeletekey
; Register txmt protocol
Root: HKCR; Subkey: "txmt"; ValueType: string; ValueName: ""; ValueData: "URL:txmt protocol"; Flags: uninsdeletekey
Root: HKCR; Subkey: "txmt"; ValueType: string; ValueName: "URL Protocol"; ValueData: ""; Flags: uninsdeletekey
Root: HKCR; Subkey: "txmt\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\e.exe"; Flags: uninsdeletekey
Root: HKCR; Subkey: "txmt\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\e.exe"" ""%1"""; Flags: uninsdeletekey

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"
Name: quicklaunchicon; Description: "Create a &Quick Launch icon"
Name: associate; Description: "Make e the default for opening .txt files"
Name: contextmenu; Description: "Add ""Edit with e"" to explorers right click menu"
Name: modifypath; Description: "Add e command line tool to your system path";

[Run]
;Filename: "msiexec.exe"; Parameters: "/lv inslog.txt /i ""{tmp}\BonjourCore-1_0_3.msi"""
;Filename: "{app}\e.exe"; Description: "Launch e"; Flags: postinstall nowait
Filename: "{app}\e.exe"; Parameters: """{app}\release_notes.txt"""; Description: "Launch e"; Flags: postinstall nowait; OnlyBelowVersion: 0,6;


[Code]
function ModPathDir(): TArrayOfString;
var
    Dir: TArrayOfString;
begin
    setArrayLength(Dir, 1)
    Dir[0] := ExpandConstant('{app}\cmd');
    Result := Dir;
end;
#include "modpath.iss"

procedure CurStepChanged(CurStep: TSetupStep);
var
  path: string;
  bakpath: string;
begin
  // Clean up before installing bundles
  if (CurStep = ssInstall) then begin
    path := ExpandConstant('{app}\Bundles');
    if DirExists(path) then begin
      DelTree(path, True, True, True);
    end;
    
    path := ExpandConstant('{app}\Support');
    if DirExists(path) then begin
      DelTree(path, True, True, True);
    end;
    
    path := ExpandConstant('{app}\Themes');
    if DirExists(path) then begin
      DelTree(path, True, True, True);
    end;
  end;
  
  if (CurStep = ssPostInstall) then begin
  
		// First update the path (environment)
    if IsTaskSelected('modifypath') then
			ModPath();
  
    // Move old settings to new location (appdata dir)
    if DirExists(ExpandConstant('{app}')) then begin
      // Create dir for User Application Data
      path := ExpandConstant('{userappdata}\e');
      bakpath := ExpandConstant('{app}\settings_backup');
      if (not DirExists(path)) then CreateDir(path);

      // Move local config files
      path := ExpandConstant('{app}\e.db');
      if FileExists(path) then begin
        if (not DirExists(bakpath)) then CreateDir(bakpath);
        RenameFile(path, ExpandConstant('{app}\settings_backup\e.db'));
      end;

      path := ExpandConstant('{app}\config.db');
      if FileExists(path) then begin
        if (not DirExists(bakpath)) then CreateDir(bakpath);
        RenameFile(path, ExpandConstant('{app}\settings_backup\config.db'));
      end;

      path := ExpandConstant('{app}\license_key');
      if FileExists(path) then begin
        if (not DirExists(bakpath)) then CreateDir(bakpath);
        RenameFile(path, ExpandConstant('{app}\settings_backup\license_key'));
      end;

      // Move Buffer dirs
      path := ExpandConstant('{app}\.df');
      if DirExists(path) then begin
        if (not DirExists(bakpath)) then CreateDir(bakpath);
        RenameFile(path, ExpandConstant('{app}\settings_backup\.df'));
      end;

      path := ExpandConstant('{app}\.wf');
      if DirExists(path) then begin
        if (not DirExists(bakpath)) then CreateDir(bakpath);
        RenameFile(path, ExpandConstant('{app}\settings_backup\.wf'));
      end;

      // Local Bundles
      path := ExpandConstant('{app}\Bundles\local');
      if DirExists(path) then begin
        if (not DirExists(bakpath)) then CreateDir(bakpath);
        RenameFile(path, ExpandConstant('{app}\settings_backup\Bundles'));
      end;

      path := ExpandConstant('{app}\Themes\local');
      if DirExists(path) then begin
        if (not DirExists(bakpath)) then CreateDir(bakpath);
        RenameFile(path, ExpandConstant('{app}\settings_backup\Themes'));
      end;

    end;
  end;
end;


