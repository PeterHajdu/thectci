#include <thectci/multiplexer.hpp>
#include <thectci/dispatcher.hpp>

#include <igloo/igloo.h>
#include <igloo/igloo_alt.h>

using namespace igloo;

namespace
{
  class TestEvent
  {
    public:
      add_polymorphic_ctci( "test_event" );
  };

  class TestEventChild : public TestEvent
  {
    public:
      add_polymorphic_ctci( "test_event_child" );
  };

  class DispatcherBundle
  {
    public:
      DispatcherBundle( the::ctci::Multiplexer& multiplexer )
      {
        multiplexer.register_dispatcher( dispatcher );
        dispatcher.register_listener< TestEvent >(
            [ this ] ( const TestEvent& event )
            {
              last_event_dispatched = &event;
            } );

        dispatcher.register_listener< TestEventChild >(
            [ this ] ( const TestEventChild& event )
            {
              last_child_event_dispatched = &event;
            } );
      }

      the::ctci::Dispatcher dispatcher;
      const TestEvent* last_event_dispatched{ nullptr };
      const TestEvent* last_child_event_dispatched{ nullptr };
  };

  class MultiplexerBundle
  {
    public:
      MultiplexerBundle( the::ctci::Multiplexer& test_multiplexer )
        : dispatcher_bundle( multiplexer )
      {
        test_multiplexer.register_multiplexer( multiplexer );
      }

      the::ctci::Multiplexer multiplexer;
      DispatcherBundle dispatcher_bundle;
  };

}

Describe( a_multiplexer )
{
  void SetUp()
  {
    test_multiplexer.reset( new the::ctci::Multiplexer() );
    dispatchers.clear();
    for ( size_t i( 0 ); i < number_of_dispatchers; ++i )
    {
      dispatchers.emplace_back( new DispatcherBundle( *test_multiplexer ) );
    }

    for ( size_t i( 0 ); i < number_of_multiplexers; ++i )
    {
      multiplexers.emplace_back( new MultiplexerBundle( *test_multiplexer ) );
    }
  }

  bool was_dispatched( const TestEvent& event ) const
  {
    return std::all_of( std::begin( dispatchers ), std::end( dispatchers ),
       [ &event ]( const DispatcherBundlePointer& bundle )
       {
          return bundle->last_event_dispatched == &event;
       } );
  }

  It( dispatches_events_to_all_registered_dispatchers )
  {
    TestEvent event;
    test_multiplexer->dispatch( event );
    AssertThat( was_dispatched( event ), Equals( true ) );
  }

  bool was_dispatched_as_child( const TestEvent& event ) const
  {
    return std::all_of( std::begin( dispatchers ), std::end( dispatchers ),
       [ &event ]( const DispatcherBundlePointer& bundle )
       {
          return bundle->last_child_event_dispatched == &event;
       } );
  }

  It( dispatches_events_to_all_registered_dispatchers_polymorphic )
  {
    TestEventChild event;
    test_multiplexer->polymorphic_dispatch( static_cast< const TestEvent& >( event ) );
    AssertThat( was_dispatched_as_child( event ), Equals( true ) );
  }

  bool was_forwarded( const TestEvent& event ) const
  {
    return std::all_of( std::begin( multiplexers ), std::end( multiplexers ),
       [ &event ]( const MultiplexerBundlePointer& bundle )
       {
          return bundle->dispatcher_bundle.last_event_dispatched == &event;
       } );
  }

  It( forwards_events_to_registered_multiplexers )
  {
    TestEvent event;
    test_multiplexer->dispatch( event );
    AssertThat( was_forwarded( event ), Equals( true ) );
  }

  bool was_forwarded_as_child( const TestEvent& event ) const
  {
    return std::all_of( std::begin( multiplexers ), std::end( multiplexers ),
       [ &event ]( const MultiplexerBundlePointer& bundle )
       {
          return bundle->dispatcher_bundle.last_child_event_dispatched == &event;
       } );
  }

  It( forwards_events_to_registered_multiplexers_polymorphic )
  {
    TestEventChild event;
    test_multiplexer->polymorphic_dispatch( static_cast< const TestEvent& >( event ) );
    AssertThat( was_forwarded_as_child( event ), Equals( true ) );
  }

  const size_t number_of_dispatchers{ 5 };
  const size_t number_of_multiplexers{ 5 };
  typedef std::unique_ptr< the::ctci::Multiplexer > MultiplexerPointer;
  MultiplexerPointer test_multiplexer;

  typedef std::unique_ptr< MultiplexerBundle > MultiplexerBundlePointer;
  typedef std::vector< MultiplexerBundlePointer > MultiplexerBundleContainer;
  MultiplexerBundleContainer multiplexers;

  typedef std::unique_ptr< DispatcherBundle > DispatcherBundlePointer;
  typedef std::vector< DispatcherBundlePointer > DispatcherBundleContainer;
  DispatcherBundleContainer dispatchers;
};

