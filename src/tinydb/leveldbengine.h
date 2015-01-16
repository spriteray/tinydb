
#ifndef __SRC_TINYDB_LEVELDBENGINE_H__
#define __SRC_TINYDB_LEVELDBENGINE_H__

#include "storage.h"

#include "leveldb/db.h"
#include "leveldb/cache.h"
#include "leveldb/options.h"

using namespace Storage;

namespace Datad
{

class CLevelDBEngine : public IStorageEngine
{
public :
    CLevelDBEngine();
    virtual ~CLevelDBEngine();

    virtual bool start( const char * tag );
    virtual void stop();

    virtual bool add( const Key & key, const Value & value );
    virtual bool set( const Key & key, const Value & value );
    virtual bool get( const Key & key, Value & value );
    virtual bool del( const Key & key );

public :
    enum DBOptions
    {
        eDBOptions_BlockSize            = (8 * 1024),            // 8K
        eDBOptions_WriteBufferSize      = (32 * 1024 * 1024),    // 32M
    };

    leveldb::DB * getDatabase() const { return m_Database; }

    template<class Fn>
        void foreach( const std::string & prefix, Fn & f )
        {
            leveldb::Iterator * it = m_Database->NewIterator(leveldb::ReadOptions());
            if ( it == NULL )
            {
                return;
            }

            for ( it->Seek(prefix); it->Valid(); it->Next() )
            {
                if ( it->key().ToString().find(prefix) != 0 )
                {
                    break;
                }

                if ( !f( it->key().ToString(), it->value().ToString() ) )
                {
                    break;
                }
            }

            delete it;
        }


private :
    leveldb::DB *        m_Database;
};

}

#endif
