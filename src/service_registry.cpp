#include <thectci/service_registry.hpp>
#include <memory>

namespace
{
  std::unique_ptr< the::ctci::ServiceRegistry > service_registry_instance;
}

namespace the
{
namespace ctci
{

ServiceRegistry&
ServiceRegistry::instance()
{
  if ( !service_registry_instance )
  {
    service_registry_instance.reset( new ServiceRegistry() );
  }

  return *service_registry_instance;
}


}
}

