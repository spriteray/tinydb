
#ifndef __SRC_TINYDB_DUMPBACKEND_H__
#define __SRC_TINYDB_DUMPBACKEND_H__

#include "io/io.h"

namespace Datad
{

// 参数
class CDataServer;
struct DumpThreadArgs
{
    sid_t sid;
    CDataServer * server;
};

// 存档后台线程
void * dump_backend( void * arg );

}

#endif
