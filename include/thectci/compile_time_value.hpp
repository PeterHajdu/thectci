#pragma once

namespace the
{
namespace ctci
{

template < typename type, type v >
class compile_time
{
  public:
    static const type value{ v };
};

}
}

