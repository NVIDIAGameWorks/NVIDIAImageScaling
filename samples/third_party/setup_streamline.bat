@echo off
rem Select you Visual Studio version by adding the vs2017 (default) or vs2019 to the command line
rem Example: setup_streamline.bat vs2019

set STREAMLINE_REPO=https://github.com/NVIDIAGameWorks/Streamline.git
set BRANCH=main
set TAG=v1.1.1

del /S /F /Q Streamline
rmdir /S /Q Streamline

git clone  %STREAMLINE_REPO% --recurse-submodules -b %BRANCH%
pushd Streamline
git checkout tags/%TAG% -b %TAG%

call setup.bat %1
call build.bat -release
call build.bat -production
popd