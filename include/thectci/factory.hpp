#pragma once
#include <thectci/id.hpp>
#include <memory>
#include <cassert>
#include <functional>
#include <unordered_map>
namespace the
{
namespace ctci
{

template < typename Base >
class Creator
{
  public:
    typedef std::unique_ptr< Base > base_pointer;
    virtual ~Creator() {}

    virtual base_pointer create() = 0;
};

template < typename Base, typename Child >
class ExactCreator : public Creator< Base >
{
  public:
    typedef std::unique_ptr< Base > base_pointer;

    base_pointer create() override
    {
      return std::unique_ptr< Base >( new Child() );
    }
};


template < typename Base >
class Factory
{
  public:
    typedef std::unique_ptr< Base > base_pointer;
    typedef Creator< Base > BaseCreator;

    void register_creator( Id class_id, BaseCreator& base_creator )
    {
      assert( m_creators.find( class_id ) == end( m_creators ) );
      m_creators.emplace( std::make_pair( class_id, std::ref( base_creator ) ) );
    }

    base_pointer create( Id class_id ) const
    {
      typename Creators::const_iterator creator_iterator( m_creators.find( class_id ) );
      if ( creator_iterator == end( m_creators ) )
      {
        return base_pointer( nullptr );
      }

      return creator_iterator->second.get().create();
    }

    bool is_registered( Id class_id ) const
    {
      return m_creators.find( class_id ) != m_creators.end();
    }

  private:
    typedef std::reference_wrapper< BaseCreator > CreatorReference;
    typedef std::unordered_map< Id, CreatorReference > Creators;
    Creators m_creators;
};

}
}

