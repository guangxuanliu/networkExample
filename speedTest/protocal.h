//由于不同操作系统之间的字节对齐方式不同，故网络传输数据时，要设置字节对齐方式
//设置变量以n字节对齐方式，指定结构体、联合以及类成员
#pragma pack(push) //保持对齐状态
#pragma pack(1) //设定为1字节对齐

#ifndef _MESSAGE_HEADER
#define _MESSAGE_HEADER

typedef struct MESSAGE_HEADER
{
    unsigned short int uhMessageLength; //消息长度
}SOCKET_MESSAGE_HEADER;
#endif

namespace DateStruct
{
    struct DS_STATE
    {
        char m_char;
        int m_int;
        char m_string[100];
    };
}

#pragma pack(pop) //恢复对齐状态