@echo off

echo Building boost...
pushd boost

.\bootstrap && .\bjam toolset=msvc-9.0 link=static runtime-link=shared  && popd