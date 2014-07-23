#pragma once

#include <thectci/dispatcher.hpp>

namespace the
{

namespace ctci
{

class Multiplexer
{
  public:
    virtual ~Multiplexer() = default;

    void register_dispatcher( const Dispatcher& dispatcher )
    {
      m_dispatchers.emplace_back( dispatcher );
    }

    void register_multiplexer( const Multiplexer& multiplexer )
    {
      m_multiplexers.emplace_back( multiplexer );
    }

    template < typename Event >
    void dispatch( const Event& event ) const
    {
      dispatch_on_elements_of( m_dispatchers, event );
      dispatch_on_elements_of( m_multiplexers, event );
    }

    template < typename Event >
    void polymorphic_dispatch( const Event& event ) const
    {
      polymorphic_dispatch_on_elements_of( m_dispatchers, event );
      polymorphic_dispatch_on_elements_of( m_multiplexers, event );
    }

  private:
    template < typename Container, typename Event >
    void dispatch_on_elements_of( const Container& container, const Event& event ) const
    {
      for ( const auto& element : container )
      {
        element.get().dispatch( event );
      }
    }

    template < typename Container, typename Event >
    void polymorphic_dispatch_on_elements_of( const Container& container, const Event& event ) const
    {
      for ( const auto& element : container )
      {
        element.get().polymorphic_dispatch( event );
      }
    }

    std::vector< std::reference_wrapper< const Dispatcher > > m_dispatchers;
    std::vector< std::reference_wrapper< const Multiplexer > > m_multiplexers;
};

}

}

