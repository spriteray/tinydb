
#ifndef __SRC_TINYDB_MYSQLENGINE_H__
#define __SRC_TINYDB_MYSQLENGINE_H__

#include "storage.h"

using namespace Storage;

namespace Datad
{

class CMysqlEngine : public IStorageEngine
{
public :
    CMysqlEngine();
    virtual ~CMysqlEngine();

    virtual bool start( const char * tag );
    virtual void stop();

    virtual bool add( const Key & key, const Value & value );
    virtual bool set( const Key & key, const Value & value );
    virtual bool get( const Key & key, Value & value );
    virtual bool del( const Key & key );
};

}

#endif
