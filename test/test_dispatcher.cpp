#include <thectci/dispatcher.hpp>

#include <igloo/igloo.h>
#include <igloo/igloo_alt.h>

using namespace igloo;

namespace
{
  struct EventA
  {
    add_polymorphic_ctci( "EventA" );
  };

  struct EventB : public EventA
  {
    add_polymorphic_ctci( "EventB" );
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

    test_dispatcher->dispatch( aevent );
    test_dispatcher->dispatch( bevent );
    AssertThat( alistener.dispatched_event, Equals( &aevent ) );
    AssertThat( alistener2.dispatched_event, Equals( &aevent ) );
    AssertThat( blistener.dispatched_event, Equals( &bevent ) );
  }

  It( dispatches_events_by_polymorphic_id )
  {
    AListener alistener;
    register_listener( *test_dispatcher, alistener );
    BListener blistener;
    register_listener( *test_dispatcher, blistener );
    test_dispatcher->polymorphic_dispatch( aevent );
    test_dispatcher->polymorphic_dispatch( static_cast<const EventA& >( bevent ) );
    AssertThat( alistener.dispatched_event, Equals( &aevent ) );
    AssertThat( blistener.dispatched_event, Equals( &bevent ) );
  }

  It( forwards_events_to_subdispatchers )
  {
    const EventA* dispatched_event{ nullptr };
    the::ctci::Dispatcher sub_dispatcher;
    sub_dispatcher.register_listener< EventA >(
        [ &dispatched_event ]( const EventA& event )
        {
          dispatched_event = &event;
        } );
    test_dispatcher->register_dispatcher( sub_dispatcher );
    test_dispatcher->dispatch( aevent );
    AssertThat( dispatched_event, Equals( &aevent ) );
  }

  It( forwards_events_to_subdispatchers_polymorphic )
  {
    const EventB* dispatched_event{ nullptr };
    the::ctci::Dispatcher sub_dispatcher;
    sub_dispatcher.register_listener< EventB >(
        [ &dispatched_event ]( const EventB& event )
        {
          dispatched_event = &event;
        } );

    test_dispatcher->register_dispatcher( sub_dispatcher );
    test_dispatcher->polymorphic_dispatch( static_cast< const EventA& >( bevent ) );
    AssertThat( dispatched_event, Equals( &bevent ) );
  }

  It( sub_dispatchers_can_be_removed )
  {
    const EventA* dispatched_event{ nullptr };
    the::ctci::Dispatcher sub_dispatcher;
    sub_dispatcher.register_listener< EventA >(
        [ &dispatched_event ]( const EventA& event )
        {
          dispatched_event = &event;
        } );
    test_dispatcher->register_dispatcher( sub_dispatcher );
    test_dispatcher->remove_dispatcher( sub_dispatcher );
    test_dispatcher->dispatch( aevent );
    AssertThat( dispatched_event == nullptr, Equals( true ) );
  }

  std::unique_ptr< the::ctci::Dispatcher > test_dispatcher;
  EventA aevent;
  EventB bevent;
};

