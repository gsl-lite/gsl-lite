@echo off & setlocal enableextensions enabledelayedexpansion
::
:: t.bat - compile & run tests (MSVC).
::

set args=%*

call :CompilerVersion version
echo VC%version%: %args%

set unit_config=^
    -Dgsl_FEATURE_EXPERIMENTAL_RETURN_GUARD ^
    -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS 

set msvc_defines=^
    -D_CRT_SECURE_NO_WARNINGS ^
    -D_SCL_SECURE_NO_WARNINGS

set CppCoreCheckInclude=%VCINSTALLDIR%\Auxiliary\VS\include

cl -EHsc %unit_config% %msvc_defines% -I../include -I"%CppCoreCheckInclude%" %args% gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe

endlocal & goto :EOF

:: subroutines:

:CompilerVersion  version
@echo off & setlocal enableextensions
set tmpprogram=_getcompilerversion.tmp
set tmpsource=%tmpprogram%.c

echo #include ^<stdio.h^>                   >%tmpsource%
echo int main(){printf("%%d\n",_MSC_VER);} >>%tmpsource%

cl /nologo %tmpsource% >nul
for /f %%x in ('%tmpprogram%') do set version=%%x
del %tmpprogram%.* >nul
set offset=0
if %version% LSS 1900 set /a offset=1
set /a version="version / 10 - 10 * ( 5 + offset )"
endlocal & set %1=%version%& goto :EOF
