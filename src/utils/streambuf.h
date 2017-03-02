
#ifndef __SRC_UTILS_STREAMBUFFER_H__
#define __SRC_UTILS_STREAMBUFFER_H__

#include <vector>
#include <string>
#include <cassert>
#include <stdint.h>

#include "slice.h"

class StreamBuf
{
public :
    enum
    {
        eMethod_Encode      = 1,
        eMethod_Decode      = 2,
        eEndian_Big         = 1,    // 字节序为大端
        eEndian_Little      = 2,    // 字节序为小端
    };

    // Decode StreamBuf
    // buf: 缓冲区地址
    // len: 缓冲区长度
    StreamBuf( const char * buf, uint32_t len, int8_t endian=eEndian_Big );

    // Encode StreamBuf
    // buf: 缓冲区地址
    // len: 缓冲区长度
    StreamBuf( char * buf, uint32_t len, int8_t endian=eEndian_Big );

    // Encode StreamBuf
    // len: buf的初始化长度
    // offset: 初始化偏移量, 一般是偏移掉包头
    StreamBuf( uint32_t len=128, uint32_t offset=0, int8_t endian=eEndian_Big );

    //
    ~StreamBuf();

public :
    bool decode( bool & data );
    bool decode( int8_t & data );
    bool decode( uint8_t & data );
    bool decode( int16_t & data );
    bool decode( uint16_t & data );
    bool decode( int32_t & data );
    bool decode( uint32_t & data );
    bool decode( int64_t & data );
    bool decode( uint64_t & data );
    bool decode( Slice & data );
    bool decode( std::string & data );
    bool decode( char * data, uint16_t & len );
    bool decode( char * data, uint32_t & len );

    // 模板原型
    template<class T>
        bool decode( T & data )
        {
            assert( m_Method == eMethod_Decode );
            return true;
        }

    // 编码vector
    template<class T>
        bool decode( std::vector<T> & data )
        {
            assert ( m_Method == eMethod_Decode );

            uint32_t count = 0;

            if ( !decode(count) )
            {
                return false;
            }

            data.resize( count );

            for ( uint32_t i = 0; i < count; ++i )
            {
                if ( !decode( data[i] ) )
                {
                    return false;
                }
            }

            return true;
        }

public :
    bool encode( const bool & data );
    bool encode( const int8_t & data );
    bool encode( const uint8_t & data );
    bool encode( const int16_t & data );
    bool encode( const uint16_t & data );
    bool encode( const int32_t & data );
    bool encode( const uint32_t & data );
    bool encode( const int64_t & data );
    bool encode( const uint64_t & data );
    bool encode( const Slice & data );
    bool encode( const std::string & data );
    bool encode( const char * data, uint16_t len );
    bool encode( const char * data, uint32_t len );

    // 模板
    template<class T>
        bool encode( T const & data )
        {
            assert( m_Method == eMethod_Encode );
            return true;
        }

    // 编码vector
    template<class T>
        bool encode( const std::vector<T> & data )
        {
            assert ( m_Method == eMethod_Encode );

            uint32_t count = (uint32_t)data.size();
            if ( !encode(count) )
            {
                return false;
            }

            for ( uint32_t i = 0; i < count; ++i )
            {
                if ( !encode(data[i]) )
                {
                    return false;
                }
            }

            return true;
        }

    // 追加一段空间
    bool append( const std::string & data );
    bool append( const char * data, uint32_t len );

public :
    // 重置偏移量
    void        reset();
    // 清空对象
    void        clear();

    // ToSlice
    Slice       slice();
    // ToString
    std::string string() const;

    // BUFF起始地址
    char *      data() const { return m_Buffer; }
    // 当前长度
    uint32_t    size() const { return m_Size; }
    // BUFF的总长度
    uint32_t    length() const { return m_Length; }
    // 位置
    uint32_t    position() const { return m_Position; }

private :
    // 扩展空间
    bool        expand( uint32_t len );

    int8_t      m_Endian;
    int8_t      m_Method;
    char *      m_Buffer;
    uint32_t    m_Length;           // BUFF总长度
    uint32_t    m_Size;             // 长度
    uint32_t    m_Offset;           // 构造函数中定义的偏移量
    uint32_t    m_Position;         // 序列化的位置
    bool        m_IsFixed;          // 是否是定长的BUFF
    bool        m_IsSelfFree;       // 是否自销毁
};

#endif
