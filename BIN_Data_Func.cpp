/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email &
 * please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-11 11:24:52
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * user.email & please set dead value or install git & please set dead value or install git user.email & please set dead
 * value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-23 00:48:35
 * @FilePath: \HK_BIN_Tool\BIN_Data_Func.cpp
 * @Description:
 *
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved.
 */
#include "BIN_Data_Func.h"
#include <QFileDialog>
#include <QFile>
#include <QRegularExpression>
#include <QVector>
#include <QStringList>
#include <QDebug>

/*背光电流定位结构体*/
uchar BacklightDef_Data_Buffer[6] = {0};
const char *BacklightDef_string = "This is BacklightDef Flag!";
int BacklightDef_string_len = STRLEN_INCL_NULL(BacklightDef_string);
Bin_Data_String BinData_BackLightDef = {
    BacklightDef_string,      // 目标字符串
    0,                        // 字符串初始位置
    BacklightDef_string_len,  // 偏移量
    BacklightDef_Data_Buffer, // 输出缓冲区
    6                         // 输出缓冲区大小
};
/*键值定位结构体*/
uchar Key_Value_Data_Buffer[10] = {0};
const char *Key_Value_Data_string = "This is Keyboardflag Flag!";
int Key_Value_Data_string_len = STRLEN_INCL_NULL(Key_Value_Data_string);
Bin_Data_String Key_Value_DataDef = {
    Key_Value_Data_string,     // 目标字符串
    0,                         // 字符串初始位置
    Key_Value_Data_string_len, // 偏移量
    Key_Value_Data_Buffer,     // 输出缓冲区
    10                         // 输出缓冲区大小
};
/*OSD常用设置定位结构体*/
uchar Osd_Data_Buffer[10] = {0};
const char *Osd_Data_string = "HK_OSD_Default Flag";
int Osd_Data_string_len = STRLEN_INCL_NULL(Osd_Data_string);
Bin_Data_String Osd_DataDef = {
    Osd_Data_string,     // 目标字符串
    0,                   // 字符串初始位置
    Osd_Data_string_len, // 偏移量
    Osd_Data_Buffer,     // 输出缓冲区
    10                   // 输出缓冲区大小
};
/*HKC_OSD常用设置定位结构体*/
uchar HKC_Osd_Data_Buffer[24] = {0};
const char *HKC_Osd_Data_string = "HK_HKC_OSD_Default Flag";
int HKC_Osd_Data_string_len = STRLEN_INCL_NULL(HKC_Osd_Data_string);
Bin_Data_String HKC_Osd_DataDef = {
    HKC_Osd_Data_string,     // 目标字符串
    0,                   // 字符串初始位置
    HKC_Osd_Data_string_len, // 偏移量
    HKC_Osd_Data_Buffer,     // 输出缓冲区
    24                   // 输出缓冲区大小
};

/*HKC色温定位结构体*/
uchar HKC_TEMP_COLOR_Data_Buffer[18] = {0};
const char *HKC_TEMP_COLOR_Data_string = "HK_HKC_TEMP_COLOR_Default Flag";
int HKC_TEMP_COLOR_Data_string_len = STRLEN_INCL_NULL(HKC_TEMP_COLOR_Data_string);
Bin_Data_String HKC_TEMP_COLOR_DataDef = {
    HKC_TEMP_COLOR_Data_string,     // 目标字符串
    0,                   // 字符串初始位置
    HKC_TEMP_COLOR_Data_string_len, // 偏移量
    HKC_TEMP_COLOR_Data_Buffer,     // 输出缓冲区
    18                   // 输出缓冲区大小
};
/*LOGO基础定义数据定位结构体*/
uchar LOGO_BASE_Default_Buffer[7] = {0};
const char *LOGO_BASE_Default_string = "HK_LOGO_BASE_Default Flag";
int LOGO_BASE_Default_string_len = STRLEN_INCL_NULL(LOGO_BASE_Default_string);
Bin_Data_String LOGO_BASE_Default_DataDef = {
    LOGO_BASE_Default_string,     // 目标字符串
    0,                      // 字符串初始位置
    LOGO_BASE_Default_string_len, // 偏移量
    LOGO_BASE_Default_Buffer,       // 输出缓冲区
    7                    // 输出缓冲区大小
};

/*LOGO_INDEX数据定位结构体*/
uchar LOGO_INDEX_Data_Buffer[1024] = {0};
const char *LOGO_INDEX_target_string = "HK_LOGO_INDEX_VLC_DATA Flag";
int LOGO_INDEX_target_string_len = STRLEN_INCL_NULL(LOGO_INDEX_target_string);
Bin_Data_String LOGO_INDEX_DataDef = {
    LOGO_INDEX_target_string,     // 目标字符串
    0,                            // 字符串初始位置
    LOGO_INDEX_target_string_len, // 偏移量
    LOGO_INDEX_Data_Buffer,       // 输出缓冲区
    1024                          // 输出缓冲区大小
};
/*LOGO_VLC数据定位结构体*/
uchar LOGO_Data_Buffer[2048] = {0};
const char *LOGO_target_string = "HK_LOGO0_VLC_DATA Flag";
int LOGO_target_string_len = STRLEN_INCL_NULL(LOGO_target_string);
Bin_Data_String LOGO_DataDef = {
    LOGO_target_string,     // 目标字符串
    0,                      // 字符串初始位置
    LOGO_target_string_len, // 偏移量
    LOGO_Data_Buffer,       // 输出缓冲区
    2048                    // 输出缓冲区大小
};


uchar LOGO_Data_END_Buffer[4096] = {0};
const char *LOGO_target_END_string = "HK_LOGO0_VLC_DATA_END Flag";
int LOGO_target_END_string_len = STRLEN_INCL_NULL(LOGO_target_END_string);
Bin_Data_String LOGO_END_DataDef = {
    LOGO_target_END_string,     // 目标字符串
    0,                      // 字符串初始位置
    STRLEN_INCL_NULL(LOGO_target_END_string), // 偏移量
    LOGO_Data_END_Buffer,       // 输出缓冲区
    4096                    // 输出缓冲区大小
};




uchar LOGO1_Data_Buffer[4096] = {0};
const char *LOGO1_target_string = "HK_LOGO1_VLC_DATA Flag";
int LOGO1_target_string_len = STRLEN_INCL_NULL(LOGO1_target_string);
Bin_Data_String LOGO1_DataDef = {
    LOGO1_target_string,     // 目标字符串
    0,                       // 字符串初始位置
    LOGO1_target_string_len, // 偏移量
    LOGO1_Data_Buffer,       // 输出缓冲区
    4096                     // 输出缓冲区大小
};
uchar LOGO1_INDEX_Data_Buffer[4096] = {0};
const char *LOGO1_INDEX_target_string = "HK_LOGO1_INDEX_VLC_DATA Flag";
int LOGO1_INDEX_target_string_len = STRLEN_INCL_NULL(LOGO1_INDEX_target_string);
Bin_Data_String LOGO1_INDEX_DataDef = {
    LOGO1_INDEX_target_string,     // 目标字符串
    0,                             // 字符串初始位置
    LOGO1_INDEX_target_string_len, // 偏移量
    LOGO1_INDEX_Data_Buffer,       // 输出缓冲区
    4096                           // 输出缓冲区大小
};
bool Find_TargetString_InBinFile(const QByteArray Bin_Buffer, Bin_Data_String &Bin_Data)
{
    bool State;

    // 查找目标字符串的位置
    Bin_Data.positions = Bin_Buffer.indexOf(Bin_Data.target_string);

    // 如果找到目标字符串
    if (Bin_Data.positions != -1)
    {
        qDebug() << "找到目标字符串, 起始位置:" << Bin_Data.positions;
        // 拷贝目标字符串到输出缓冲区
        std::memcpy(Bin_Data.outputBuffer,
                    Bin_Buffer.mid(Bin_Data.positions + Bin_Data.offset, Bin_Data.outputBufferSize).data(),
                    Bin_Data.outputBufferSize);

        State = true; // 成功找到目标字符串
    }
    else
    {
        State = false;
        qDebug() << "未找到目标字符串";
    }
    return State;
}

bool Write_TargetString_InBinFile(QByteArray &Bin_Buffer, Bin_Data_String &Bin_Data)
{
    // 状态变量，默认失败
    bool State = false;

    // 打印调试信息
    qDebug() << "目标字符串:" << Bin_Data.target_string;
    qDebug() << "起始位置:" << Bin_Data.positions;
    qDebug() << "偏移量:" << Bin_Data.offset;
    qDebug() << "输出缓冲区大小:" << Bin_Data.outputBufferSize;
    qDebug() << "缓冲区大小:" << Bin_Buffer.size();

    // 检查目标字符串的起始位置是否有效
    if (Bin_Data.positions != -1)
    {
        // 检查偏移量和输出缓冲区是否符合条件
        if (Bin_Data.positions + Bin_Data.offset + Bin_Data.outputBufferSize <= Bin_Buffer.size())
        {
            // 替换数据
            memcpy(Bin_Buffer.data() + Bin_Data.positions + Bin_Data.offset, Bin_Data.outputBuffer,
                   Bin_Data.outputBufferSize);

            State = true; // 标记操作成功
            qDebug() << "写入成功!";
        }
        else
        {
            qDebug() << "写入失败: 超出缓冲区范围!";
        }
    }
    else
    {
        qDebug() << "写入失败: 无效的起始位置!";
    }

    return State;
}

bool Add_LOGO_DATA(const QString &filePath, QVector<int> &indexArray, QVector<int> &vlcArray)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << QObject::tr("无法打开文件:") << filePath;
        return false;
    }
    QString content = QString::fromUtf8(file.readAll());
    file.close();

    // 替换占位符
    content.replace("_NEXT_", "0xFE");
    content.replace("_END_",  "0xFF");

    // 内部函数：解析数组内容，使用捕获组匹配
    auto parseArray = [&](const QString &name, QVector<int> &outArray) {
        // 构造正则：匹配 name[] = { ... }
        QString pattern = QString("%1\\s*\\[\\s*\\]\\s*=\\s*\\{([^}]*)\\}").arg(name);
        QRegularExpression re(pattern);
        re.setPatternOptions(QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatch match = re.match(content);
        if (!match.hasMatch()) {
            qWarning() << QObject::tr("未找到 %1 数组").arg(name);
            return;
        }
        QString raw = match.captured(1);
        QStringList vals = raw.split(',', Qt::SkipEmptyParts);
        for (const QString &v : vals) {
            outArray.append(v.trimmed().toInt(nullptr, 0));
        }
    };

    parseArray("Index", indexArray);
    parseArray("VLC",   vlcArray);

    return true;
}

