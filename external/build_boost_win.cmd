@echo off

echo Building boost...
pushd boost

.\bootstrap && .\bjam link=static runtime-link=static && popd