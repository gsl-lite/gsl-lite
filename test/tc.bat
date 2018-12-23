@echo off & setlocal enableextensions enabledelayedexpansion
::
:: tc.bat - compile & run tests (clang).
::

:: if no std is given, use c++14

set std=%1
if "%std%"=="" set std=c++14

set clang=clang

call :CompilerVersion version
echo %clang% %version%: %std%

set unit_config=^
    -Dgsl_FEATURE_EXPERIMENTAL_RETURN_GUARD ^
    -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS 

set  optflags=-O2

set warnflags=-Wall -Wextra -Wpedantic -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-missing-noreturn -Wno-documentation-unknown-command -Wno-documentation-deprecated-sync -Wno-documentation -Wno-weak-vtables -Wno-missing-prototypes -Wno-missing-variable-declarations -Wno-exit-time-destructors -Wno-global-constructors

"%clang%" -std=%std% %optflags% %warnflags% %unit_config% -fms-compatibility-version=19.00 -isystem "%VCInstallDir%include" -isystem "%WindowsSdkDir_71A%include" -I../include -o gsl-lite.t.exe gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe
endlocal & goto :EOF

:: subroutines:

:CompilerVersion  version
echo off & setlocal enableextensions
set tmpprogram=_getcompilerversion.tmp
set tmpsource=%tmpprogram%.c

echo #include ^<stdio.h^>     > %tmpsource%
echo int main(){printf("%%d.%%d.%%d\n",__clang_major__,__clang_minor__,__clang_patchlevel__);} >> %tmpsource%

"%clang%" -o %tmpprogram% %tmpsource% >nul
for /f %%x in ('%tmpprogram%') do set version=%%x
del %tmpprogram%.* >nul
endlocal & set %1=%version%& goto :EOF

