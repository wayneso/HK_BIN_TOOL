#ifndef BIN_DATA_FUNC_H
#define BIN_DATA_FUNC_H

#include <QByteArray>
#include <QString>
#include <QDebug>


typedef struct
{
    const char* target_string;
    int positions;
    int offset;
    uchar* outputBuffer;
    size_t outputBufferSize;

}Bin_Data_String;


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



#endif
