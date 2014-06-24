#pragma once

#include <thectci/compile_time_value.hpp>
#include <thectci/hash.hpp>

namespace the
{
  namespace ctci
  {
    typedef uint32_t Id;
  }
}

#define add_ctci( string ) static const the::ctci::Id ctci{ the::ctci::compile_time< the::ctci::Id, the::ctci::hash( string ) >::value };
#define add_polymorphic_ctci( string ) add_ctci( string ); virtual the::ctci::Id polymorphic_ctci() const { return ctci; };

