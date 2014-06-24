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

#define add_ctci( string ) static const the::ctci::Id id{ the::ctci::compile_time< the::ctci::Id, the::ctci::hash( string ) >::value };

