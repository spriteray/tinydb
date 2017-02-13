
#ifndef __SRC_TINYDB_PROTOCOL_H__
#define __SRC_TINYDB_PROTOCOL_H__

#include <stdint.h>

class CacheMessage;

namespace tinydb
{
class CacheProtocol
{
public :
    CacheProtocol();
    ~CacheProtocol();

public :
    void init();
    void clear();

    // 获取解析得到的消息
    CacheMessage * getMessage() const;

    // 解析消息
    int32_t decode( const char * buffer, uint32_t nbytes );

private :
    // length - 包括换行符的长度
    char * getline( const char * buffer, uint32_t nbytes, int32_t & length );

private :
    CacheMessage *        m_Message;

};

}

#endif
