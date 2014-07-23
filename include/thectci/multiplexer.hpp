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

    template < typename Event >
    void polymorphic_dispatch( const Event& event ) const
    {
      for ( const auto& dispatcher : m_dispatchers )
      {
        dispatcher.get().polymorphic_dispatch( event );
      }
    }

    template < typename Event >
    void dispatch( const Event& event ) const
    {
      for ( const auto& dispatcher : m_dispatchers )
      {
        dispatcher.get().dispatch( event );
      }
    }

  private:
    std::vector< std::reference_wrapper< const Dispatcher > > m_dispatchers;
};

}

}

