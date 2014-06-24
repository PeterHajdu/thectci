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
    template < class Event >
    void register_listener( Id class_id, typename Callback< Event >::type listener )
    {
      get_dispatcher_for< Event >( class_id ).register_listener( listener );
    }

    template < typename Event >
    void dispatch( Id class_id, const Event& event )
    {
      auto dispatcher_iterator( m_dispatchers.find( class_id ) );
      if ( m_dispatchers.end() == dispatcher_iterator )
      {
        return;
      }

      static_cast< ExactDispatcher< Event >& >( *( dispatcher_iterator->second ) ).dispatch( event );
    }

  private:
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

        void dispatch( parameter_type event )
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
    ExactDispatcher<Event>& get_dispatcher_for( Id class_id )
    {
      auto dispatcher_iterator( m_dispatchers.find( class_id ) );
      if ( m_dispatchers.end() == dispatcher_iterator )
      {
        auto emplace_return( m_dispatchers.emplace(
              std::make_pair(
                class_id,
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

