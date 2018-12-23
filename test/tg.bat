@echo off & setlocal enableextensions enabledelayedexpansion
::
:: tg.bat - compile & run tests (GNUC).
::

:: if no std is given, use c++11

set std=%1
set args=%2 %3 %4 %5 %6 %7 %8 %9
if "%1" == "" set std=c++11

set gpp=g++

call :CompilerVersion version
echo %gpp% %version%: %std% %args%

set unit_config=^
    -Dgsl_FEATURE_EXPERIMENTAL_RETURN_GUARD ^
    -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS 

set  optflags=-O2

set warnflags=-Wall -Wextra -Wno-unused-parameter -Wundef -Wpedantic -Wconversion -Wsign-conversion -Wctor-dtor-privacy -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -Wold-style-cast

%gpp% -std=%std% %optflags% %warnflags% %unit_config%-fno-elide-constructors -o gsl-lite.t.exe -I../include gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe

endlocal & goto :EOF

:: subroutines:

:CompilerVersion  version
echo off & setlocal enableextensions
set tmpprogram=_getcompilerversion.tmp
set tmpsource=%tmpprogram%.c

echo #include ^<stdio.h^>     > %tmpsource%
echo int main(){printf("%%d.%%d.%%d\n",__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__);} >> %tmpsource%

%gpp% -o %tmpprogram% %tmpsource% >nul
for /f %%x in ('%tmpprogram%') do set version=%%x
del %tmpprogram%.* >nul
endlocal & set %1=%version%& goto :EOF
