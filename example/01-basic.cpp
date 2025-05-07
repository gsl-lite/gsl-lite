
#include <gsl-lite/gsl-lite.hpp>

using namespace gsl_lite;

int * use( not_null<int *> p ) 
{
    // use p knowing it's not nullptr, NULL or 0.
    
    return p;
}

struct Widget
{
    Widget() : owned_ptr( new int(42) ) {}

    ~Widget() { delete owned_ptr; }

    void work() { non_owned_ptr = use( gsl_lite::make_not_null( owned_ptr ) ); }

    owner<int *> owned_ptr;	// if alias template support
    int * non_owned_ptr;
};

int main()
{
    Widget w;
    w.work();
}
