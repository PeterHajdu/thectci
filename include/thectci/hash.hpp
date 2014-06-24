#pragma once

#include <cstdlib>

namespace the
{
namespace ctci
{

constexpr unsigned int hash_impl( uint32_t hash, const char* string )
{
      return !*string ?
        hash :
        hash_impl( hash * 33 + *string, string + 1 );
}

constexpr uint32_t hash( const char* string )
{
  return !string ?
    0 :
    hash_impl( 5381, string );
}

}
}

