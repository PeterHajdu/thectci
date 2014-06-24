#include <thectci/id.hpp>

#include <igloo/igloo_alt.h>
using namespace igloo;

namespace
{
  class first_test_class
  {
    public:
      add_ctci( "dogfood" );
  };

}

Describe( a_ctci_id )
{

  template < typename T, uint32_t expected_id >
  void id_member_matches()
  {
    static_assert( T::id == expected_id, "id member does not match djb2 hash" );
  }

  It( can_add_the_strings_djb2_hash_as_id )
  {
    const uint32_t id_of_dogfood{ 535668743 };
    id_member_matches< first_test_class, id_of_dogfood >();
  }

};

