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
#include <QLineEdit>

#define STRLEN_INCL_NULL(s)  ( (int)(strlen(s) + 1) )
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

// 合成：高4位和低4位拼接为8位整数
#define COMBINE_4BIT_HIGH_LOW(high, low)   (((high) << 4) | ((low) >> 4))

// 拆分：从整数中获取低4位和高4位
#define GET_LOW_4BIT(value)   (((value) & 0x0F) << 4)
#define GET_HIGH_4BIT(value)  (((value) >> 4) & 0x0F)

#define _RTD2795_OSD_COLOR_TEMP_MAKS                0xE0
#define _RTD2795_OSD_COLOR_TEMP_MAKS_OFFSET         5

#define _RTD2795_OSD_LANGUAGE_MAKS                  0x1F
#define _RTD2795_OSD_LANGUAGE_MAKS_OFFSET           0


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

struct ColorEditItem {
    QLineEdit* edit;
    int index;
};

extern Bin_Data_String BinData_BackLightDef;
extern Bin_Data_String Key_Value_DataDef;
extern Bin_Data_String Osd_DataDef;
extern Bin_Data_String ColorTemp_DataDef;

extern Bin_Data_String HKC_Osd_DataDef;
extern Bin_Data_String HKC_TEMP_COLOR_DataDef;

extern Bin_Data_String LOGO_2383_BASE_Default_DataDef;
extern Bin_Data_String LOGO_2383_BASE_Default1_DataDef;
extern Bin_Data_String LOGO_BASE_Default_DataDef;


extern Bin_Data_String LOGO_Palette_DataDef;

extern Bin_Data_String LOGO_INDEX_DataDef;
extern Bin_Data_String LOGO_INDEX_END_DataDef;
extern Bin_Data_String LOGO_DataDef;
extern Bin_Data_String LOGO_END_DataDef;
extern Bin_Data_String LOGO1_INDEX_DataDef;
extern Bin_Data_String LOGO1_DataDef;
uchar Find_Byte_With_Mask_Offset(uchar byte, uchar mask, uchar offset);
uchar Write_Byte_With_Mask_Offset(uchar byte, uchar value, uchar mask, uchar offset);
void Combine_High_Low_4BIT(QLineEdit* edit, uchar* buffer);
bool Find_TargetString_InBinFile(const QByteArray Bin_Buffer, Bin_Data_String &Bin_Data);
bool Write_TargetString_InBinFile(QByteArray &Bin_Buffer, Bin_Data_String &Bin_Data);
bool Add_LOGO_DATA(const QString &filePath, QVector<int> &indexArray, QVector<int> &vlcArray);
QMap<QString, QString> parseHeaderMacros(const QString &filePath,
                                         const QStringList &activeDefines,
                                         const QStringList &targetMacros);
QVector<quint8> extractDrawLogoArray(const QString &filePath, const QStringList &activeDefines);
QVector<quint8> extractByteArray(const QString &filePath,
                                 const QStringList &activeDefines,
                                 const QString &arrayName);
#endif
