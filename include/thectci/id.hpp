#pragma once

#include <thectci/hash.hpp>

namespace the
{
  namespace ctci
  {
    typedef uint32_t Id;
  }
}

#define add_ctci( string ) enum : the::ctci::Id { ctci = the::ctci::hash( string ) };
#define add_polymorphic_ctci( string ) add_ctci( string ); virtual the::ctci::Id polymorphic_ctci() const { return ctci; };
#define add_pure_polymorphic_ctci() virtual the::ctci::Id polymorphic_ctci() const = 0;

