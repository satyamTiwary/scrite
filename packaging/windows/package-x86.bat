copy ..\..\..\Release\scrite.exe .
copy C:\Qt\Qt5.13.2\vcredist\vcredist_msvc2017_x86.exe vcredist_x86.exe
windeployqt --qmldir ..\..\qml --no-compiler-runtime --no-translations . --list relative > files.txt
fillnsi --installs-key WINDEPLOYQT_INSTALLS --uninstalls-key WINDEPLOYQT_UNINSTALLS --list files.txt --input installer-x86.nsi.in --output installer-x86.nsi
%MakeNSISTool% installer-x86.nsi
%CodeSignTool% sign /f %TERIFLIX_CSC% /p %TERIFLIX_CSC_PWORD% Scrite-*-Beta-32bit-Setup.exe
