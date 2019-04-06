@echo off & setlocal enableextensions enabledelayedexpansion
::
:: t-all.bat - perform tests with varying contract violation response, features, span index type and C++ standard (MSVC).
::

set log=%0.log
echo. > %log%

set /a compiler_version=0
call :CompilerVersion compiler_version

set cvResponses=^
    gsl_CONFIG_CONTRACT_VIOLATION_THROWS ^
    gsl_CONFIG_CONTRACT_VIOLATION_TERMINATES ^
    gsl_CONFIG_CONTRACT_VIOLATION_CALLS_HANDLER

set spanIndexTypes=^
    std::size_t ^
    std::ptrdiff_t

set gslFeatures=^
    "gsl_CONFIG_DEPRECATE_TO_LEVEL=0" ^
    "gsl_CONFIG_DEPRECATE_TO_LEVEL=1" ^
    "gsl_CONFIG_DEPRECATE_TO_LEVEL=2" ^
    "gsl_CONFIG_DEPRECATE_TO_LEVEL=3" ^
    "gsl_CONFIG_DEPRECATE_TO_LEVEL=4" ^
    "gsl_CONFIG_DEPRECATE_TO_LEVEL=5" ^
    "gsl_CONFIG_DEPRECATE_TO_LEVEL=99" ^
    "gsl_CONFIG_NOT_NULL_EXPLICIT_CTOR=1" ^
    "gsl_CONFIG_NOT_NULL_GET_BY_CONST_REF=1" ^
    "gsl_CONFIG_ALLOWS_NONSTRICT_SPAN_COMPARISON=0" ^
    "gsl_CONFIG_ALLOWS_UNCONSTRAINED_SPAN_CONTAINER_CTOR=0"^
    "gsl_FEATURE_WITH_CONTAINER_TO_STD=0" ^
    "gsl_FEATURE_MAKE_SPAN_TO_STD=0" ^
    "gsl_FEATURE_BYTE_SPAN_TO_STD=0" ^
    "gsl_FEATURE_IMPLICIT_MACRO=0" ^
    "gsl_FEATURE_OWNER_MACRO=0" ^
    "gsl_FEATURE_EXPERIMENTAL_RETURN_GUARD=1" 

set cppStandards=^
    c++14 ^
    c++latest

set CppCoreCheckInclude=%VCINSTALLDIR%\Auxiliary\VS\include

call :ForAllCombinations
endlocal & goto :EOF

:: subroutines

:ForAllCombinations
:ForContractViolationResponse
for %%r in ( %cvResponses% ) do (
    call :ForGslFeature -D%%r
)
goto :EOF

:ForGslFeature  contractViolationResponse
for %%i in ( %gslFeatures% ) do (
    call :ForSpanIndexType %* "-D%%i"
)
goto :EOF

:ForSpanIndexType  contractViolationResponse
for %%i in ( %spanIndexTypes% ) do (
    call :ForCppStd %* "-Dgsl_CONFIG_SPAN_INDEX_TYPE=%%i"
)
goto :EOF

:ForCppStd  contractViolationResponse spanIndexSize
if %compiler_version% LSS 14 (
    call :CompileLog %*
) else (
for %%s in ( %cppStandards% ) do (
    call :CompileLog %* -std:%%s
))
goto :EOF

:CompileLog  contractViolationResponse spanIndexSize [CppStd]
echo VC%compiler_version%: %*
call :Compile %* >> %log%  2>&1
if errorlevel 1 (
    less %log% & goto :EOF
) else (
    echo.%* | findstr /C:"THROWS" 1>nul
    if not errorlevel 1 ( gsl-lite.t.exe )
)
goto :EOF

:Compile  contractViolationResponse spanIndexSize [CppStd]
set args=%*
set compile=cl -EHsc -I../include -I"%CppCoreCheckInclude%" %args% gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp
echo %compile% && %compile%
goto :EOF

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
set /a version="version / 100 - 5 - offset"
endlocal & set %1=%version%& goto :EOF
