@echo off

echo Building wxwidgets...
pushd wxwidgets\build\msw

REM Using a .sln might be faster, but don't want to keep all the .vcprojs up-to-date
nmake -f makefile.vc BUILD=debug UNICODE=1 > wxwidgets_build.log

popd
