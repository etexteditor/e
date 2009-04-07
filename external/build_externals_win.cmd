@echo off

call :BUILD libtommath\libtommath.sln
call :BUILD libtomcrypt\libtomcrypt.sln
call :BUILD curl\lib\curllib.sln
call :BUILD metakit\win\msvc90\mksrc.sln
call :BUILD pcre\pcre.sln
call :BUILD tinyxml\tinyxml.sln

REM Using a .sln might be faster, but don't want to keep all the .vcprojs up-to-date
pushd wxwidgets\build\msw
nmake -f makefile.vc BUILD=debug UNICODE=1 > build_logs\wxwidgets.log
popd

echo Builds complete, check build_logs for possible issues.

goto :EOF


REM **** Subroutines start here ****

:BUILD
setlocal
REM Args are: path_to_sln, [config_to_build=DEBUG]
set CONFIG=%2
if {%CONFIG%}=={} set CONFIG="DEBUG"

echo Building %1...
devenv %1 /Build %CONFIG% > build_logs\%~n1.log
set RET=%ERRORLEVEL%
taskkill.exe /f /t /im mspdbsrv.exe > nul 2> nul
endlocal & set RET=%RET%
goto :EOF
