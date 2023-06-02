set buildpath=%~dp0
set sdkfolder=%buildpath%..\
echo %sdkfolder%
echo "Checking submodules"
call git submodule update --init
echo "Building libcurl..."
cd %sdkfolder%libs\libcurl
set RTLIBCFG=static
call .\buildconf.bat
cd %sdkfolder%libs\libcurl\winbuild
call nmake /f Makefile.vc mode=static vc=17 debug=yes
if  errorlevel 1 goto ERROR
call nmake /f Makefile.vc mode=static vc=17 debug=no
if  errorlevel 1 goto ERROR

cd %sdkfolder%libs\zlib

call nmake /f win32/Makefile.msc

mkdir "%sdkfolder%build"
cd "%sdkfolder%build"

cmake ..

goto EOF 
:ERROR
echo Failed
cmd /k
exit /b 1

:EOF