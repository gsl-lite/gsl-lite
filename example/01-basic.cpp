
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

    void work() { non_owned_ptr = use( make_not_null( owned_ptr ) ); }

#if gsl_HAVE( ALIAS_TEMPLATE )
    owner<int *> owned_ptr;	// if alias template support
#else // ! gsl_HAVE( ALIAS_TEMPLATE )
    int * owned_ptr;	// otherwise
#endif // gsl_HAVE( ALIAS_TEMPLATE )

    int * non_owned_ptr;
};

int main()
{
    Widget w;
    w.work();
}
