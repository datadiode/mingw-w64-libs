@echo off
setlocal

if not defined tag set tag=v8.0.2

FOR /F "delims=" %%P IN (
	'"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere" -latest -property installationPath'
) DO (
	SET VisualStudioInstallationPath=%%P
)

call "C:\D\dmd2vars32.bat"
dmd -mscrtlib=msvcrt buildsdk.d

setlocal
call "%VisualStudioInstallationPath%\VC\Auxiliary\Build\vcvars32.bat"
buildsdk x86 mingw-w64-%tag:v=% lib32
FOR %%A in ("C:\msys64\mingw32\lib\libmsvcrt.a") DO (
	ECHO %%A
	FOR /F %%O IN ('lib.exe /NOLOGO /LIST %%A') DO (
		FOR %%S in (common_a-acrt_iob_func. extra_a-) DO (
			IF ^
			%%O == lib32_libmsvcrt_extra_a-_scprintf.o (ECHO %%O ruled out by exception) ELSE IF ^
			%%O == lib32_libmsvcrt_extra_a-_vscprintf.o (ECHO %%O ruled out by exception) ELSE IF ^
			%%O == lib32_libmsvcrt_extra_a-strtoimax.o (ECHO %%O ruled out by exception) ELSE IF ^
			%%O == lib32_libmsvcrt_extra_a-strtoumax.o (ECHO %%O ruled out by exception) ELSE IF ^
			%%O GTR lib32_libmsvcrt_%%S IF %%O LSS lib32_libmsvcrt_%%SZZZ (
				ECHO %%O
				lib.exe /NOLOGO /EXTRACT:%%O %%A
				link.exe /LIB /NOLOGO lib32\msvcrt.lib %%O
			)
		)
	)
)
endlocal

setlocal
call "%VisualStudioInstallationPath%\VC\Auxiliary\Build\vcvars64.bat"
buildsdk x64 mingw-w64-%tag:v=% lib64
FOR %%A in ("C:\msys64\mingw64\lib\libmsvcrt.a") DO (
	ECHO %%A
	FOR /F %%O IN ('lib.exe /NOLOGO /LIST %%A') DO (
		FOR %%S in (common_a-acrt_iob_func. extra_a-) DO (
			IF ^
			%%O GTR lib64_libmsvcrt_%%S IF %%O LSS lib64_libmsvcrt_%%SZZZ (
				ECHO %%O
				lib.exe /NOLOGO /EXTRACT:%%O %%A
				link.exe /LIB /NOLOGO lib64\msvcrt.lib %%O
			)
		)
	)
)
endlocal

del "%~dp0mingw-w64-libs-%tag%.7z"
"%ProgramFiles%\7-Zip\7z.exe" a %~dp0mingw-w64-libs-%tag%.7z lib32 lib64
del "%~dp0mingw-w64-libs-%tag%.zip"
"%ProgramFiles%\7-Zip\7z.exe" a "%~dp0mingw-w64-libs-%tag%.zip" lib32\* lib64\* mingw-w64-libs-setup.hta
"%ProgramFiles%\7-Zip\7z.exe" rn "%~dp0mingw-w64-libs-%tag%.zip" lib32 dmd2\windows\lib32mscoff\mingw
"%ProgramFiles%\7-Zip\7z.exe" rn "%~dp0mingw-w64-libs-%tag%.zip" lib64 dmd2\windows\lib64\mingw
REM Create the SFX
"%ProgramFiles(x86)%\FreeExtractor\MakeSFX.exe" ^
/zip="mingw-w64-libs-%tag%.zip" ^
/sfx="mingw-w64-libs-%tag%.exe" ^
/title="mingw-w64-libs" ^
/website="https://github.com/datadiode/mingw-w64-libs" ^
/intro="Version: %tag:v=%\n\nFork of https://github.com/ldc-developers/mingw-w64-libs which includes part of the MinGW runtime additions to get the thing work with the OS provided msvcrt.dll." ^
/overwrite ^
/exec="\"$sysdir$\mshta.exe\" \"$targetdir$\mingw-w64-libs-setup.hta?$dialogplacement$\" $cmdline$" ^
/defaultpath="$registry:HKLM\SOFTWARE\DMD@InstallationFolder$"

REM For local testing, create msvcrt05.lib as a copy of msvcrt.lib
copy /y lib32\msvcrt.lib lib32\msvcrt05.lib
copy /y lib64\msvcrt.lib lib64\msvcrt05.lib

endlocal
