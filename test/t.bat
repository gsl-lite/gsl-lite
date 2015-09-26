cl -EHsc -I../include/ -Dgsl_FEATURE_THROW_ON_FAILURE gsl-lite.t.cpp owner.t.cpp not_null.t.cpp && gsl-lite.t.exe --pass
