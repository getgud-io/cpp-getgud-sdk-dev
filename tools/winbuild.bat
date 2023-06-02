set buildpath=%~dp0
set sdkfolder=%buildpath%..\
echo %sdkfolder%

set result=1
if not exist "%sdkfolder%libs\libcurl" (
	set result=0
)

if not exist "%sdkfolder%libs\zlib" (
	set result=0
)

if %result%==0 (
	echo Checking submodules
	call git submodule update --init
)

if not exist "%sdkfolder%libs\libcurl\builds\" (
	echo Building libcurl...
	cd %sdkfolder%libs\libcurl
	set RTLIBCFG=static
	call .\buildconf.bat
	cd %sdkfolder%libs\libcurl\winbuild
	call nmake /f Makefile.vc mode=static vc=17 debug=yes
	if  errorlevel 1 goto ERROR
	call nmake /f Makefile.vc mode=static vc=17 debug=no
	if  errorlevel 1 goto ERROR
) else (
	echo CURL exists
)

if not exist %sdkfolder%libs\zlib\zlib.lib (
	echo Building zlib...
	cd %sdkfolder%libs\zlib
	call nmake /f win32/Makefile.msc
) else  (
	echo ZLIB exists
)

mkdir "%sdkfolder%build"
cd "%sdkfolder%build"

cmake ..

goto EOF 
:ERROR
echo Failed
cmd /k
exit /b 1

:EOF