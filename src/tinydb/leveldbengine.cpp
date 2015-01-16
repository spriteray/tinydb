
#include "base.h"
#include "utils/utility.h"

#include "config.h"
#include "leveldbengine.h"

namespace Datad
{

CLevelDBEngine::CLevelDBEngine()
    :  m_Database(NULL)
{}

CLevelDBEngine::~CLevelDBEngine()
{}

bool CLevelDBEngine::start( const char * tag )
{
    size_t cachesize = CDatadConfig::getInstance().getCacheSize();
    const char * location = CDatadConfig::getInstance().getStorageLocation();

    Utils::Utility::mkdirp( location );

    // 创建cache
    leveldb::Cache * cache = leveldb::NewLRUCache( cachesize );
    assert( cache != NULL );

    // DB选项
    leveldb::Options options;
    options.block_cache         = cache;
    options.create_if_missing   = true;
    options.block_size          = eDBOptions_BlockSize;
    options.write_buffer_size   = eDBOptions_WriteBufferSize;

    // 打开数据库
    leveldb::Status rc = leveldb::DB::Open( options, location, &m_Database );
    assert( rc.ok() );

    LOG_INFO( "CLevelDBEngine::start(cachesize:%ld, location:'%s') .\n", cachesize, location );

    return true;
}

void CLevelDBEngine::stop()
{
    delete m_Database;
}

bool CLevelDBEngine::add( const Key & key, const Value & value )
{
    std::string dbValue;
    leveldb::Slice dbKey( key );

    leveldb::Status rc = m_Database->Get( leveldb::ReadOptions(), dbKey, &dbValue );
    if ( rc.IsNotFound() )
    {
        // 增加
        rc = m_Database->Put( leveldb::WriteOptions(), dbKey, leveldb::Slice(value) );
        return rc.ok();
    }

    return false;
}

bool CLevelDBEngine::set( const Key & key, const Value & value )
{
    const leveldb::Slice dbKey(key);
    const leveldb::Slice dbValue(value);

    leveldb::Status rc = m_Database->Put( leveldb::WriteOptions(), dbKey, dbValue );

    return rc.ok();
}

bool CLevelDBEngine::get( const Key & key, Value & value )
{
    const leveldb::Slice dbKey(key);

    leveldb::Status rc = m_Database->Get( leveldb::ReadOptions(), dbKey, &value );

    return rc.ok();
}

bool CLevelDBEngine::del( const Key & key )
{
    const leveldb::Slice dbKey(key);

    leveldb::Status rc = m_Database->Delete( leveldb::WriteOptions(), dbKey );

    return rc.ok();
}

}
