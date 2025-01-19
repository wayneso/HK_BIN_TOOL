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
    float Pixel_Clock;                     // 像素时钟
    unsigned short Horizontal_Active_Pixels;        // 水平活动像素数（分辨率的水平部分，例如1920）
    unsigned short Horizontal_Blanking_Pixels;      // 水平空白像素数
    unsigned short Horizontal_Sync_Offset;          // 水平同步偏移量
    unsigned short Horizontal_Sync_Width;           // 水平同步宽度

    unsigned short Vertical_Active_Pixels;          // 垂直活动像素数（分辨率的垂直部分，例如1080）
    unsigned short Vertical_Blanking_Pixels;        // 垂直空白像素数
    unsigned short Vertical_Sync_Offset;            // 垂直同步偏移量
    unsigned short Vertical_Sync_Width;             // 垂直同步宽度

    unsigned short Horizontal_Image_Size;           // 水平图像尺寸（单位：毫米）
    unsigned short Vertical_Image_Size;             // 垂直图像尺寸（单位：毫米）
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
