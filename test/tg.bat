@echo off
REM set FLAG=-Wsign-conversion
REM set FLAG=-Wshadow
set EXTRAFLAGS=%FLAG% -Wpedantic -Wconversion -Wctor-dtor-privacy -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -Wold-style-cast

@setlocal
@set std=%1
@if "%std%"=="" set std=c++11
echo GNUC ISO standard: %std%
g++ -std=%std% -O2 %EXTRAFLAGS% -Wall -Wextra -Wno-unused-parameter -fno-elide-constructors -o gsl-lite.t.exe -I../include/gsl -Dgsl_CONFIG_SPAN_INDEX_TYPE=std::ptrdiff_t -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS  -Dgsl_FEATURE_EXPERIMENTAL_RETURN_GUARD gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe
@endlocal
