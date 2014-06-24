#include <thectci/compile_time_value.hpp>

#include <igloo/igloo_alt.h>
using namespace igloo;

Describe( a_compile_time_value )
{
  template < uint32_t val >
  void check_that_values_match()
  {
    static_assert(
        the::ctci::compile_time< uint32_t, val >::value == val,
        "Compile time value does not match original value." );
  }

  It( makes_sure_a_value_is_generated_during_compilation )
  {
    check_that_values_match< 10u >();
    check_that_values_match< 11u >();
  }
};

