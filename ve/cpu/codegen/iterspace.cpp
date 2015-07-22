#include <sstream>
#include <string>
#include "codegen.hpp"

using namespace std;
using namespace bohrium::core;

namespace bohrium{
namespace engine{
namespace cpu{
namespace codegen{

Iterspace::Iterspace(iterspace_t& iterspace) : iterspace_(iterspace) {}

string Iterspace::name(void)
{
    stringstream ss;
    ss << "iterspace";
    return ss.str();
}

string Iterspace::layout(void)
{
    stringstream ss;
    ss << name() << "_layout";
    return ss.str();
}

string Iterspace::ndim(void)
{
    stringstream ss;
    ss << name() << "_ndim";
    return ss.str();
}

string Iterspace::shape(void)
{
    stringstream ss;
    ss << name() << "_shape";
    return ss.str();
}

string Iterspace::shape(uint32_t dim)
{
    stringstream ss;
    ss << _index(shape(), dim);
    return ss.str();
}

string Iterspace::nelem(void)
{
    stringstream ss;
    ss << name() << "_nelem";
    return ss.str();
}

iterspace_t& Iterspace::meta(void)
{
    return iterspace_;
}

}}}}
