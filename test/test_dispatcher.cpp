#include <thectci/event.hpp>

#include <igloo/igloo.h>
#include <igloo/igloo_alt.h>

using namespace igloo;

namespace
{
  struct EventA
  {
    add_ctci( "EventA" );
  };

  struct EventB
  {
    add_ctci( "EventB" );
  };

  template < class Event >
  struct ListenerObject
  {
    typedef Event event_type;

    ListenerObject()
      : dispatched_event( nullptr )
    {
    }

    void handle_event( const Event& event )
    {
      assert( dispatched_event == nullptr );
      dispatched_event = &event;
    }

    const Event* dispatched_event;

    ListenerObject( const ListenerObject& ) = delete;
    ListenerObject& operator=( const ListenerObject& ) = delete;
  };

  typedef ListenerObject< EventA > AListener;
  typedef ListenerObject< EventB > BListener;

  template < class Listener >
  void register_listener( the::ctci::Dispatcher& dispatcher, Listener& listener )
  {
    dispatcher.register_listener< typename Listener::event_type >(
        Listener::event_type::ctci,
        std::bind(
          &Listener::handle_event,
          &listener,
          std::placeholders::_1 ) );
  }
}

Describe( a_dispatcher )
{
  void SetUp()
  {
    test_dispatcher.reset( new the::ctci::Dispatcher() );
  }

  Spec( listeners_can_register_by_event_type )
  {
    AListener listener;
    register_listener( *test_dispatcher, listener );
  }


  It( dispatches_events_to_registered_listeners_by_event_type )
  {
    AListener alistener;
    register_listener( *test_dispatcher, alistener );
    AListener alistener2;
    register_listener( *test_dispatcher, alistener2 );
    BListener blistener;
    register_listener( *test_dispatcher, blistener );

    test_dispatcher->dispatch( EventA::ctci, aevent );
    test_dispatcher->dispatch( EventB::ctci, bevent );
    AssertThat( alistener.dispatched_event, Equals( &aevent ) );
    AssertThat( alistener2.dispatched_event, Equals( &aevent ) );
    AssertThat( blistener.dispatched_event, Equals( &bevent ) );
  }

  std::unique_ptr< the::ctci::Dispatcher > test_dispatcher;
  EventA aevent;
  EventB bevent;
};
