
#ifndef __SRC_TINYDB_DATASERVICE_H__
#define __SRC_TINYDB_DATASERVICE_H__

#include "types.h"
#include "io/io.h"

#include "protocol.h"

namespace tinydb
{

class CClientSession : public IIOSession
{
public :
    CClientSession();
    virtual ~CClientSession();

public :
    virtual int32_t onStart();
    virtual int32_t onProcess( const char * buf, uint32_t nbytes );
    virtual int32_t onTimeout();
    virtual int32_t onError( int32_t result );
    virtual void    onShutdown( int32_t way );

private :
    CacheProtocol    m_MsgDecoder;
};

class CDataService : public IIOService
{
public :
    CDataService( uint8_t nthreads, uint32_t nclients );
    virtual ~CDataService();

public :
    virtual IIOSession * onAccept( sid_t id, const char * host, uint16_t port );

public :

};

}

#endif
