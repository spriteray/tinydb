
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "version.h"

#include "utils/utility.h"
#include "utils/timeutils.h"

#include "config.h"
#include "message.h"
#include "dataservice.h"
#include "dumpbackend.h"
#include "dataserver.h"

#include "leveldbengine.h"

namespace Datad
{

struct LeveldbFetcher
{
    LeveldbFetcher( std::string & data ) : response(data) {}
    ~LeveldbFetcher() {}

    bool operator () ( const std::string & key, const std::string & value )
    {
        std::string prefix;
        utils::Utility::snprintf( prefix, key.size()+512,
                "VALUE %s 0 %d\r\n", key.c_str(), value.size() );
        response += prefix;
        response += value;
        response += "\r\n";
        return true;
    }

    std::string &   response;
};

CDataServer::CDataServer()
    : m_DataService( NULL ),
      m_DumpThread( 0 ),
      m_StorageEngine( NULL )
{
    pthread_mutex_init( &m_QueueLock, NULL );
}

CDataServer::~CDataServer()
{
    pthread_mutex_destroy( &m_QueueLock );
}

bool CDataServer::onStart()
{
    // 读取配置
    uint16_t port = CDatadConfig::getInstance().getListenPort();
    const char * host = CDatadConfig::getInstance().getBindHost();

    // 初始化数据库
    m_StorageEngine = new LevelDBEngine( CDatadConfig::getInstance().getStorageLocation() );
    if ( m_StorageEngine == NULL )
    {
        return false;
    }

    m_StorageEngine->setCacheSize( CDatadConfig::getInstance().getCacheSize() );

    if ( !m_StorageEngine->initialize() )
    {
        return false;
    }

    // DataService
    m_DataService = new CDataService(
            eDataService_ThreadsCount,
            eDataService_SessionsCount );
    if ( m_DataService == NULL )
    {
        return false;
    }

    if ( !m_DataService->listen( host, port ) )
    {
        LOG_FATAL( "CDataService(%d, %d) listen (%s::%d) failure .\n",
                eDataService_ThreadsCount, eDataService_SessionsCount, host, port );
        return false;
    }

    LOG_INFO( "CDataService(%d, %d) listen (%s::%d) succeed .\n",
           eDataService_ThreadsCount, eDataService_SessionsCount, host, port );

    return true;
}

void CDataServer::onExecute()
{
    this->dispatch();
    utils::TimeUtils::sleep( 10 );
}

void CDataServer::onStop()
{
    // 处理数据
    this->dispatch();

    if ( m_DataService != NULL )
    {
        m_DataService->stop();
        delete m_DataService;
        m_DataService = NULL;
    }

    if ( m_StorageEngine != NULL )
    {
        m_StorageEngine->finalize();
        delete m_StorageEngine;
        m_StorageEngine = NULL;
    }

    LOG_INFO( "CDataServer Stoped .\n" );
}

void CDataServer::post( CacheMessage * msg )
{
    if ( isRunning() )
    {
        pthread_mutex_lock( &m_QueueLock );
        m_TaskQueue.push_back( msg );
        pthread_mutex_unlock( &m_QueueLock );
    }
    else
    {
        LOG_ERROR( "CDataServer is not Running, post a message failed .\n" );
        delete msg;
    }
}

void CDataServer::dispatch()
{
    TaskQueue swapqueue;

    pthread_mutex_lock( &m_QueueLock );
    std::swap( swapqueue, m_TaskQueue );
    pthread_mutex_unlock( &m_QueueLock );

    for ( TaskQueue::iterator iter = swapqueue.begin(); iter != swapqueue.end(); ++iter )
    {
        this->process( *iter );
        delete (*iter);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static const char * MEMCACHED_RESPONSE_STORED       = "STORED\r\n";
static const char * MEMCACHED_RESPONSE_NOT_STORED   = "NOT_STORED\r\n";
static const char * MEMCACHED_RESPONSE_UNKNOWN      = "ERROR unknown command:";
static const char * MEMCACHED_RESPONSE_VALUES_END   = "END\r\n";
static const char * MEMCACHED_RESPONSE_DELETED      = "DELETED\r\n";
static const char * MEMCACHED_RESPONSE_NOT_FOUND    = "NOT_FOUND\r\n";
static const char * MEMCACHED_RESPONSE_VERSION      = "VERSION";
static const char * MEMCACHED_RESPONSE_ERROR        = "ERROR\r\n";
static const char * MEMCACHED_RESPONSE_CLIENTERROR  = "CLIENT_ERROR";
static const char * MEMCACHED_RESPONSE_SERVERERROR  = "SERVER_ERROR";

void CDataServer::process( CacheMessage * message )
{
    // TODO: spriteray, 确保消息中的key确实是在本线程中处理
    // CDataServer::getShardIndex( const char * key )

    if ( message->getItem() != NULL )
    {
        if ( message->isCommand( "add" ) )
        {
            this->add( message );
        }
        else if ( message->isCommand( "set" ) )
        {
            this->set( message );
        }
        else if ( message->isCommand( "delete" ) )
        {
            this->del( message );
        }
        else if ( message->isCommand( "incr" ) )
        {
            this->calc( message, 1 );
        }
        else if ( message->isCommand( "decr" ) )
        {
            this->calc( message, -1 );
        }
        // TODO: 增加memcache协议
        else
        {
            this->error( message );
        }
    }
    else
    {
        if ( message->isCommand( "get" ) || message->isCommand( "gets" ) )
        {
            this->gets( message );
        }
        else if ( message->isCommand( "version" ) )
        {
            this->version( message );
        }
        else if ( message->isCommand( "stats" ) )
        {
            this->stat( message );
        }
        else if ( message->isCommand( "dump" ) )
        {
            this->dump( message );
        }
        // TODO: 增加memcache协议
        else
        {
            this->error( message );
        }
    }

}

void CDataServer::add( CacheMessage * message )
{
    bool rc = false;

    rc = m_StorageEngine->add(
            message->getItem()->getKey(), message->getItem()->getValue() );
    if ( rc )
    {
        m_DataService->send( message->getSid(),
                MEMCACHED_RESPONSE_STORED, strlen(MEMCACHED_RESPONSE_STORED) );
    }
    else
    {
        m_DataService->send( message->getSid(),
                MEMCACHED_RESPONSE_NOT_STORED, strlen(MEMCACHED_RESPONSE_NOT_STORED) );
        LOG_ERROR( "CDataServer::add(KEY:'%s') failed .\n", message->getItem()->getKey().c_str() );
    }
}

void CDataServer::set( CacheMessage * message )
{
    bool rc = false;

    rc = m_StorageEngine->set(
            message->getItem()->getKey(), message->getItem()->getValue() );
    if ( rc )
    {
        m_DataService->send( message->getSid(),
                MEMCACHED_RESPONSE_STORED, strlen(MEMCACHED_RESPONSE_STORED) );
    }
    else
    {
        m_DataService->send( message->getSid(),
                MEMCACHED_RESPONSE_NOT_STORED, strlen(MEMCACHED_RESPONSE_NOT_STORED) );
        LOG_ERROR( "CDataServer::set(KEY:'%s') failed .\n", message->getItem()->getKey().c_str() );
    }

    m_ServerStatus.addSetOps();
}

void CDataServer::del( CacheMessage * message )
{
    bool rc = false;

    rc = m_StorageEngine->del( message->getItem()->getKey() );
    if ( rc )
    {
        m_DataService->send( message->getSid(),
                MEMCACHED_RESPONSE_DELETED, strlen(MEMCACHED_RESPONSE_DELETED) );
    }
    else
    {
        m_DataService->send( message->getSid(),
                MEMCACHED_RESPONSE_NOT_FOUND, strlen(MEMCACHED_RESPONSE_NOT_FOUND) );
        LOG_ERROR( "CDataServer::del(KEY:'%s') failed .\n", message->getItem()->getKey().c_str() );
    }
}

void CDataServer::gets( CacheMessage * message )
{
    std::string response;
    CacheMessage::Keys::iterator iter;

    for ( iter = message->getKeyList().begin(); iter != message->getKeyList().end(); ++iter )
    {
        int32_t npos = (*iter).find( "*" );
        if ( npos != -1 )
        {
            std::string prefix = (*iter).substr( 0, npos );

            LeveldbFetcher fetcher( response );
            m_StorageEngine->foreach( prefix, fetcher );

            continue;
        }

        Value value;
        bool rc = m_StorageEngine->get( *iter, value );
        if ( rc )
        {
            std::string prefix;
            utils::Utility::snprintf( prefix, (*iter).size()+512,
                    "VALUE %s 0 %d\r\n", (*iter).c_str(), value.size() );
            response += prefix;
            response += value;
            response += "\r\n";
        }

        m_ServerStatus.addGetOps();
    }

    response += MEMCACHED_RESPONSE_VALUES_END;
    m_DataService->send( message->getSid(), response );
}

void CDataServer::stat( CacheMessage * message )
{
    char data[ 512 ];
    std::string response;
    uint64_t sec = 0, usec = 0;

    m_ServerStatus.refresh();

    sprintf( data, "STAT pid %u\r\n", m_ServerStatus.getPid() );
    response += data;

    sprintf( data, "STAT uptime %ld\r\n", m_ServerStatus.getNowTime() - m_ServerStatus.getStartTime() );
    response += data;

    sprintf( data, "STAT time %ld\r\n", m_ServerStatus.getNowTime() );
    response += data;

    m_ServerStatus.getUserUsage( sec, usec );
    sprintf( data, "STAT rusage_user %ld.%06ld\r\n", sec, usec );
    response += data;

    m_ServerStatus.getSystemUsage( sec, usec );
    sprintf( data, "STAT rusage_system %ld.%06ld\r\n", sec, usec );
    response += data;

    response += "STAT curr_items 0\r\n";
    response += "STAT total_items 0\r\n";

    sprintf( data, "STAT cmd_get %ld\r\n", m_ServerStatus.getGetOps() );
    response += data;
    sprintf( data, "STAT cmd_set %ld\r\n", m_ServerStatus.getSetOps() );
    response += data;

    response += "STAT get_hits 0\r\n";
    response += "STAT get_misses 0\r\n";
    response += "END\r\n";

    m_DataService->send( message->getSid(), response );
}

void CDataServer::error( CacheMessage * message )
{
    std::string err = MEMCACHED_RESPONSE_UNKNOWN;
    err += message->getCmd();
    err += "\r\n";

    m_DataService->send( message->getSid(), err );
}

void CDataServer::version( CacheMessage * message )
{
    std::string version = MEMCACHED_RESPONSE_VERSION;

    version += " ";
    version += __APPVERSION__;
    version += "\r\n";

    m_DataService->send( message->getSid(), version );
}

void CDataServer::calc( CacheMessage * message, int32_t value )
{
    Value v;
    bool rc = false;
    char strvalue[ 64 ] = { 0 };

    rc = m_StorageEngine->get( message->getItem()->getKey(), v );
    if ( !rc )
    {
        // 未找到
        m_DataService->send( message->getSid(),
                MEMCACHED_RESPONSE_NOT_FOUND, strlen(MEMCACHED_RESPONSE_NOT_FOUND) );
        return;
    }

    if ( message->getDelta() == 0 )
    {
        strncpy( strvalue, v.c_str(), 63 );
    }
    else
    {
        uint64_t rawvalue = (uint64_t)atoll( v.c_str() );
        int64_t change = value * (int64_t)message->getDelta();

        if ( (change>0 && rawvalue+change<rawvalue)
                || (change<0 && rawvalue<(uint64_t)(~change+1)) )
        {
            std::string err = MEMCACHED_RESPONSE_CLIENTERROR;
            err += " ";
            err += "cannot increment or decrement non-numeric value";
            err += "\r\n";
            m_DataService->send( message->getSid(), err );
            return;
        }

        rawvalue += change;
        sprintf( strvalue, "%lu", rawvalue );

        rc = m_StorageEngine->set( message->getItem()->getKey(), std::string( strvalue ) );
        if ( !rc )
        {
            // 未存档
            m_DataService->send( message->getSid(),
                    MEMCACHED_RESPONSE_ERROR, strlen(MEMCACHED_RESPONSE_ERROR) );
            return;
        }
    }

    std::string response;
    response += strvalue;
    response += "\r\n";
    m_DataService->send( message->getSid(), response );
}

void CDataServer::dump( CacheMessage * message )
{
    std::string response;

    if ( m_DumpThread != 0 && utils::IThread::check( m_DumpThread ) )
    {
        response += MEMCACHED_RESPONSE_SERVERERROR;
        response += " ";
        response += "the dump of the thread already exists";
        response += "\r\n";
        m_DataService->send( message->getSid(), response );
        return;
    }

    DumpThreadArgs * args = new DumpThreadArgs;
    args->server = this;
    args->sid = message->getSid();

    if ( pthread_create( &m_DumpThread, NULL, Datad::dump_backend, args ) != 0 )
    {
        response += MEMCACHED_RESPONSE_SERVERERROR;
        response += " ";
        response += "create the dump of the thread failed";
        m_DataService->send( message->getSid(), response );
        delete args;
    }
}

}
