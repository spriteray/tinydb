
#include "utils/streambuf.h"

#include "leveldbengine.h"

#include "dataserver.h"
#include "dataservice.h"
#include "dumpbackend.h"

namespace tinydb
{

void * dump_backend( void * arg )
{
    DumpThreadArgs * args = (DumpThreadArgs *)arg;
    sid_t sid = args->sid;
    CDataServer * server = args->server;
    LevelDBEngine * engine = args->server->getStorageEngine();

    leveldb::ReadOptions options;
    options.snapshot = engine->getDatabase()->GetSnapshot();

    do
    {
        StreamBuf buf;
        uint32_t count = 0;

        // 迭代器
        leveldb::Iterator * it = engine->getDatabase()->NewIterator( options );
        if ( it == NULL )
        {
            break;
        }

        for ( it->SeekToFirst(); it->Valid(); it->Next() )
        {
            if ( count != 0 && count % 100 == 0 )
            {
                // 发送
                server->getService()->send(
                        sid, buf.data(), buf.length(), true );
                buf.clear();
            }
            else
            {
                std::string key = it->key().ToString();
                std::string value = it->value().ToString();

                // 拼接KEY和VALUE
                buf.encode( key );
                buf.encode( value );
            }

            ++count;
        }

        delete it;
    }
    while ( 0 );

    // 删除镜像
    if ( options.snapshot != NULL )
    {
        engine->getDatabase()->ReleaseSnapshot( options.snapshot );
    }

    delete args;
    return (void *)0;
}

}
