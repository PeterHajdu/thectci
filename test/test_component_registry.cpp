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
    add_polymorphic_ctci( "a" );
};

class ChildOfA : public a
{
  public:
    add_polymorphic_ctci( "child_of_a" );
};

class b
{
  public:
    add_ctci( "b" );
};

class class_never_registered
{
  public:
    add_ctci( "class_never_registered" );
};

}

Describe( a_component_registry )
{
  void SetUp()
  {
    registry.reset( new the::ctci::ComponentRegistry() );
    registry->register_component( a_component );
    registry->register_component( b_component );
  }

  template < class Component >
  void make_sure_that_component_is_registered( Component& component )
  {
    Component& retrieved_component( registry->component<Component>() );
    AssertThat( &retrieved_component, Equals( &component ) );
  }

  It( accepts_objects_with_ctci_as_components )
  {
    make_sure_that_component_is_registered( a_component );
    make_sure_that_component_is_registered( b_component );
  }

  It( can_check_if_a_component_is_registered_or_not )
  {
    AssertThat( registry->has_component< a >(), Equals( true ) );
    AssertThat( registry->has_component< class_never_registered >(), Equals( false ) );
  }

  It( can_register_components_by_polymorphic_ctci )
  {
    ChildOfA child_of_component_a;
    a& base_referenced_component( child_of_component_a );
    registry->register_polymorphic_component( base_referenced_component );
    make_sure_that_component_is_registered( child_of_component_a );
  }

  It( can_update_a_component_to_a_new_value )
  {
    a another_a_component;
    registry->register_component( another_a_component );
    AssertThat( &registry->component< a >(), Equals( &another_a_component ) );
  }

  std::unique_ptr< the::ctci::ComponentRegistry > registry;
  a a_component;
  b b_component;
};

