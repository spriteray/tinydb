
#ifndef __SRC_TINYDB_MESSAGE_H__
#define __SRC_TINYDB_MESSAGE_H__

#include <vector>
#include <string>

#include <stdint.h>
#include <pthread.h>

#include "io/io.h"

namespace tinydb
{

class CacheItem
{
public :
    CacheItem() {}
    ~CacheItem() {}

public :
    // Key
    void setKey( const char * key ) { m_Key = key; }
    const std::string & getKey() const { return m_Key; }

    // Value
    void setCapacity( int32_t c ) { m_Value.reserve( c ); }

    const std::string & getValue() const { return m_Value; }
    uint32_t getValueSize() const { return m_Value.size(); }
    uint32_t getValueCapacity() const { return m_Value.capacity(); }

    void appendValue( const std::string & value ) { m_Value += value; }

private :
    std::string     m_Key;
    std::string     m_Value;
};

class CacheMessage
{
public :
    CacheMessage();
    ~CacheMessage();

    typedef std::vector<std::string> Keys;

public :
    bool isComplete();

    sid_t getSid() const { return m_Sid; }
    void setSid( sid_t id ) { m_Sid = id; }

public :
    //
    void setCmd( const char * command );
    bool isCommand( const char * command ) const;
    const char * getCmd() const { return m_Command; }

    // 错误
    void setError( const char * error );
    const char * getError() const { return m_Error; }

    //
    CacheItem * fetchItem();
    CacheItem * getItem() const { return m_Item; }

    void addKey( const char * key );
    Keys & getKeyList() { return m_Keys; }

    //
    uint32_t getDelta() const { return m_Delta; }
    void setDelta( uint32_t delta ) { m_Delta = delta; }

private :
    sid_t       m_Sid;

    char *      m_Error;        // 消息解析出错
    char *      m_Command;      // 命令字

    Keys        m_Keys;
    CacheItem * m_Item;

    uint32_t    m_Delta;
};




}

#endif
