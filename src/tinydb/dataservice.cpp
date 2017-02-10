
#include <stdio.h>
#include <string.h>

#include "message.h"
#include "dataserver.h"
#include "dataservice.h"

namespace tinydb
{

CClientSession::CClientSession()
{
    m_MsgDecoder.init();
}

CClientSession::~CClientSession()
{
}

int32_t CClientSession::onStart()
{
    // TODO: @spriteray,
    return 0;
}

int32_t CClientSession::onProcess( const char * buf, uint32_t nbytes )
{
    int32_t length = 0;

    while ( true )
    {
        int32_t nprocess = m_MsgDecoder.decode( buf+length, nbytes-length );
        if ( nprocess == 0 )
        {
            break;
        }

        length += nprocess;

        CacheMessage * msg = m_MsgDecoder.getMessage();
        if ( msg == NULL )
        {
            continue;
        }

        if ( msg->isComplete() )
        {
            if ( msg->getError() != NULL )
            {
                std::string error = msg->getError();
                error += "\r\n";
                this->send( error );
            }
            else
            {
                if ( msg->isCommand( "quit" ) )
                {
                    return -1;
                }

                // 没有出错, 提交给DataThread处理
                msg->setSid( id() );
                CDataServer::getInstance().post( msg );
            }

            m_MsgDecoder.clear();
        }
    }

    return length;
}

int32_t CClientSession::onTimeout()
{
    return -1;
}

int32_t CClientSession::onError( int32_t result )
{
    return -1;
}

void CClientSession::onShutdown( int32_t way )
{}

CDataService::CDataService( uint8_t nthreads, uint32_t nclients )
    : IIOService( nthreads, nclients )
{}

CDataService::~CDataService()
{}

IIOSession * CDataService::onAccept( sid_t id, const char * host, uint16_t port )
{
    return new CClientSession;
}


}
