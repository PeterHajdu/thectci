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

  class TestParent
  {
    public:
      add_polymorphic_ctci( "Parent" );
  };

  class TestChild : public TestParent
  {
    public:
      add_polymorphic_ctci( "Child" );
  };

  class PureParent
  {
    public:
      add_pure_polymorphic_ctci();
  };

  class PureParentChild : public PureParent
  {
    public:
      add_polymorphic_ctci( "pure_parent_child" );
  };

}

Describe( a_ctci_id )
{

  template < typename T, uint32_t expected_id >
  void id_member_matches()
  {
    static_assert( T::ctci == expected_id, "id member does not match djb2 hash" );
  }

  It( can_add_the_strings_djb2_hash_as_id )
  {
    const uint32_t id_of_dogfood{ 535668743 };
    id_member_matches< first_test_class, id_of_dogfood >();
  }

  It( has_a_virtual_id_interface_for_polymorphic_type_id )
  {
    TestChild child_instance;
    TestParent& parent_referenc_to_child( child_instance );
    AssertThat( child_instance.polymorphic_ctci(), Equals( parent_referenc_to_child.polymorphic_ctci() ) );
  }

};

Describe( a_pure_polymorphic_ctci )
{
  It( makes_sure_children_have_a_polymorphic_ctci )
  {
    PureParentChild child;
    (void)child;
  }
};

