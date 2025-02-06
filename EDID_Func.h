#ifndef EDID_FUNC_H
#define EDID_FUNC_H

#include <QByteArray>
#include <QString>
#include <QList>

typedef struct
{
    int addr;
    QByteArray buffer;
    int len;
}EDID;

typedef struct {
    float pixel_clock_MHz;        // 像素时钟（单位：MHz）
    uint16_t h_active;            // 水平有效像素
    uint16_t h_blank;             // 水平消隐像素
    uint16_t v_active;            // 垂直有效行数
    uint16_t v_blank;             // 垂直消隐行数
    uint16_t h_front_porch;       // 水平前廊
    uint16_t h_sync_pulse;        // 水平同步脉冲宽度
    uint16_t v_front_porch;       // 垂直前廊
    uint16_t v_sync_pulse;        // 垂直同步脉冲宽度
    bool h_sync_positive;         // 水平同步极性（true = 正极性）
    bool v_sync_positive;         // 垂直同步极性（true = 正极性）
    bool interlaced;              // 隔行扫描标志
} EDID_Resolution;

typedef struct {
    QString manufacturerID;    // 制造商 ID
    QString productID;         // 产品 ID
    QString version;           // EDID 版本号
    int manufactureWeek;       // 制造周
    int manufactureYear;       // 制造年份
    float horizontalSizeCm;   // 水平物理尺寸（厘米）
    float verticalSizeCm;     // 垂直物理尺寸（厘米）
    float diagonalSizeInches; // 对角线尺寸（英寸）
    QString monitorName;       // 显示器名字
    QList<EDID_Resolution> EDID_DTD;
} EDID_Info;







QVector<EDID> Find_EDID(const QByteArray& binBuffer);
EDID_Info Read_EDID(const QByteArray& edidBuffer);
QByteArray Modify_EDID(QByteArray edidBuffer, const EDID_Info& edidInfo);
void Write_EDID(QByteArray& binBuffer, const QVector<EDID>& edidList);
QString EDID_Data_Convert_String(const EDID& edid);

EDID_Resolution Parse_EDID_DTD(const QByteArray& edidBuffer, int offset);

#endif
