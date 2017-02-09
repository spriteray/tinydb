
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/time.h>

#include "timeutils.h"

namespace global
{
    int32_t delta_timestamp = 0;
}

namespace utils
{

Clock::Clock( const char * now )
    : m_Timestamp( TimeUtils::getTimestamp( now ) )
{}

Clock::Clock( time_t now, uint32_t interval )
    : m_Timestamp( now+interval )
{}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

TimeUtils::TimeUtils()
{
    m_Timestamp = 0;
    std::memset( &m_TimeStruct, 0, sizeof(m_TimeStruct) );
}

TimeUtils::TimeUtils( time_t t )
{
    m_Timestamp = t;
    ::localtime_r( &m_Timestamp, &m_TimeStruct );
}

TimeUtils::TimeUtils( struct tm * tm )
{
    m_TimeStruct = *tm;
    m_Timestamp = ::mktime( tm );
}

time_t TimeUtils::getTimestamp()
{
    if ( m_Timestamp == 0 )
    {
        m_Timestamp = TimeUtils::time();
    }

    return m_Timestamp;
}

struct tm * TimeUtils::getTimeStruct()
{
    if ( m_TimeStruct.tm_year == 0 )
    {
        time_t now = getTimestamp();
        ::localtime_r( &now, &m_TimeStruct );
    }

    return &m_TimeStruct;
}

int32_t TimeUtils::getDate()
{
    int32_t rc = 0;

    getTimeStruct();

    rc = ( m_TimeStruct.tm_year+1900 )*10000;
    rc += ( m_TimeStruct.tm_mon+1 )*100;
    rc += m_TimeStruct.tm_mday;

    return rc;
}

int32_t TimeUtils::getTime()
{
    int32_t rc = 0;

    getTimeStruct();

    rc = (m_TimeStruct.tm_hour)*10000;
    rc += (m_TimeStruct.tm_min)*100;
    rc += m_TimeStruct.tm_sec;

    return rc;
}

int32_t TimeUtils::getWeekday()
{
    this->getTimeStruct();
    return m_TimeStruct.tm_wday;
}

int32_t TimeUtils::getWeeknumber()
{
    char buf[ 64 ];

    this->getTimeStruct();

    if ( ::strftime( buf, 63, "%W", &m_TimeStruct ) != 0 )
    {
        return std::atoi( buf );
    }

    return 0;
}

int32_t TimeUtils::getMonth()
{
    this->getTimeStruct();
    return m_TimeStruct.tm_mon + 1;
}

time_t TimeUtils::getZeroClockTimestamp()
{
    getTimeStruct();

    struct tm zero_timestamp = m_TimeStruct;
    zero_timestamp.tm_hour = zero_timestamp.tm_min = zero_timestamp.tm_sec = 0;

    return ::mktime( &zero_timestamp );
}

time_t TimeUtils::getNextZeroClockTimestamp()
{
    return this->getZeroClockTimestamp() + eSeconds_OneDay;
}

time_t TimeUtils::getSpecifiedTimestamp( const char * s )
{
    getTimeStruct();

    int32_t matched = 0;
    struct tm spec = m_TimeStruct;
    matched = std::sscanf( s, "%d:%d:%d", &(spec.tm_hour), &(spec.tm_min), &(spec.tm_sec) );

    return matched > 0 ? ::mktime( &spec ) : 0;
}

time_t TimeUtils::getSpecifiedTimestamp( int32_t offset, int32_t weekday, const char * s )
{
    time_t rc = 0;

    getTimeStruct();

    struct tm spec = m_TimeStruct;
    int32_t matched = 0, nweekday = 0;
    matched = std::sscanf( s, "%d:%d:%d", &(spec.tm_hour), &(spec.tm_min), &(spec.tm_sec) );

    if ( matched <= 0 )
    {
        return 0;
    }

    // 时间
    rc = ::mktime( &spec );

    // 计算周日
    weekday = weekday == 0 ? 7 : weekday;
    nweekday = spec.tm_wday == 0 ? 7 : spec.tm_wday;

    // 和weekday相差的天数
    rc += ( weekday - nweekday ) * eSeconds_OneDay;
    rc += offset * eSeconds_OneDay * eDays_OneWeek;

    return rc;
}

time_t TimeUtils::time()
{
    time_t now = 0;
    struct timeval tv;

    if ( ::gettimeofday(&tv, NULL) == 0 )
    {
        now = tv.tv_sec;
    }
    else
    {
        now = ::time( NULL );
    }

    now += global::delta_timestamp;

    return now;
}

int64_t TimeUtils::now()
{
    int64_t now = 0;
    struct timeval tv;

    if ( ::gettimeofday(&tv, NULL) == 0 )
    {
        now = tv.tv_sec*1000+tv.tv_usec/1000;
        now += (int64_t)global::delta_timestamp * 1000;
    }

    return now;
}

int32_t TimeUtils::tzminutes()
{
    struct timeval tv;
    struct timezone tz;

    if ( ::gettimeofday( &tv, &tz ) == 0 )
    {
        return tz.tz_minuteswest;
    }

    return 0;
}

int32_t TimeUtils::sleep( uint64_t mseconds )
{
    struct timeval tv;

    tv.tv_sec   = mseconds/1000;
    tv.tv_usec  = (mseconds%1000)*1000;

    return ::select(0, NULL, NULL, NULL, &tv);
}

time_t TimeUtils::getTimestamp( const char * str )
{
    struct tm t;
    char * matched = NULL;

    std::memset( &t, 0, sizeof(tm) );
    matched = strptime( str, "%Y-%m-%d %H:%M:%S", &t );

    return matched != NULL ? mktime(&t) : 0;
}

time_t TimeUtils::getTimestampByDate( const char * date )
{
    struct tm t;
    char * matched = NULL;

    std::memset( &t, 0, sizeof(tm) );
    matched = strptime( date, "%Y-%m-%d", &t );

    return matched != NULL ? mktime(&t) : 0;
}

int32_t TimeUtils::daysdelta( time_t t1, time_t t2 )
{
    utils::TimeUtils tt1( t1 );
    utils::TimeUtils tt2( t2 );

    return std::abs( tt2.getZeroClockTimestamp() - tt1.getZeroClockTimestamp() ) / eSeconds_OneDay + 1;
}

}
