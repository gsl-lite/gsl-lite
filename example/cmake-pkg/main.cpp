// Standard Library
#include <iostream>
#include <string>

// Guidelines Support Library (GSL)
#include <gsl/gsl>

void not_null_example( const gsl::not_null<const std::string *> arg )
{
    // NOOP
}

int main()
{
    using UniqueImmutableStringPtr = std::unique_ptr<const std::string>;
    const UniqueImmutableStringPtr text = std::make_unique<std::string>("hello, gsl");

    not_null_example( text.get() );
    //not_null_example(nullptr);

    std::cout << text.get() << '\n';
}
