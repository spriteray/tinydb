
#ifndef __SRC_TINYDB_BDBENGINE_H__
#define __SRC_TINYDB_BDBENGINE_H__

#include "storage.h"

using namespace Storage;

namespace Datad
{

class CBdbEngine : public IStorageEngine
{
public :
    CBdbEngine();
    virtual ~CBdbEngine();

    virtual bool start( const char * tag );
    virtual void stop();

    virtual bool add( const Key & key, const Value & value );
    virtual bool set( const Key & key, const Value & value );
    virtual bool get( const Key & key, Value & value );
    virtual bool del( const Key & key );
};

}

#endif
