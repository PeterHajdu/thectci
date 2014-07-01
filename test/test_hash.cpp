#include <thectci/hash.hpp>

#include <igloo/igloo_alt.h>
using namespace igloo;

namespace
{

template < typename type, type v >
class compile_time
{
  public:
    static const type value{ v };
};

}

Describe( a_hash )
{
  It( is_a_compile_time_hash )
  {
    typedef compile_time< uint32_t, the::ctci::hash( "dogfood" ) > hash_type;
    hash_type unused_variable;
    (void)unused_variable;
  }

  It( is_the_djb2_hash_algorithm )
  {
    constexpr uint32_t test_hash( compile_time< uint32_t, the::ctci::hash( "dogfood" ) >::value );

    const uint32_t djb2_hash( 535668743 );

    static_assert( test_hash == djb2_hash, "hash value does not match" );
  }

};

