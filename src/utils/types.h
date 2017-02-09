
#ifndef __SRC_UTILS_TYPES_H__
#define __SRC_UTILS_TYPES_H__

#include <map>
#include <stdint.h>

// UnorderedMap定义
#if ( __GNUC__ < 4 || __GNUC__ == 4 && __GNUC_MINOR__ < 1 )
#include <map>
#define UnorderedMap std::map
#else
#include <tr1/unordered_map>
#define UnorderedMap std::tr1::unordered_map
#endif

// 分支预测
#define likely(x)       __builtin_expect( (x), 1 )
#define unlikely(x)     __builtin_expect( (x), 0 )

#define SAFE_DELETE(x)  { if (x) { delete (x); (x) = NULL; } }

// 全局变量
namespace global
{
    // 全局时间修正
    extern int32_t delta_timestamp;
}

#endif
