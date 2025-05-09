
#include <iostream>

#include <gsl-lite/gsl-lite.hpp>

void printArgs( gsl_lite::span<gsl_lite::zstring const> args )
{
    for ( auto arg : args )
    {
        std::cout << arg << "\n";
    }
}

int main( int argc, char* argv[] )
{
    gsl_Expects( argc > 0 );

    auto args = gsl_lite::span( argv, argc );
    auto argsWithoutExeName = args.subspan( 1 );
    printArgs( argsWithoutExeName );
}
