#include <thectci/component_registry.hpp>
#include <thectci/id.hpp>

#include <igloo/igloo.h>
#include <igloo/igloo_alt.h>

using namespace igloo;

namespace
{

class a
{
  public:
    add_ctci( "a" );
};

class b
{
  public:
    add_ctci( "b" );
};

}

Describe( a_component_registry )
{
  void SetUp()
  {
    test_registry.reset( new the::ctci::ComponentRegistry() );
    test_registry->register_component( a_component );
    test_registry->register_component( b_component );
  }

  template < class Component >
  void make_sure_that_component_is_registered( Component& component )
  {
    Component& retrieved_component( test_registry->component<Component>() );
    AssertThat( &retrieved_component, Equals( &component ) );
  }

  It( accepts_objects_with_ctci_as_components )
  {
    make_sure_that_component_is_registered( a_component );
    make_sure_that_component_is_registered( b_component );
  }

  std::unique_ptr< the::ctci::ComponentRegistry > test_registry;
  a a_component;
  b b_component;
};

