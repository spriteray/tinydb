
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "base.h"
#include "config.h"

CDatadConfig::CDatadConfig()
    : m_LogLevel( 0 ),
      m_Port( 0 ),
      m_Timeoutseconds( 0 ),
      m_CacheSize( 0 )
{}

CDatadConfig::~CDatadConfig()
{}

bool CDatadConfig::load( const char * path )
{
    bool rc = false;

    Utils::ConfigFile raw_file( path );
    rc = raw_file.open();
    assert( rc && "CDatadConfig::load() failed" );

    // Core
    raw_file.get( "Core", "loglevel", m_LogLevel );

    // Service
    raw_file.get( "Service", "host", m_BindHost );
    raw_file.get( "Service", "port", m_Port );
    raw_file.get( "Service", "timeout", m_Timeoutseconds );

    // Storage
    raw_file.get( "Storage", "location", m_StorageLocation );
    raw_file.get( "Storage", "cachesize", m_CacheSize );

    LOG_INFO( "CDatadConfig::load('%s') succeed .\n", path );
    raw_file.close();

    return true;
}

bool CDatadConfig::reload( const char * path )
{
    // TODO:
    return true;
}

void CDatadConfig::unload()
{
    m_LogLevel = 0;
    m_Port = 0;
    m_BindHost.clear();
    m_Timeoutseconds = 0;
    m_StorageLocation.clear();
    m_CacheSize = 0;
}
