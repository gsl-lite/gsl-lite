cl -EHsc -I../include/gsl -Dgsl_CONFIG_THROWS_FOR_TESTING gsl-lite.t.cpp array_view.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp util.t.cpp && gsl-lite.t.exe --pass
