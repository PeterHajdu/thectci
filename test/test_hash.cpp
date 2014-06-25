#include <thectci/hash.hpp>
#include <thectci/compile_time_value.hpp>

#include <igloo/igloo_alt.h>
using namespace igloo;

Describe( a_hash )
{
  It( is_a_compile_time_hash )
  {
    typedef the::ctci::compile_time< uint32_t, the::ctci::hash( "dogfood" ) > hash_type;
    hash_type unused_variable;
    (void)unused_variable;
  }

  It( is_the_djb2_hash_algorithm )
  {
    constexpr uint32_t test_hash(
        the::ctci::compile_time< uint32_t, the::ctci::hash( "dogfood" ) >::value );

    const uint32_t djb2_hash( 535668743 );

    static_assert( test_hash == djb2_hash, "hash value does not match" );
  }

};

