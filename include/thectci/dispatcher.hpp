#pragma once

#include <thectci/id.hpp>

#include <unordered_map>
#include <memory>
#include <vector>
#include <cassert>
#include <algorithm>

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
  private:
    using ListenerId = unsigned long long;
    class DispatcherInterface
    {
      public:
        typedef std::unique_ptr< DispatcherInterface > Pointer;
        virtual void deregister_listener( ListenerId ) = 0;
        virtual ~DispatcherInterface() = default;
    };

  public:

    class SmartToken
    {
      public:
        using Pointer = std::unique_ptr< SmartToken >;
        SmartToken( ListenerId id, DispatcherInterface& dispatcher )
          : m_was_released( false )
          , m_id( id )
          , m_dispatcher( dispatcher )
        {
        }

        ~SmartToken()
        {
          deregister_listener();
        }

        SmartToken( const SmartToken& ) = delete;
        SmartToken( SmartToken&& ) = delete;
        SmartToken& operator=( const SmartToken& ) = delete;
        SmartToken& operator=( SmartToken&& ) = delete;

        void release_token()
        {
          m_was_released = true;
        }

      private:
        void deregister_listener()
        {
          if ( m_was_released )
          {
            return;
          }

          m_dispatcher.deregister_listener( m_id );
        }

        bool m_was_released;
        const ListenerId m_id;
        DispatcherInterface& m_dispatcher;
    };

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
      auto token( get_dispatcher_for< Event >().register_listener( std::move( listener ) ) );
      token->release_token();
    }

    template < class Event >
    SmartToken::Pointer register_smart_listener( typename Callback< Event >::type listener )
    {
      return get_dispatcher_for< Event >().register_listener( std::move( listener ) );
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

    void clear()
    {
      m_sub_dispatchers.clear();
      m_dispatchers.clear();
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

    template < class Event >
    class ExactDispatcher : public DispatcherInterface
    {
      public:
        typedef typename Callback<Event>::type callback_type;
        typedef typename Callback<Event>::parameter_type parameter_type;

        SmartToken::Pointer register_listener( callback_type listener )
        {
          m_listeners.emplace( std::make_pair(
                m_last_id,
                std::move( listener ) ) );
          //todo: implement decent id generation
          return std::make_unique< SmartToken >( m_last_id++, *this );
        }

        void dispatch( parameter_type event ) const
        {
          for ( const auto& listener_pair : m_listeners )
          {
            listener_pair.second( event );
          }
        }

        virtual void deregister_listener( ListenerId id ) override
        {
          m_listeners.erase( id );
        }

      private:
        ListenerId m_last_id{ 0u };
        std::unordered_map< ListenerId, callback_type > m_listeners;
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

using SmartListener = Dispatcher::SmartToken::Pointer;

}
}

