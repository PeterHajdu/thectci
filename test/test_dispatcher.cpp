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
    typedef std::unique_ptr< ListenerObject< Event > > Pointer;

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
    bool was_not_dispatched() const
    {
      return nullptr == dispatched_event;
    }

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
    dispatcher.reset( new the::ctci::Dispatcher() );
    sub_dispatcher.reset( new the::ctci::Dispatcher() );
    dispatcher->register_dispatcher( *sub_dispatcher );

    dispatcher_a_listener.reset( new AListener() );
    dispatcher_b_listener.reset( new BListener() );
    register_listener( *dispatcher, *dispatcher_a_listener );
    register_listener( *dispatcher, *dispatcher_b_listener );

    sub_dispatcher_a_listener.reset( new AListener() );
    sub_dispatcher_b_listener.reset( new BListener() );
    register_listener( *sub_dispatcher, *sub_dispatcher_a_listener );
    register_listener( *sub_dispatcher, *sub_dispatcher_b_listener );
  }

  It( dispatches_events_to_registered_listeners_by_event_type )
  {
    dispatcher->dispatch( aevent );
    dispatcher->dispatch( bevent );
    AssertThat( dispatcher_a_listener->dispatched_event, Equals( &aevent ) );
    AssertThat( dispatcher_b_listener->dispatched_event, Equals( &bevent ) );
  }

  It( dispatches_events_to_all_listeners )
  {
    AListener another_a_listener;
    register_listener( *dispatcher, another_a_listener );

    dispatcher->dispatch( aevent );

    AssertThat( dispatcher_a_listener->dispatched_event, Equals( &aevent ) );
    AssertThat( another_a_listener.dispatched_event, Equals( &aevent ) );
  }

  It( dispatches_events_by_polymorphic_id )
  {
    dispatcher->polymorphic_dispatch( static_cast<const EventA& >( bevent ) );
    AssertThat( dispatcher_b_listener->dispatched_event, Equals( &bevent ) );
  }

  It( forwards_events_to_subdispatchers )
  {
    dispatcher->dispatch( aevent );
    AssertThat( sub_dispatcher_a_listener->dispatched_event, Equals( &aevent ) );
  }

  It( forwards_events_to_subdispatchers_polymorphic )
  {
    dispatcher->polymorphic_dispatch( static_cast< const EventA& >( bevent ) );
    AssertThat( sub_dispatcher_b_listener->dispatched_event, Equals( &bevent ) );
  }

  Spec( sub_dispatchers_can_be_removed )
  {
    dispatcher->remove_dispatcher( *sub_dispatcher );
    dispatcher->dispatch( aevent );
    AssertThat( sub_dispatcher_a_listener->was_not_dispatched(), Equals( true ) );
  }

  Spec( all_listeners_and_sub_dispatchers_can_be_removed )
  {
    dispatcher->clear();
    dispatcher->dispatch( aevent );
    dispatcher->dispatch( bevent );
    AssertThat( sub_dispatcher_a_listener->was_not_dispatched(), Equals( true ) );
    AssertThat( sub_dispatcher_b_listener->was_not_dispatched(), Equals( true ) );
    AssertThat( dispatcher_a_listener->was_not_dispatched(), Equals( true ) );
    AssertThat( dispatcher_b_listener->was_not_dispatched(), Equals( true ) );
  }

  std::unique_ptr< the::ctci::Dispatcher > dispatcher;
  std::unique_ptr< the::ctci::Dispatcher > sub_dispatcher;

  AListener::Pointer dispatcher_a_listener;
  BListener::Pointer dispatcher_b_listener;
  AListener::Pointer sub_dispatcher_a_listener;
  BListener::Pointer sub_dispatcher_b_listener;

  EventA aevent;
  EventB bevent;
};

