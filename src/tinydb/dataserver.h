
#ifndef __SRC_TINYDB_TINYDB_H__
#define __SRC_TINYDB_TINYDB_H__

#include <pthread.h>

#include "base.h"

#include "utils/thread.h"
#include "utils/singleton.h"

#include "status.h"

namespace Datad
{

class CDataService;
class CacheMessage;
class LevelDBEngine;

class CDataServer : public utils::IThread, public Singleton<CDataServer>
{
public :
    enum
    {
        eDataService_ThreadsCount   = 1,            // DataService的线程个数
        eDataService_SessionsCount  = 1000,         // DataService最大会话个数
    };

public :
    CDataServer();
    virtual ~CDataServer();

public :
    virtual bool onStart();
    virtual void onExecute();
    virtual void onStop();

public :
    // 提交
    void post( CacheMessage * msg );

    // 获取数据服务
    CDataService * getService() const { return m_DataService; }

    // 获取存档服务
    LevelDBEngine * getStorageEngine() const { return m_StorageEngine; }

private :
    void dispatch();
    void process( CacheMessage * msg );

private :
    void add( CacheMessage * msg );
    void set( CacheMessage * msg );
    void del( CacheMessage * msg );
    void gets( CacheMessage * msg );
    void calc( CacheMessage * msg, int32_t value );

    void stat( CacheMessage * msg );
    void error( CacheMessage * msg );
    void version( CacheMessage * msg );

private :
    void dump( CacheMessage * msg );

private :
    typedef std::deque<CacheMessage*> TaskQueue;

private :
    CDataService *              m_DataService;

    TaskQueue                   m_TaskQueue;
    pthread_mutex_t             m_QueueLock;

    pthread_t                   m_DumpThread;       // 存档线程
    ServerStatus                m_ServerStatus;
    LevelDBEngine *             m_StorageEngine;
};


}

#endif
