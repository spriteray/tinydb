
#include "bdbengine.h"

namespace Datad
{

CBdbEngine::CBdbEngine()
{}

CBdbEngine::~CBdbEngine()
{}

bool CBdbEngine::start( const char * tag )
{
    return true;
}

void CBdbEngine::stop()
{}

bool CBdbEngine::add( const Key & key, const Value & value )
{
    return true;
}

bool CBdbEngine::set( const Key & key, const Value & value )
{
    return true;
}

bool CBdbEngine::get( const Key & key, Value & value )
{
    return true;
}

bool CBdbEngine::del( const Key & key )
{
    return true;
}

}
