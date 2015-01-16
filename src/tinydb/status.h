
#ifndef __SRC_TINYDB_STATUS_H__
#define __SRC_TINYDB_STATUS_H__

#include <sys/time.h>
#include <sys/resource.h>

#include <sys/types.h>
#include <unistd.h>

#include "define.h"
#include "utils/timeutils.h"

namespace Datad
{

class ServerStatus
{
public :
    ServerStatus();
    ~ServerStatus();

    void refresh();

public :
    // 获取进程ID
    pid_t getPid() const { return getpid(); }
    // 服务器开机时间
    time_t getStartTime() const { return m_StartTime; }

    // 添加getops
    void addGetOps() { ++m_GetOps; }
    uint64_t getGetOps() const { return m_GetOps; }

    // 添加getops
    void addSetOps() { ++m_SetOps; }
    uint64_t getSetOps() const { return m_SetOps; }

    // 获取当前时间
    time_t getNowTime() { return m_NowTime; }

    // 获取系统开销
    void getUserUsage( uint64_t & sec, uint64_t & usec );
    void getSystemUsage( uint64_t & sec, uint64_t & usec );

private :
    time_t          m_StartTime;
    uint64_t        m_GetOps;
    uint64_t        m_SetOps;
    time_t          m_NowTime;
    struct rusage   m_CpuUsage;
};

}

#endif
