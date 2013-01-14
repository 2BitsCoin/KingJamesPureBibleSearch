;****************************************************************************
;**
;** Copyright (C) 2012 Donna Whisnant, a.k.a. Dewtronics.
;** Contact: http://www.dewtronics.com/
;**
;** This file is part of the KJVCanOpener Application as originally written
;** and developed for Bethel Church, Festus, MO.
;**
;** GNU General Public License Usage
;** This file may be used under the terms of the GNU General Public License
;** version 3.0 as published by the Free Software Foundation and appearing
;** in the file gpl-3.0.txt included in the packaging of this file. Please
;** review the following information to ensure the GNU General Public License
;** version 3.0 requirements will be met:
;** http://www.gnu.org/copyleft/gpl.html.
;**
;** Other Usage
;** Alternatively, this file may be used in accordance with the terms and
;** conditions contained in a signed written agreement between you and
;** Dewtronics.
;**
;****************************************************************************

#include "..\InnoSetup\version.iss"

[Setup]
AppName=King James Pure Bible Search
AppId=KJVPureBibleSearch
#expr VersionInstaller("..\KJVCanOpener-build-desktop-Qt_4_8_3\release\KJVCanOpener.exe")
AppCopyright=Copyright (C) 2012 Donna Whisnant, a.k.a. Dewtronics.
AppPublisher=Dewtronics/Bethel Church
AppPublisherURL=http://www.dewtronics.com/
AppContact=Bethel Church
AppSupportURL=http://visitbethelchurch.com/
AppSupportPhone=(636)-931-3999
AppComments=King James Pure Bible Search ("Can Opener") Program.  Brought to you by the fervent prayers of Bethel Church, Festus, MO.
DefaultDirName={pf}\KJVPureBibleSearch
DefaultGroupName=King James Pure Bible Search
ShowLanguageDialog=auto
LicenseFile=LICENSE.txt
Compression=lzma
ChangesAssociations=yes
PrivilegesRequired=admin

[InstallDelete]
; Remove old database since it was still opening read/write and user's computer will
; create a "virtual store" for it in the user's folder.  See if we can find it and
; remove it.  Darn Windoze!
Type: files; Name: "{localappdata}\VirtualStore\Program Files\KJVCanOpener\db\kjvtext.s3db"; 
Type: files; Name: "{localappdata}\VirtualStore\Program Files (x86)\KJVCanOpener\db\kjvtext.s3db";
Type: dirifempty; Name: "{localappdata}\VirtualStore\Program Files\KJVCanOpener";
Type: dirifempty; Name: "{localappdata}\VirtualStore\Program Files\KJVCanOpener\db";
Type: dirifempty; Name: "{localappdata}\VirtualStore\Program Files (x86)\KJVCanOpener";
Type: dirifempty; Name: "{localappdata}\VirtualStore\Program Files (x86)\KJVCanOpener\db";


[Files]
; app
Source: "..\KJVCanOpener-build-desktop-Qt_4_8_3\release\KJVCanOpener.exe"; DestDir: "{app}\KJVCanOpener\app"; Flags: ignoreversion;
Source: {#GetEnv('QTDIR')}\bin\QtCore4.dll; DestDir: "{app}\KJVCanOpener\app"; Flags: ignoreversion;
Source: {#GetEnv('QTDIR')}\bin\QtGui4.dll; DestDir: "{app}\KJVCanOpener\app"; Flags: ignoreversion;
Source: {#GetEnv('QTDIR')}\bin\QtSql4.dll; DestDir: "{app}\KJVCanOpener\app"; Flags: ignoreversion;
Source: {#GetEnv('QTDIR')}\..\..\..\MinGW\bin\libstdc++-6.dll; DestDir: "{app}\KJVCanOpener\app"; Flags: ignoreversion;
Source: {#GetEnv('QTDIR')}\..\..\..\MinGW\bin\libgcc_s_dw2-1.dll; DestDir: "{app}\KJVCanOpener\app"; Flags: ignoreversion;

; plugins/sqldrivers
Source: {#GetEnv('QTDIR')}\plugins\sqldrivers\qsqlite4.dll; DestDir: "{app}\KJVCanOpener\plugins\sqldrivers"; Flags: ignoreversion;

; plugins/imageformats
Source: {#GetEnv('QTDIR')}\plugins\imageformats\qgif4.dll; DestDir: "{app}\KJVCanOpener\plugins\imageformats"; Flags: ignoreversion;
Source: {#GetEnv('QTDIR')}\plugins\imageformats\qico4.dll; DestDir: "{app}\KJVCanOpener\plugins\imageformats"; Flags: ignoreversion;
Source: {#GetEnv('QTDIR')}\plugins\imageformats\qjpeg4.dll; DestDir: "{app}\KJVCanOpener\plugins\imageformats"; Flags: ignoreversion;
Source: {#GetEnv('QTDIR')}\plugins\imageformats\qmng4.dll; DestDir: "{app}\KJVCanOpener\plugins\imageformats"; Flags: ignoreversion;
;Source: {#GetEnv('QTDIR')}\plugins\imageformats\qsvg4.dll; DestDir: "{app}\KJVCanOpener\plugins\imageformats"; Flags: ignoreversion;
;Source: {#GetEnv('QTDIR')}\plugins\imageformats\qtga4.dll; DestDir: "{app}\KJVCanOpener\plugins\imageformats"; Flags: ignoreversion;
;Source: {#GetEnv('QTDIR')}\plugins\imageformats\qtiff4.dll; DestDir: "{app}\KJVCanOpener\plugins\imageformats"; Flags: ignoreversion;

; db
Source: "db\kjvtext2.s3db"; DestDir: "{app}\KJVCanOpener\db"; Flags: ignoreversion; Permissions: users-modify;
Source: "db\kjvuser.s3db"; DestDir: "{app}\KJVCanOpener\db"; Flags: onlyifdoesntexist uninsneveruninstall; Permissions: users-modify;

; license
Source: "gpl-3.0.txt"; DestDir: "{app}\KJVCanOpener\license"; Flags: ignoreversion;

; {fonts}
Source: "fonts/SCRIPTBL.TTF"; DestDir: "{fonts}"; FontInstall: "Script MT Bold"; Flags: onlyifdoesntexist uninsneveruninstall;


 [Registry]
Root: HKCR; Subkey: ".kjs"; ValueType: string; ValueName: ""; ValueData: "KJVCanOpener"; Flags: uninsdeletevalue;
Root: HKCR; Subkey: "KJVCanOpener"; ValueType: string; ValueName: ""; ValueData: "KJVCanOpener"; Flags: uninsdeletekey;
Root: HKCR; Subkey: "KJVCanOpener\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\KJVCanOpener\app\KJVCanOpener.exe,0";
Root: HKCR; Subkey: "KJVCanOpener\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\KJVCanOpener\app\KJVCanOpener.exe"" ""%1""";

[Icons]
Name: {group}\{#SetupSetting("AppName")}; Filename: "{app}\KJVCanOpener\app\KJVCanOpener.exe"

[Run]
Filename: {app}\KJVCanOpener\app\KJVCanOpener.exe; Description: {cm:LaunchProgram,{#SetupSetting("AppName")}}; Flags: nowait postinstall skipifsilent

