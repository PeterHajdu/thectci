#pragma once

#include <thectci/id.hpp>

#include <unordered_map>
#include <memory>
#include <vector>
#include <cassert>

namespace the
{
namespace ctci
{

template < typename Event >
struct Callback
{
  typedef const Event& parameter_type;
  typedef std::function< void( parameter_type ) > type;
};

class Dispatcher
{
  public:
    void register_dispatcher( const Dispatcher& sub_dispatcher )
    {
      m_sub_dispatchers.emplace_back( sub_dispatcher );
    }

    void remove_dispatcher( const Dispatcher& sub_dispatcher )
    {
      m_sub_dispatchers.erase(
          std::remove_if( std::begin( m_sub_dispatchers ), std::end( m_sub_dispatchers ),
            [ &sub_dispatcher ]( const std::reference_wrapper< const Dispatcher >& dispatcher_in_container )
            {
            return &sub_dispatcher == &dispatcher_in_container.get();
            } ),
          std::end( m_sub_dispatchers ) );
    }

    template < class Event >
    void register_listener( typename Callback< Event >::type listener )
    {
      get_dispatcher_for< Event >().register_listener( listener );
    }

    template < typename Event >
    void dispatch( const Event& event ) const
    {
      dispatch( Event::ctci, event );
      for ( const auto& sub_dispatcher : m_sub_dispatchers )
      {
        sub_dispatcher.get().dispatch( event );
      }
    }


    template < typename Event >
    void polymorphic_dispatch( const Event& event ) const
    {
      dispatch( event.polymorphic_ctci(), event );
      for ( const auto& sub_dispatcher : m_sub_dispatchers )
      {
        sub_dispatcher.get().polymorphic_dispatch( event );
      }
    }

    virtual ~Dispatcher() = default;
    Dispatcher() = default;
    Dispatcher( const Dispatcher& ) = delete;
    Dispatcher& operator=( const Dispatcher& ) = delete;
  private:
    std::vector< std::reference_wrapper< const Dispatcher > > m_sub_dispatchers;

    template < typename Event >
    void dispatch( Id class_id, const Event& event ) const
    {
      auto dispatcher_iterator( m_dispatchers.find( class_id ) );
      if ( m_dispatchers.end() == dispatcher_iterator )
      {
        return;
      }

      static_cast< ExactDispatcher< Event >& >( *( dispatcher_iterator->second ) ).dispatch( event );
    }

    class DispatcherInterface
    {
      public:
        typedef std::unique_ptr< DispatcherInterface > Pointer;
        virtual ~DispatcherInterface() = default;
    };

    template < class Event >
    class ExactDispatcher : public DispatcherInterface
    {
      public:
        typedef typename Callback<Event>::type callback_type;
        typedef typename Callback<Event>::parameter_type parameter_type;

        void register_listener( callback_type listener )
        {
          m_listeners.emplace_back( listener );
        }

        void dispatch( parameter_type event ) const
        {
          for ( const auto& listener : m_listeners )
          {
            listener( event );
          }
        }

      private:
        std::vector< callback_type > m_listeners;
    };

    template < class Event >
    ExactDispatcher<Event>& get_dispatcher_for()
    {
      auto dispatcher_iterator( m_dispatchers.find( Event::ctci ) );
      if ( m_dispatchers.end() == dispatcher_iterator )
      {
        auto emplace_return( m_dispatchers.emplace(
              std::make_pair(
                Event::ctci,
                DispatcherInterface::Pointer(
                  new ExactDispatcher< Event >() ) ) ) );
        assert( emplace_return.second );
        dispatcher_iterator = emplace_return.first;
      }

      return static_cast< ExactDispatcher< Event >& >( *( dispatcher_iterator->second ) );
    }

    std::unordered_map< Id, DispatcherInterface::Pointer > m_dispatchers;
};
}
}

