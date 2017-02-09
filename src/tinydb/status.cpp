
#include "status.h"

namespace Datad
{

ServerStatus::ServerStatus()
    : m_StartTime( utils::TimeUtils::time() ),
      m_GetOps( 0 ),
      m_SetOps( 0 ),
      m_NowTime( 0ULL )
{}

ServerStatus::~ServerStatus()
{}

void ServerStatus::refresh()
{
    m_NowTime = utils::TimeUtils::time();
    getrusage( RUSAGE_SELF, &m_CpuUsage );
}

void ServerStatus::getUserUsage( uint64_t & sec, uint64_t & usec )
{
    sec = m_CpuUsage.ru_utime.tv_sec;
    usec = m_CpuUsage.ru_utime.tv_usec;
}

void ServerStatus::getSystemUsage( uint64_t & sec, uint64_t & usec )
{
    sec = m_CpuUsage.ru_stime.tv_sec;
    usec = m_CpuUsage.ru_stime.tv_usec;
}

}
