@echo off
setlocal EnableExtensions

REM === CLI unchanged: %1 = qmake path, %2 = build dir ===
cd /d "%~dp0"
set "currDIR=%CD%"

set "qmakeCmd=%~1"
set "buildDIR=%~2"

REM normalize slashes
set "qmakeCmd=%qmakeCmd:/=\%"
set "buildDIR=%buildDIR:/=\%"

set "deployDIR=%currDIR%\..\..\neuTube_win64"
set "libDIR=%currDIR%\..\lib\Mingw\64\bin"

for /f %%i in ('"%qmakeCmd%" -query QT_INSTALL_BINS') do set "qtbinDIR=%%i"
for /f %%i in ('"%qmakeCmd%" -query QT_INSTALL_PLUGINS') do set "qtpluginsDIR=%%i"

set "qtbinDIR=%qtbinDIR:/=\%"
set "qtpluginsDIR=%qtpluginsDIR:/=\%"

REM --- fresh deploy folder ---
rd /q /s "%deployDIR%" 2>nul
md "%deployDIR%"
md "%deployDIR%\doc"
md "%deployDIR%\platforms"
md "%deployDIR%\styles"
md "%deployDIR%\imageformats"
md "%deployDIR%\plugins"

REM --- app + config/doc ---
xcopy "%buildDIR%\release\neuTube.exe" "%deployDIR%\" /y >nul
xcopy "%buildDIR%\debug\neuTube_d.exe" "%deployDIR%\" /y >nul
xcopy "%currDIR%\config.xml" "%deployDIR%\" /y >nul
xcopy "%currDIR%\doc\ThirdPartyLibraries.txt" "%deployDIR%\doc\" /y >nul

REM --- copy entire Qt plugin tree as optional extras (not required by Qt loader)
xcopy "%qtpluginsDIR%" "%deployDIR%\plugins\" /i /e /y >nul

REM --- platform plugin expected by Qt loader ---
if exist "%qtpluginsDIR%\platforms\qwindows.dll" (
  xcopy "%qtpluginsDIR%\platforms\qwindows.dll" "%deployDIR%\platforms\" /y >nul
)

REM --- style plugins for modern look ---
if exist "%qtpluginsDIR%\styles\qwindowsvistastyle.dll" (
  xcopy "%qtpluginsDIR%\styles\qwindowsvistastyle.dll" "%deployDIR%\styles\" /y >nul
)
if exist "%qtpluginsDIR%\styles\qfusionstyle.dll" (
  xcopy "%qtpluginsDIR%\styles\qfusionstyle.dll" "%deployDIR%\styles\" /y >nul
)

REM --- common image format plugins (png/jpeg/ico/svg/webp...) ---
if exist "%qtpluginsDIR%\imageformats\" (
  call :copy_glob "%qtpluginsDIR%\imageformats" "*.dll" "%deployDIR%\imageformats"
)

REM --- MinGW runtimes (from mingw64\bin that qmake reports) ---
call :copy_one "%qtbinDIR%\libgcc_s_seh-1.dll"   "%deployDIR%"
call :copy_one "%qtbinDIR%\libstdc++-6.dll"      "%deployDIR%"
call :copy_one "%qtbinDIR%\libwinpthread-1.dll"  "%deployDIR%"

REM --- Qt5 DLLs used by neuTube ---
call :copy_one "%qtbinDIR%\Qt5Core.dll"        "%deployDIR%"
call :copy_one "%qtbinDIR%\Qt5Gui.dll"         "%deployDIR%"
call :copy_one "%qtbinDIR%\Qt5Widgets.dll"     "%deployDIR%"
call :copy_one "%qtbinDIR%\Qt5OpenGL.dll"      "%deployDIR%"
call :copy_one "%qtbinDIR%\Qt5Xml.dll"         "%deployDIR%"
call :copy_one "%qtbinDIR%\Qt5Network.dll"     "%deployDIR%"
call :copy_one "%qtbinDIR%\Qt5Concurrent.dll"  "%deployDIR%"

REM --- PCRE2 (dash names on MSYS2) ---
call :copy_glob "%qtbinDIR%" "libpcre2-posix-*.dll" "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libpcre2-8-*.dll"     "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libpcre2-16-*.dll"    "%deployDIR%"

REM --- ICU runtimes commonly required by Qt on MSYS2 ---
call :copy_glob "%qtbinDIR%" "libicudt*.dll" "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libicuuc*.dll" "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libicuin*.dll" "%deployDIR%"

REM --- Other common deps seen in MSYS2 Qt builds ---
call :copy_glob "%qtbinDIR%" "libharfbuzz-*.dll"          "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libfreetype-*.dll"          "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libgraphite2*.dll"          "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libglib-2.0-0.dll"          "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libintl-8.dll"              "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libiconv-2.dll"             "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libbz2-1.dll"               "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libmd4c.dll"                "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libzstd.dll"                "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libpng16-*.dll"             "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libbrotlidec*.dll"          "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libbrotlicommon*.dll"       "%deployDIR%"
call :copy_glob "%qtbinDIR%" "libdouble-conversion*.dll"  "%deployDIR%"

REM --- Your bundled third-party DLLs (if you ship them locally) ---
call :copy_one "%libDIR%\libfftw3-3.dll"  "%deployDIR%"
call :copy_one "%libDIR%\libfftw3f-3.dll" "%deployDIR%"
call :copy_one "%libDIR%\libxml2.dll"     "%deployDIR%"
call :copy_one "%libDIR%\libzlib.dll"     "%deployDIR%"

REM --- Fallbacks if using MSYS2 system zlib/libxml2 instead of your own ---
if not exist "%deployDIR%\libxml2.dll" (
  call :copy_glob "%qtbinDIR%" "libxml2-*.dll"  "%deployDIR%"
  call :copy_glob "%qtbinDIR%" "liblzma-*.dll"  "%deployDIR%"
  call :copy_one  "%qtbinDIR%\libcharset-1.dll" "%deployDIR%"
  call :copy_one  "%qtbinDIR%\libiconv-2.dll"   "%deployDIR%"
)
if not exist "%deployDIR%\libzlib.dll" (
  call :copy_one "%qtbinDIR%\zlib1.dll" "%deployDIR%"
)

REM --- prune debug-suffixed Qt5 plugins if any slipped in ---
for %%D in ("%deployDIR%\plugins" "%deployDIR%\platforms" "%deployDIR%\styles" "%deployDIR%\imageformats") do (
  if exist "%%~D" (
    pushd "%%~D" >nul
    del /F /S /Q *d.dll 2>nul
    popd >nul
  )
)

echo(
echo Deployment created at:
echo   %deployDIR%
echo(

endlocal
exit /b


REM === helpers ===
:copy_one
REM %1 = fullpath-to-file, %2 = dest-dir
if exist "%~1" (
  xcopy "%~1" "%~2\" /y >nul
)
exit /b

:copy_glob
REM %1 = src-dir, %2 = pattern, %3 = dest-dir
for /f "delims=" %%F in ('dir /b /a:-d "%~1\%~2" 2^>nul') do (
  xcopy "%~1\%%F" "%~3\" /y >nul
)
exit /b
