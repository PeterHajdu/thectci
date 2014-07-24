#pragma once
#include <thectci/component_registry.hpp>

namespace the
{

namespace ctci
{

class ServiceRegistry
{
  public:
    template < typename T >
    static void register_service( T& service_implementation )
    {
      instance().m_services.register_component< T >( service_implementation );
    }

    template < typename T >
    static T& service()
    {
      return instance().m_services.component< T >();
    }

  private:
    static ServiceRegistry& instance();

    typedef ComponentRegistry Services;
    Services m_services;
};

template < typename ServiceInterface, typename ServiceImplementation >
class AutoServiceRegister
{
  public:
    template < typename... Args >
    AutoServiceRegister( const Args&... args )
      : m_service_instance( args... )
    {
      ServiceRegistry::register_service< ServiceInterface >( m_service_instance );
    }

  private:
    ServiceImplementation m_service_instance;
};

}

}

