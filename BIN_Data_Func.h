#ifndef BIN_DATA_FUNC_H
#define BIN_DATA_FUNC_H

#include <QByteArray>
#include <QString>
#include <QDebug>


typedef struct
{
    const char* target_string;  // 目标字符串
    int positions;              // 初始位置
    int offset;                 // 偏移量
    uchar* outputBuffer;        // 输出缓冲区指针
    size_t outputBufferSize;    // 输出缓冲区大小
} Bin_Data_String;



typedef struct
{
    const char* target_string;
    int len;
    int positions;
    int offset;
    int* outputBuffer;
    size_t outputBufferSize;

}Bin_Data;



bool Find_TargetString_InBinFile(const QByteArray Bin_Buffer, Bin_Data_String &Bin_Data);
bool Write_TargetString_InBinFile(QByteArray &Bin_Buffer, Bin_Data_String &Bin_Data);


#endif
