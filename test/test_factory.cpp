#include <thectci/factory.hpp>

#include <igloo/igloo_alt.h>
using namespace igloo;

namespace
{
  class foo
  {
    public:
      add_ctci( "foo" );
      virtual ~foo() {}

      virtual const std::string name() const
      {
        return "foo";
      }
  };

  class foo_child : public foo
  {
    public:
      add_ctci( "foo child" );
      virtual const std::string name() const override
      {
        return "foo child";
      }
  };

  class second_foo_child : public foo
  {
    public:
      add_ctci( "second foo child" );
  };

}

Describe( an_exact_creator )
{
  It( creates_objects_referenced_by_base_type )
  {
    the::ctci::ExactCreator< foo, foo_child > creator;
    std::unique_ptr< foo > foochild_instance( creator.create() );
    AssertThat( foochild_instance->name(), Equals( "foo child" ) );
  }

};

Describe( a_factory )
{
  void SetUp()
  {
    test_factory.reset( new TestFactory() );
    test_factory->register_creator( foo_child::ctci, foochild_creator );
    test_factory->register_creator( foo::ctci, foo_creator );
  }

  It( creates_objects_by_class_id )
  {
    std::unique_ptr< foo > foochild_instance( test_factory->create( foo_child::ctci ) );
    AssertThat( foochild_instance->name(), Equals( "foo child" ) );

    std::unique_ptr< foo > foo_instance( test_factory->create( foo::ctci ) );
    AssertThat( foo_instance->name(), Equals( "foo" ) );
  }

  It( returns_nullptr_if_creator_is_not_registered_for_the_class )
  {
    std::unique_ptr< foo > foo_instance( test_factory->create( second_foo_child::ctci ) );
    AssertThat( foo_instance.get(), Equals( static_cast< foo* >( nullptr ) ) );
  }

  typedef the::ctci::Factory< foo > TestFactory;
  std::unique_ptr< TestFactory > test_factory;
  the::ctci::ExactCreator< foo, foo_child > foochild_creator;
  the::ctci::ExactCreator< foo, foo > foo_creator;
};

