cl -EHsc -I../include/gsl -Dgsl_CONFIG_THROWS_FOR_TESTING gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp && gsl-lite.t.exe --pass
