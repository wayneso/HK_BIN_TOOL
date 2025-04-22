/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-11 11:24:52
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-23 00:54:03
 * @FilePath: \HK_BIN_Tool\BIN_Data_Func.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef BIN_DATA_FUNC_H
#define BIN_DATA_FUNC_H

#include <QByteArray>
#include <QString>
#include <QDebug>
#include <string.h>

#define STRLEN_INCL_NULL(s)  ( (int)(strlen(s) + 1) )

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


extern Bin_Data_String BinData_BackLightDef;
extern Bin_Data_String Key_Value_DataDef;
extern Bin_Data_String Osd_DataDef;
extern Bin_Data_String LOGO_INDEX_DataDef;
extern Bin_Data_String LOGO_DataDef;
extern Bin_Data_String LOGO1_INDEX_DataDef;
extern Bin_Data_String LOGO1_DataDef;

bool Find_TargetString_InBinFile(const QByteArray Bin_Buffer, Bin_Data_String &Bin_Data);
bool Write_TargetString_InBinFile(QByteArray &Bin_Buffer, Bin_Data_String &Bin_Data);


#endif
