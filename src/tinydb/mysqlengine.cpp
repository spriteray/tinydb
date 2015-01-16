
#include "mysqlengine.h"

namespace Datad
{

CMysqlEngine::CMysqlEngine()
{}

CMysqlEngine::~CMysqlEngine()
{}

bool CMysqlEngine::start( const char * tag )
{
    return true;
}

void CMysqlEngine::stop()
{}

bool CMysqlEngine::add( const Key & key, const Value & value )
{
    return true;
}

bool CMysqlEngine::set( const Key & key, const Value & value )
{
    return true;
}

bool CMysqlEngine::get( const Key & key, Value & value )
{
    return true;
}

bool CMysqlEngine::del( const Key & key )
{
    return true;
}

}
