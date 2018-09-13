@echo off & setlocal enableextensions enabledelayedexpansion
::
:: tg-all.bat - perform tests with varying contract violation response,
::              span index type and C++ standard (GNUC).
::

set log=%0.log
echo. > %log%

set /a compiler_version=0
call :CompilerVersion compiler_version

set   cppStandards=c++98 c++03 c++11 c++14 c++1z
set    cvResponses=gsl_CONFIG_CONTRACT_VIOLATION_THROWS gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES
set spanIndexTypes=std::size_t std::ptrdiff_t

call :ForAllCombinations
endlocal & goto :EOF

:: subroutines

:ForAllCombinations
:ForCppStd
for %%s in ( %cppStandards% ) do (
    call :ForContractViolationResponse -std=%%s
)
goto :EOF

:ForContractViolationResponse
for %%r in ( %cvResponses% ) do (
    call :ForSpanIndexType %* -D%%r
)
goto :EOF

:ForSpanIndexType CppStd contractViolationResponse
for %%i in ( %spanIndexTypes% ) do (
    call :CompileLog %* "-Dgsl_CONFIG_SPAN_INDEX_TYPE=%%i"
)
goto :EOF

:CompileLog  CppStd contractViolationResponse spanIndexSize
echo g++ %compiler_version%: %*
call :Compile %* >> %log%  2>&1
if errorlevel 1 (
    less %log% & goto :EOF
) else (
    echo.%* | findstr /C:"THROWS" 1>nul
    if not errorlevel 1 ( gsl-lite.t.exe )
)
goto :EOF

:Compile  CppStd contractViolationResponse spanIndexSize
setlocal
REM set FLAG=-Wsign-conversion
REM set FLAG=-Wshadow
set EXTRAFLAGS=%FLAG% -Wpedantic -Wconversion -Wsign-conversion -Wctor-dtor-privacy -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -Wold-style-cast
set compile=g++ %* -O2 %EXTRAFLAGS% -Wall -Wextra -Wno-unused-parameter -fno-elide-constructors -o gsl-lite.t.exe -I../include -Dgsl_FEATURE_EXPERIMENTAL_RETURN_GUARD gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp
echo %compile% && %compile%
endlocal & goto :EOF

:CompilerVersion  version
@echo off & setlocal enableextensions
set tmpprogram=_getcompilerversion.tmp
set tmpsource=%tmpprogram%.c

echo #include ^<stdio.h^>  > %tmpsource%
echo int main(){printf("%%d.%%d.%%d\n",__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__);} >> %tmpsource%

gcc -o %tmpprogram% %tmpsource% >nul
for /f %%x in ('%tmpprogram%') do set version=%%x
del %tmpprogram%.* >nul
endlocal & set %1=%version%& goto :EOF
