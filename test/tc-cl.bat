@echo off & setlocal
set std=%1
if "%std%"=="" set std=c++14

set clang=clang-cl

::set san=-fsanitize=address

"%clang%" -Werror=unknown-argument -std:%std% -EHsc -O2 -W3 -m32 %san% -fms-compatibility-version=19.00 -imsvc "%VCInstallDir%include" -imsvc "%WindowsSdkDir_71A%include" -I../include gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp  && gsl-lite.t.exe

endlocal
