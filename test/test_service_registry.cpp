#include <thectci/service_registry.hpp>
#include <thectci/id.hpp>

#include <igloo/igloo.h>
#include <igloo/igloo_alt.h>

using namespace igloo;

namespace
{
  class ArbitraryClass
  {
    public:
      add_ctci( "arbitrary_class" );
  };


  class SomeClassInterface
  {
    public:
      add_ctci( "some_class_interface" );
      virtual void stuff() = 0;
  };

  class SomeClass : public SomeClassInterface
  {
    public:
      add_ctci( "some_class" );
      virtual void stuff() override {}
  };

  the::ctci::AutoServiceRegister< SomeClass, SomeClass > some_class_register;
  the::ctci::AutoServiceRegister< SomeClassInterface, SomeClass > some_class_interface_register;

  class ClassWithConstructorParameters
  {
    public:
      add_ctci( "some_class" );
      ClassWithConstructorParameters( int a, std::string b )
        : a( a )
        , b( b )
      {
      }

      const int a;
      const std::string b;
  };

  the::ctci::AutoServiceRegister< ClassWithConstructorParameters, ClassWithConstructorParameters >
    class_with_constructor_parameters_register( 10, "dogfood" );
}

Describe( the_service_registry )
{

  void SetUp()
  {
    the::ctci::ServiceRegistry::register_service< ArbitraryClass >( arbitrary_object );
  }

  It( allows_arbitrary_class_registration )
  {
    ArbitraryClass& retrieved_service( the::ctci::ServiceRegistry::service< ArbitraryClass >() );
    AssertThat( &retrieved_service, Equals( &arbitrary_object ) );
  }

  It( has_a_short_access_function )
  {
    ArbitraryClass& retrieved_service( the::ctci::service< ArbitraryClass >() );
    AssertThat( &retrieved_service, Equals( &arbitrary_object ) );
  }

  ArbitraryClass arbitrary_object;
};

Describe( auto_service_register )
{

  It( registers_a_service_automatically )
  {
    SomeClass& some_class( the::ctci::ServiceRegistry::service< SomeClass >() );
    (void)some_class;
  }

  It( forwards_constructor_parameters )
  {
    ClassWithConstructorParameters& some_class( the::ctci::ServiceRegistry::service< ClassWithConstructorParameters >() );
    (void)some_class;
  }

  It( registers_implementation_of_an_interface )
  {
    SomeClassInterface& some_class( the::ctci::ServiceRegistry::service< SomeClassInterface >() );
    some_class.stuff();
  }

  It( can_return_the_instance_of_the_service )
  {
    SomeClassInterface& some_class( the::ctci::ServiceRegistry::service< SomeClassInterface >() );
    AssertThat( &some_class_interface_register.get(), Equals( &some_class ) );
  }
};

