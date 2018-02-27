#include "gsl/gsl-lite.hpp"
#
void use( not_null<int *> p ) {}

int main()
{
    int a = 42;
    use( &a );
}
