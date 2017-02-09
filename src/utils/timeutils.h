
#ifndef __SRC_UTILS_TIME_H__
#define __SRC_UTILS_TIME_H__

#include <time.h>
#include <stdint.h>

#include "types.h"

namespace utils
{

class Clock
{
public :
    // 绝对时间
    // "2010-10-10 00:10:01"
    Clock( const char * now );
    // 相对时间
    Clock( time_t now, uint32_t interval );

public :
    // 转换为时间戳
    time_t convert() const { return m_Timestamp; }

private :
    time_t          m_Timestamp;
};

class TimeUtils
{
public:
    TimeUtils();
    TimeUtils( time_t t );
    TimeUtils( struct tm * tm );

public :
    // 获取UTC时间
    time_t getTimestamp();

    // 获取时间结构
    struct tm * getTimeStruct();

public :
    enum
    {
        eSeconds_OneDay         = 3600*24,      // 一天的秒数
        eDays_OneWeek           = 7,            // 一周的天数
    };

    // 2012-12-24 20121224
    int32_t getDate();

    // 10:10:58 101058
    int32_t getTime();

    // 获取星期几
    int32_t getWeekday();

    // 获取月份
    int32_t getMonth();

    // 获取第几周(距离元旦，周一开始)
    int32_t getWeeknumber();

    // 获取默认0点的时间戳
    time_t getZeroClockTimestamp();

    // 获取下一天0点的时间戳
    time_t getNextZeroClockTimestamp();

    // 获取当天指定时间点的时间戳
    // 10:00:00
    time_t getSpecifiedTimestamp( const char * s );

    // 获取周为单位的时间
    // offset   - -1上周;0本周;1下周;2下下周
    // weekday  - 周几
    // s        - 10:00:00
    time_t getSpecifiedTimestamp( int32_t offset, int32_t weekday, const char * s );

public :
    // 获得当前时间的秒数
    static time_t time();

    // 获得当前时间的毫秒数
    static int64_t now();

    // 获取时区分钟数
    static int32_t tzminutes();

    // 毫秒
    static int32_t sleep( uint64_t mseconds );

    // "2010-10-10 00:10:01"
    static time_t getTimestamp( const char * str );

    // "2010-10-10"
    static time_t getTimestampByDate( const char * date );

    // 计算t1到t2经过的自然日
    static int32_t daysdelta( time_t t1, time_t t2 );

private :
    time_t        m_Timestamp;
    struct tm     m_TimeStruct;
};

}

#endif
