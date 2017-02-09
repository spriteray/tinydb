
#ifndef __SRC_TINYDB_CONFIG_H__
#define __SRC_TINYDB_CONFIG_H__

#include <stdint.h>

#include <vector>
#include <string>

#include "utils/file.h"
#include "utils/singleton.h"

class CDatadConfig : public Singleton<CDatadConfig>
{
public :
    CDatadConfig();
    ~CDatadConfig();

public :
    bool load( const char * path );
    bool reload( const char * path );
    void unload();

public :
    // Core
    // 日志等级
    uint8_t getLogLevel() const { return m_LogLevel; }

    // 监听的端口号
    uint16_t getListenPort() const { return m_Port; }
    const char * getBindHost() const { return m_BindHost.c_str(); }
    uint32_t getTimeoutSeconds() const { return m_Timeoutseconds; }

    // 缓存大小
    size_t getCacheSize() const { return m_CacheSize; }
    const char * getStorageLocation() const { return m_StorageLocation.c_str(); }

private :
    uint8_t         m_LogLevel;

private :
    uint16_t        m_Port;
    std::string     m_BindHost;
    int32_t         m_Timeoutseconds;

private :
    size_t          m_CacheSize;
    std::string     m_StorageLocation;
};

#endif
