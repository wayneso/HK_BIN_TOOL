#ifndef EDID_FUNC_H
#define EDID_FUNC_H

#include <QByteArray>
#include <QString>

typedef struct
{
    int addr;
    QByteArray buffer;
    int len;
}EDID;


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
} EDID_Info;



QVector<EDID> Find_EDID(const QByteArray& binBuffer);
EDID_Info Read_EDID(const QByteArray& edidBuffer);
QByteArray Modify_EDID(QByteArray edidBuffer, const EDID_Info& edidInfo);
void Write_EDID(QByteArray& binBuffer, const QVector<EDID>& edidList);
QString EDID_Data_Convert_String(const EDID& edid);


#endif
