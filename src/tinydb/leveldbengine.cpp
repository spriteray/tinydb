
#include <sys/statfs.h>

#include "base.h"
#include "utils/utility.h"
#include "utils/timeutils.h"

#include "leveldbengine.h"

namespace Datad
{

LevelDBEngine::LevelDBEngine( const std::string & location )
    : m_Capacity( 0 ),
      m_Path( location ),
      m_Cache( NULL ),
      m_Database( NULL ),
      m_TxnTimestamp( 0 ),
      m_Transaction( NULL ),
      m_BatchHandler( NULL )
{}

LevelDBEngine::~LevelDBEngine()
{}

bool LevelDBEngine::setCacheSize( size_t capacity )
{
    m_Capacity = capacity;

    if ( capacity == 0 )
    {
        return false;
    }

    // 创建缓存
    m_Cache = leveldb::NewLRUCache( m_Capacity );
    return m_Cache != NULL;
}

void LevelDBEngine::setBatchHandler( leveldb::WriteBatch::Handler * cb )
{
    m_BatchHandler = cb;
}

bool LevelDBEngine::initialize()
{
    // 确保数据库目录存在
    utils::Utility::mkdirp( m_Path.c_str() );

    // DB选项
    leveldb::Options options;
    options.block_cache         = m_Cache;
    options.error_if_exists     = false;
    options.create_if_missing   = true;
    options.block_size          = eDBOptions_BlockSize;
    options.write_buffer_size   = eDBOptions_WriteBufferSize;

    // 打开数据库
    leveldb::Status status = leveldb::DB::Open( options, m_Path, &m_Database );
    if ( !status.ok() )
    {
        this->finalize();
        return false;
    }

    return true;
}

void LevelDBEngine::finalize()
{
    if ( m_Transaction )
    {
        this->commit();
    }

    if ( m_Database )
    {
        delete m_Database;
        m_Database = NULL;
    }

    if ( m_Cache )
    {
        delete m_Cache;
        m_Cache = NULL;
    }
}

bool LevelDBEngine::add( const Key & key, const Value & value )
{
    Value dbvalue;
    leveldb::Slice dbkey( key );

    // 存档到本地
    // TODO: 是否需要查找Batch中是否存在
    leveldb::Status rc = m_Database->Get( leveldb::ReadOptions(), dbkey, &dbvalue );
    if ( !rc.IsNotFound() )
    {
		return false;
    }

    // 确保事务不会过期
    this->autocommit();
    // 存在事务
    if ( m_Transaction != NULL )
    {
        m_Transaction->Put( dbkey, leveldb::Slice(value) );
        return true;
    }

    // 存档
    rc = m_Database->Put( leveldb::WriteOptions(), dbkey, leveldb::Slice(value) );

	return rc.ok();
}

bool LevelDBEngine::set( const Key & key, const Value & value )
{
    const leveldb::Slice dbkey( key );
    const leveldb::Slice dbvalue( value );

    // 确保事务不会过期
    this->autocommit();
    // 存在事务
    if ( m_Transaction != NULL )
    {
        m_Transaction->Put( dbkey, dbvalue );
        return true;
    }

    // 存档
    leveldb::Status rc = m_Database->Put( leveldb::WriteOptions(), dbkey, dbvalue );

    return rc.ok();
}

bool LevelDBEngine::get( const Key & key, Value & value )
{
    const leveldb::Slice dbkey( key );

    leveldb::Status rc = m_Database->Get( leveldb::ReadOptions(), dbkey, &value );

    return rc.ok();
}

bool LevelDBEngine::del( const Key & key )
{
    const leveldb::Slice dbkey( key );

    // 确保事务不会过期
    this->autocommit();
    // 存在事务
    if ( m_Transaction != NULL )
    {
        m_Transaction->Delete( dbkey );
        return true;
    }

    // 存档
    leveldb::Status rc = m_Database->Delete( leveldb::WriteOptions(), dbkey );

    return rc.ok();
}

bool LevelDBEngine::start( int32_t timeout )
{
    if ( m_Transaction )
    {
        this->commit();
    }

    // 创建事务
    m_Transaction = new leveldb::WriteBatch();
    if ( m_Transaction != NULL )
    {
        m_TxnTimestamp = utils::TimeUtils::now() + timeout;
        return true;
    }

    return false;
}

bool LevelDBEngine::commit()
{
    if ( m_Transaction == NULL )
    {
        return false;
    }

    leveldb::Status rc = m_Database->Write( leveldb::WriteOptions(), m_Transaction );
    if ( rc.ok() )
    {
        // 处理函数
        if ( m_BatchHandler != NULL )
        {
            m_Transaction->Iterate( m_BatchHandler );
        }

        delete m_Transaction;
        //
        m_TxnTimestamp = 0;
        m_Transaction = NULL;
    }

    return rc.ok();
}

void LevelDBEngine::cleandb()
{
    leveldb::Iterator * it = m_Database->NewIterator( leveldb::ReadOptions() );
    if ( it == NULL )
    {
        return;
    }

    for ( it->SeekToFirst(); it->Valid(); it->Next() )
    {
        this->del( it->key().ToString() );
    }

    delete it;
}

bool LevelDBEngine::check( int32_t threshold ) const
{
    // 目录未初始化完成
    if ( m_Path.empty() )
    {
        return true;
    }

    return diskusage() >= threshold;
}

void LevelDBEngine::compactdb()
{
    m_Database->CompactRange( NULL, NULL );
}

bool LevelDBEngine::autocommit()
{
    if ( m_Transaction == NULL )
    {
        return false;
    }

    // 为超时
    if ( m_TxnTimestamp > utils::TimeUtils::now() )
    {
        return false;
    }

    return this->commit();
}

int32_t LevelDBEngine::diskusage() const
{
    struct statfs fs;

    if ( statfs( m_Path.c_str(), &fs ) != 0 )
    {
        return 0;
    }

    size_t ntotal = fs.f_bsize * fs.f_blocks;
    size_t navail = fs.f_bsize * fs.f_bavail;

    return (double)navail / (double)ntotal * 100.0f;
}

}
