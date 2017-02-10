
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"

namespace tinydb
{

bool CacheItem::checkDataChunk()
{
    size_t len = m_Value.size();
    if ( len < 2 )
    {
        return false;
    }

    if ( m_Value[len-1] == '\n'
            && m_Value[len-2] == '\r' )
    {
        m_Capacity -= 2;
        m_Value.resize( len-2 );
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CacheMessage::CacheMessage()
    : m_Sid( 0 ),
      m_Error( NULL ),
      m_Command( NULL ),
      m_Item( NULL ),
      m_Delta(0)
{}

CacheMessage::~CacheMessage()
{
    if ( m_Error != NULL )
    {
        ::free( m_Error );
        m_Error = NULL;
    }

    if ( m_Command != NULL )
    {
        ::free( m_Command );
        m_Command = NULL;
    }

    if ( m_Item != NULL )
    {
        delete m_Item;
        m_Item = NULL;
    }
}

bool CacheMessage::isComplete()
{
    if ( m_Error == NULL )
    {
        if ( m_Item != NULL )
        {
            return m_Item->getValueSize() == m_Item->getValueCapacity();
        }
    }

    return true;
}

bool CacheMessage::checkDataChunk()
{
    if ( m_Error == NULL )
    {
        if ( m_Item != NULL )
        {
            return m_Item->checkDataChunk();
        }
    }

    return true;
}

void CacheMessage::setCmd( const char * command )
{
    if ( m_Command != NULL )
    {
        ::free( m_Command );
        m_Command = NULL;
    }

    m_Command = ::strdup( command );
}

bool CacheMessage::isCommand( const char * command ) const
{
    return ( ::strcmp( command, m_Command ) == 0 );
}

void CacheMessage::setError( const char * error )
{
    if ( m_Error != NULL )
    {
        ::free( m_Error );
        m_Error = NULL;
    }

    m_Error = ::strdup( error );
}

CacheItem * CacheMessage::fetchItem()
{
    if ( m_Item == NULL )
    {
        m_Item = new CacheItem;
    }

    return m_Item;
}

void CacheMessage::addKey( const char * key )
{
    m_Keys.push_back( key );
}

}
