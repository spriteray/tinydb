
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "message.h"
#include "protocol.h"

#include "base.h"
#include "version.h"

namespace tinydb
{

CacheProtocol::CacheProtocol()
{}

CacheProtocol::~CacheProtocol()
{}

void CacheProtocol::init()
{
    m_Message = NULL;
}

void CacheProtocol::clear()
{
    m_Message = NULL;
}

CacheMessage * CacheProtocol::getMessage() const
{
    return m_Message;
}

int32_t CacheProtocol::decode( const char * buffer, uint32_t nbytes )
{
    int32_t length = 0;

    if ( m_Message == NULL )
    {
        char * line = getline( buffer, nbytes, length );
        if ( line == NULL )
        {
            return 0;
        }

        char * params = line;
        char * cmd = strsep( &params, " " );

        if ( cmd == NULL )
        {
            return 0;
        }

        m_Message = new CacheMessage;
        m_Message->setCmd( cmd );

        if ( strcasecmp( cmd, "add" ) == 0 || strcasecmp( cmd, "set" ) == 0
                || strcasecmp( cmd, "replace" ) == 0 || strcasecmp( cmd, "cas" ) == 0
                || strcasecmp( cmd, "append" ) == 0 || strcasecmp( cmd, "prepend" ) == 0 )
        {
            // 协议定义
            // [key] [flags] [expire] [bytes] <casunique>\r\n
            // fields说明如下:
            //      0 - key
            //      1 - flags
            //      2 - expire
            //      3 - value size
            //      4 - cas(可选)

            int32_t nfields = 0;
            char * fields[ 5 ] = { 0 };

            const char * sep = " ";
            char * word = NULL, * brkt = NULL;
            for ( word = strtok_r(params, sep, &brkt);
                    word;
                    word = strtok_r(NULL, sep, &brkt) )
            {
                fields[nfields++] = word;
            }

            if ( nfields >= 4
                    && fields[0] != NULL && fields[3] != NULL )
            {
                // key datasize 合法
                m_Message->fetchItem()->setKey( fields[0] );
                m_Message->fetchItem()->setValueCapacity( atoi(fields[3]) + 2 );     // DataChunk\r\n

                if ( fields[ 1 ] != NULL )
                {
                    LOG_WARN( "CacheProtocol::decode(CMD:'%s', KEY:'%s') : the %s-%s not support the Flags feature .\n",
                            cmd, fields[0], __APPNAME__, __APPVERSION__ );
                }
                if ( fields[ 2 ] != NULL )
                {
                    LOG_WARN( "CacheProtocol::decode(CMD:'%s', KEY:'%s') : this %s-%s not support the ExpireTime feature .\n",
                            cmd, fields[0], __APPNAME__, __APPVERSION__ );
                }
                if ( nfields == 5 && fields[4] != NULL )
                {
                    LOG_WARN( "CacheProtocol::decode(CMD:'%s', KEY:'%s') : this %s-%s not support the CasUnique feature .\n",
                            cmd, fields[0], __APPNAME__, __APPVERSION__ );
                }

                // TODO: cas
            }
            else
            {
                m_Message->setError( "CLIENT_ERROR bad command line format" );
            }
        }
        else if ( strcasecmp( cmd, "get" ) == 0 || strcasecmp( cmd, "gets" ) == 0 )
        {
            // [cmd] [key1] [key2] [key3] ... [keyn]

            const char * sep = " ";
            char * word = NULL, *brkt = NULL;
            for ( word = strtok_r(params, sep, &brkt);
                    word;
                    word = strtok_r(NULL, sep, &brkt) )
            {
                m_Message->addKey( word );
            }
        }
        else if ( strcasecmp( cmd, "incr" ) == 0 || strcasecmp( cmd, "decr" ) == 0 )
        {
            char key[ 256 ] = { 0 };
            char bytes[ 16 ] = { 0 };

            int32_t rc = sscanf( params, "%250s %15s", key, bytes );
            if ( rc == 2 && key[0] != '\0' )
            {
                m_Message->fetchItem()->setKey( key );
                m_Message->setDelta( (uint32_t)atoi(bytes) );
            }
            else
            {
                m_Message->setError("CLIENT_ERROR bad command line format");
            }
        }
        else if ( strcasecmp( cmd, "delete" ) == 0 )
        {
            char key[ 256 ] = { 0 };
            char expire[ 16 ] = { 0 };

            sscanf( params, "%250s %15s", key, expire );
            if ( key[0] != '\0' )
            {
                m_Message->fetchItem()->setKey( key );
            }
            else
            {
                m_Message->setError("CLIENT_ERROR bad command line format");
            }
        }

        free ( line );
    }

    if ( m_Message != NULL && m_Message->getError() == NULL && nbytes > (uint32_t)length )
    {
        char * buf = (char *)buffer + length;
        size_t nleft = nbytes - length;
        size_t bytes = m_Message->fetchItem()->getValueCapacity() - m_Message->fetchItem()->getValueSize();

        if ( bytes > 0 )
        {
            bytes = bytes > nleft ? nleft : bytes;
            m_Message->fetchItem()->appendValue( std::string(buf, bytes) );

            length += bytes;

            // 检查DataChunk
            if ( m_Message->isComplete()
                    && !m_Message->checkDataChunk() )
            {
                m_Message->setError("CLIENT_ERROR bad data chunk");
            }
        }
    }

    return length;
}


char * CacheProtocol::getline( const char * buffer, uint32_t nbytes, int32_t & length )
{
    uint32_t len = 0;
    char * line = NULL;

    for ( len = 0; len < nbytes; ++len )
    {
        if ( buffer[ len ] == '\r'
                && buffer[ len+1 ] == '\n' )
        {
            break;
        }
    }

    if ( len == nbytes )
    {
        return NULL;
    }

    line = (char *)::malloc( len + 1 );
    if ( line == NULL )
    {
        return NULL;
    }

    //
    ::memcpy( line, buffer, len );
    //
    length = len + 2;
    line[ len ] = '\0';

    return line;
}

}
