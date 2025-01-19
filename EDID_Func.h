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


typedef struct {
    int Pixel_Clock;                     // 像素时钟（单位：kHz，通常需将 EDID 数据中10kHz单位的值乘以10）
    int Horizontal_Active_Pixels;        // 水平活动像素数（分辨率的水平部分，例如1920）
    int Horizontal_Blanking_Pixels;      // 水平空白像素数
    int Horizontal_Sync_Offset;          // 水平同步偏移量
    int Horizontal_Sync_Width;           // 水平同步宽度

    int Vertical_Active_Pixels;          // 垂直活动像素数（分辨率的垂直部分，例如1080）
    int Vertical_Blanking_Pixels;        // 垂直空白像素数
    int Vertical_Sync_Offset;            // 垂直同步偏移量
    int Vertical_Sync_Width;             // 垂直同步宽度

    int Horizontal_Image_Size;           // 水平图像尺寸（单位：毫米）
    int Vertical_Image_Size;             // 垂直图像尺寸（单位：毫米）
} EDID_Resolution;




QVector<EDID> Find_EDID(const QByteArray& binBuffer);
EDID_Info Read_EDID(const QByteArray& edidBuffer);
QByteArray Modify_EDID(QByteArray edidBuffer, const EDID_Info& edidInfo);
void Write_EDID(QByteArray& binBuffer, const QVector<EDID>& edidList);
QString EDID_Data_Convert_String(const EDID& edid);


#endif
