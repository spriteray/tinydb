
#ifndef __SRC_TINYDB_LEVELDBENGINE_H__
#define __SRC_TINYDB_LEVELDBENGINE_H__

#include <leveldb/db.h>
#include <leveldb/env.h>
#include <leveldb/cache.h>
#include <leveldb/options.h>
#include <leveldb/write_batch.h>

namespace Datad
{

typedef std::string Key;
typedef std::string Value;

class LevelDBEngine
{
public :
    LevelDBEngine( const std::string & path );
    ~LevelDBEngine();

public :
    // 设置缓存大小
    bool setCacheSize( size_t capacity );
    // 设置事务回调函数
    void setBatchHandler( leveldb::WriteBatch::Handler * cb );

    // 初始化
    bool initialize();
    // 销毁
    void finalize();

    // 添加
    bool add( const Key & key, const Value & value );
    // 修改
    bool set( const Key & key, const Value & value );
    // 查询
    bool get( const Key & key, Value & value );
    // 删除
    bool del( const Key & key );

    // 开启/提交事务(单位毫秒)
    bool start( int32_t timeout );
    bool commit();
    leveldb::WriteBatch * txn() const { return m_Transaction; }

    // 获取数据库
    leveldb::DB * getDatabase() const { return m_Database; }

    // 检查磁盘容量
    bool check( int32_t threshold ) const;

public :
    // 清空数据库
    void cleandb();

    // 压缩数据库
    void compactdb();

    // 遍历, 支持通配符*
    template<class Fn>
        void foreach( const std::string & prefix, Fn & f )
        {
            std::string mask = prefix;

            if ( !prefix.empty() )
            {
                int32_t pos = prefix.find( "*" );
                if ( pos == -1 )
                {
                    Value value;
                    if ( this->get(prefix, value) )
                    {
                        f( prefix, value );
                    }
                    return;
                }
                mask = prefix.substr( 0, pos );
            }

            leveldb::Iterator * it = m_Database->NewIterator( leveldb::ReadOptions() );
            if ( it == NULL )
            {
                return;
            }

            if ( !mask.empty() )
            {
                it->Seek( mask );
            }
            else
            {
                it->SeekToFirst();
            }
            for ( ; it->Valid(); it->Next() )
            {
                if ( !mask.empty()
                        && it->key().ToString().find(mask) != 0 )
                {
                    continue;
                }

                if ( !f( it->key().ToString(), it->value().ToString() ) )
                {
                    break;
                }
            }
            delete it;
        }

private :
    enum
    {
        eDBOptions_BlockSize        = ( 32 * 1024 ),            // 32K
        eDBOptions_WriteBufferSize  = ( 64 * 1024 * 1024 ),     // 64M
    };

    // 自动提交
    bool autocommit();
    // 磁盘使用率
    int32_t diskusage() const;

private :
    size_t                          m_Capacity;
    std::string                     m_Path;

    leveldb::Cache *                m_Cache;
    leveldb::DB *                   m_Database;

private :
    int64_t                         m_TxnTimestamp;         // 事务超时时间
    leveldb::WriteBatch *           m_Transaction;
    leveldb::WriteBatch::Handler *  m_BatchHandler;
};

}

#endif
