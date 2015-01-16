
#ifndef __SRC_TINYDB_STORAGE_H__
#define __SRC_TINYDB_STORAGE_H__

#include <string>

// 存储引擎定义
#define DBD_STORAGEENGINE       1
#define LEVELDB_STORAGEENGINE   2
#define MYSQL_STORAGEENGINE     3

// 定义使用leveldb
#ifndef __STORAGEENGINE__
#define __STORAGEENGINE__ LEVELDB_STORAGEENGINE
#endif

namespace Storage
{

typedef std::string Key;
typedef std::string Value;

class IStorageEngine
{
public :
    IStorageEngine() {}
    virtual ~IStorageEngine() {}

public :
    virtual bool start( const char * tag ) = 0;
    virtual void stop() = 0;

public :
    // 增删改
    virtual bool get( const Key & key, Value & value ) = 0;
    virtual bool add( const Key & key, const Value & value ) = 0;
    virtual bool set( const Key & key, const Value & value ) = 0;
    virtual bool del( const Key & key ) = 0;
};

}

#endif
