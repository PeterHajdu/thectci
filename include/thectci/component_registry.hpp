#pragma once

#include <thectci/id.hpp>
#include <memory>
#include <unordered_map>
#include <cassert>

namespace the
{

namespace ctci
{

class BaseComponentHolder
{
  public:
    typedef std::unique_ptr< BaseComponentHolder > Pointer;
    virtual ~BaseComponentHolder() = default;
};

template < typename Component >
class ComponentHolder final : public BaseComponentHolder
{
  public:
    ComponentHolder( Component& component )
      : reference( component )
    {
    }

    virtual ~ComponentHolder() = default;

    Component & reference;
};

class ComponentRegistry
{
  public:
    template < typename Component >
    void register_component( Component& );

    template < typename Component >
    void register_polymorphic_component( Component& );

    template < typename Component >
    Component& component() const;

    template < typename Component >
    bool has_component() const;

  private:
    template < typename Component >
    void register_component_with_id( the::ctci::Id, Component& );

    std::unordered_map< the::ctci::Id, BaseComponentHolder::Pointer > m_components;
};

template < typename Component >
void
ComponentRegistry::register_component( Component& component )
{
  register_component_with_id( component.ctci, component );
}

template < typename Component >
void
ComponentRegistry::register_polymorphic_component( Component& component )
{
  register_component_with_id( component.polymorphic_ctci(), component );
}

template < typename Component >
void
ComponentRegistry::register_component_with_id( the::ctci::Id id, Component& component )
{
  m_components[ id ] = std::make_unique< ComponentHolder< Component > >( component );
}

template < typename Component >
Component&
ComponentRegistry::component() const
{
  const auto component_iterator( m_components.find( Component::ctci ) );
  assert( component_iterator!=m_components.end() );
  ComponentHolder<Component>& component_holder(
      static_cast< ComponentHolder<Component>& >( *component_iterator->second ) );
  return component_holder.reference;
}

template < typename Component >
bool
ComponentRegistry::has_component() const
{
  return m_components.find( Component::ctci )!=m_components.end();
}

}

}

