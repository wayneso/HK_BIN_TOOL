#include <qDebug>
#include <QString>
#include <QByteArray>

#include "EDID_Func.h"


QVector<EDID> Find_EDID(const QByteArray& binBuffer) {
    QVector<EDID> edidList;
    const QByteArray edidHeader = QByteArray::fromHex("00FFFFFFFFFFFF00");
    const int baseEdidSize = 128;

    int index = 0;
    while (index < binBuffer.size() - baseEdidSize) {
        // 查找 EDID 基础块的起始标识符
        index = binBuffer.indexOf(edidHeader, index);
        if (index == -1) {
            break;  // 未找到更多的 EDID
        }

        // 提取基础块
        QByteArray baseBlock = binBuffer.mid(index, baseEdidSize);
        if (baseBlock.size() < baseEdidSize) {
            break;  // 数据不足，结束提取
        }

        // 校验基础块的完整性
        int checksum = 0;
        for (uchar byte : baseBlock) {
            checksum += byte;
        }
        if ((checksum & 0xFF) != 0) {
            index += baseEdidSize;  // 校验失败，跳过该块
            continue;
        }

        // 获取扩展块计数器（第 126 字节）
        int extensionBlockCount = static_cast<unsigned char>(baseBlock.at(126));
        int totalEdidSize = baseEdidSize + extensionBlockCount * baseEdidSize;

        // 提取完整的 EDID 数据
        QByteArray fullEdid = binBuffer.mid(index, totalEdidSize);
        if (fullEdid.size() < totalEdidSize) {
            break;  // 数据不足，结束提取
        }

        // 校验所有扩展块的完整性
        bool valid = true;
        for (int i = baseEdidSize; i < totalEdidSize; i += baseEdidSize) {
            QByteArray block = fullEdid.mid(i, baseEdidSize);
            if (block.size() < baseEdidSize) {
                valid = false;
                break;
            }

            checksum = 0;
            for (uchar byte : block) {
                checksum += byte;
            }
            if ((checksum & 0xFF) != 0) {
                valid = false;
                break;
            }
        }

        if (valid) {
            EDID edid = {index, fullEdid, totalEdidSize};
            edidList.append(edid);
        }

        // 更新索引，避免重复匹配
        index += totalEdidSize;
    }
    return edidList;
}

EDID_Resolution Parse_EDID_DTD(const QByteArray& edidBuffer, int offset) {
    EDID_Resolution resolution;

    // 获取像素时钟（单位：10kHz）
    unsigned char pixelClockLow = edidBuffer[offset];
    unsigned char pixelClockHigh = edidBuffer[offset + 1];
    unsigned short pixelClock = (pixelClockHigh << 8) | pixelClockLow;
    resolution.Pixel_Clock = pixelClock / 100.0f;

    // 获取水平活动像素
    unsigned char horizontalActiveLow = edidBuffer[offset + 2];
    unsigned char horizontalActiveHigh = edidBuffer[offset + 4] & 0xF0;
    unsigned short horizontalActive = (horizontalActiveHigh / 16 * 256) + horizontalActiveLow;
    resolution.Horizontal_Active_Pixels = horizontalActive;

    // 获取水平空白像素
    unsigned char horizontalBlankingLow = edidBuffer[offset + 3];
    unsigned char horizontalBlankingHigh = edidBuffer[offset + 4] & 0x0F;
    unsigned short horizontalBlanking = (horizontalBlankingHigh * 256) + horizontalBlankingLow;
    resolution.Horizontal_Blanking_Pixels = horizontalBlanking;

    // 获取垂直活动像素
    unsigned char verticalActiveLow = edidBuffer[offset + 5];
    unsigned char verticalActiveHigh = edidBuffer[offset + 7];
    unsigned short verticalActive = (verticalActiveHigh  / 16 * 256) + verticalActiveLow;
    resolution.Vertical_Active_Pixels = verticalActive;

    // 获取垂直空白像素
    unsigned char verticalBlankingLow = edidBuffer[offset + 6];
    unsigned char verticalBlankingHigh = edidBuffer[offset + 7] & 0x0F;
    unsigned short verticalBlanking = (verticalBlankingHigh * 256) + verticalBlankingLow;
    resolution.Vertical_Blanking_Pixels = verticalBlanking;

    // 获取水平同步偏移和宽度
    unsigned char Horizontal_Sync_OffsetLow = edidBuffer[offset + 8];
    unsigned char Horizontal_Sync_OffsetHigh = edidBuffer[offset + 11] >> 6;
    unsigned short Horizontal_Sync_Offset = (Horizontal_Sync_OffsetHigh * 256) + Horizontal_Sync_OffsetLow;
    resolution.Horizontal_Sync_Offset = Horizontal_Sync_Offset;

    unsigned char Horizontal_Sync_WidthLow = edidBuffer[offset + 9];
    unsigned char Horizontal_Sync_WidthHigh = edidBuffer[offset + 11] & 0x30 >> 4;
    unsigned short Horizontal_Sync_Width = (Horizontal_Sync_WidthHigh * 256) + Horizontal_Sync_WidthLow;
    resolution.Horizontal_Sync_Width = Horizontal_Sync_Width;

    // 获取垂直同步偏移和宽度
    unsigned char Vertical_Sync_OffsetLow = edidBuffer[offset + 10] & 0xF0 >> 4;
    unsigned char Vertical_Sync_OffsetHigh = edidBuffer[offset + 11] & 0x0C >> 2;
    unsigned short Vertical_Sync_Offset = (Vertical_Sync_OffsetHigh * 16) + Vertical_Sync_OffsetLow;
    resolution.Vertical_Sync_Offset = Vertical_Sync_Offset;

    unsigned char Vertical_Sync_WidthLow = edidBuffer[offset + 10] & 0x0F;
    unsigned char Vertical_Sync_WidthHigh = edidBuffer[offset + 11] & 0x03;
    unsigned short Vertical_Sync_Width = (Vertical_Sync_WidthHigh * 16) + Vertical_Sync_WidthLow;
    resolution.Vertical_Sync_Width = Vertical_Sync_Width;

    return resolution;
}




EDID_Info Read_EDID(const QByteArray& edidBuffer) {
    EDID_Info info;

    const int EDID_MIN_SIZE = 128;
    const int MANUFACTURER_ID_OFFSET = 8;
    const int PRODUCT_ID_OFFSET = 10;
    const int EDID_VERSION_OFFSET = 18;
    const int MANUFACTURE_YEAR_OFFSET = 17;
    const int MANUFACTURE_WEEK_OFFSET = 16;
    const int MONITOR_NAME_BLOCK_START = 108;
    const int MONITOR_NAME_BLOCK_END = 125;

    // 校验 EDID 数据长度
    if (edidBuffer.size() < EDID_MIN_SIZE) {
        qDebug() << "Invalid EDID size";
        return info;
    }

    // 提取厂商 ID
    unsigned char id1 = edidBuffer[MANUFACTURER_ID_OFFSET];
    unsigned char id2 = edidBuffer[MANUFACTURER_ID_OFFSET + 1];
    info.manufacturerID.append(QChar((id1 >> 2) + 'A' - 1));
    info.manufacturerID.append(QChar(((id1 & 0x03) << 3 | id2 >> 5) + 'A' - 1));
    info.manufacturerID.append(QChar((id2 & 0x1F) + 'A' - 1));

    // 提取产品 ID
    unsigned short productID = static_cast<unsigned char>(edidBuffer[PRODUCT_ID_OFFSET]) |
                               (static_cast<unsigned char>(edidBuffer[PRODUCT_ID_OFFSET + 1]) << 8);
    info.productID = QString("%1").arg(productID, 4, 16, QLatin1Char('0')).toUpper();

    // 提取 EDID 版本
    int majorVersion = static_cast<unsigned char>(edidBuffer[EDID_VERSION_OFFSET]);
    int minorVersion = static_cast<unsigned char>(edidBuffer[EDID_VERSION_OFFSET + 1]);
    info.version = QString("%1.%2").arg(majorVersion).arg(minorVersion);

    // 提取制造周和年份
    info.manufactureWeek = static_cast<unsigned char>(edidBuffer[MANUFACTURE_WEEK_OFFSET]);
    info.manufactureYear = static_cast<unsigned char>(edidBuffer[MANUFACTURE_YEAR_OFFSET]) + 1990;

    // 提取尺寸数据并转换单位
    unsigned char hSizeCm = edidBuffer[21];
    unsigned char vSizeCm = edidBuffer[22];
    if (hSizeCm > 0 && vSizeCm > 0) {
        double diagonalCm = std::sqrt(hSizeCm * hSizeCm + vSizeCm * vSizeCm);
        double diagonalInches = diagonalCm / 2.54;
        info.horizontalSizeCm = hSizeCm;
        info.verticalSizeCm = vSizeCm;
        info.diagonalSizeInches = diagonalInches;
    }

    // 查找显示器名称
    info.monitorName.clear();  // 确保 monitorName 初始为空
    for (int i = MONITOR_NAME_BLOCK_START; i <= MONITOR_NAME_BLOCK_END; i += 18) {  // 遍历描述符块
        if (edidBuffer[i] == 0x00 && edidBuffer[i + 1] == 0x00 &&
            edidBuffer[i + 2] == 0x00 && static_cast<unsigned char>(edidBuffer[i + 3]) == 0xFC) {  // 名称标识符
            QByteArray nameData = edidBuffer.mid(i + 5, 13);  // 名称数据区域
            nameData = nameData.trimmed();  // 去掉末尾空格或 \0
            if (!nameData.isEmpty()) {
                info.monitorName = QString::fromUtf8(nameData);  // 使用 UTF-8 转换
            }
            break;
        }
    }
/*
    // 假设EDID数据从54偏移开始，每个DTD是18个字节
for (int offset = 54; offset < edidBuffer.size(); offset += 18) {
    EDID_Resolution resolution = Parse_EDID_DTD(edidBuffer, offset);
    info.EDID_DTD.append(resolution);
}

 */
    // 解析 DTD 描述符，假设从 offset 54 开始 155 205
    EDID_Resolution resolution = Parse_EDID_DTD(edidBuffer, 54);

    // 将解析的分辨率信息添加到 EDID_Info 中
    info.EDID_DTD.append(resolution);



    return info;
}


QByteArray Modify_EDID(QByteArray edidBuffer, const EDID_Info& edidInfo) {
    if (edidBuffer.size() < 128) {
        qDebug() << "Invalid EDID size";
        return edidBuffer;
    }

    // 检查厂商 ID 是否为 3 个字符
    if (edidInfo.manufacturerID.size() != 3) {
        qDebug() << "Manufacturer ID must be 3 characters.";
        return edidBuffer;
    }

    // 将 manufacturerID 转换为 EDID 格式并存入 edidBuffer
    char id[3];
    id[0] = ((edidInfo.manufacturerID[0].toLatin1() - 'A' + 1) & 0x1F) << 2;
    id[0] |= ((edidInfo.manufacturerID[1].toLatin1() - 'A' + 1) & 0x1F) >> 3;
    id[1] = (((edidInfo.manufacturerID[1].toLatin1() - 'A' + 1) & 0x07) << 5) |
            ((edidInfo.manufacturerID[2].toLatin1() - 'A' + 1) & 0x1F);

    edidBuffer[8] = id[0];
    edidBuffer[9] = id[1];

    // 修改产品 ID
    unsigned short productID = edidInfo.productID.toUInt(nullptr, 16);
    edidBuffer[10] = static_cast<char>(productID & 0xFF);
    edidBuffer[11] = static_cast<char>((productID >> 8) & 0xFF);
/*
    // 修改 EDID 版本
    int majorVersion = edidInfo.version.split(".")[0].toInt();
    int minorVersion = edidInfo.version.split(".")[1].toInt();
    edidBuffer[18] = static_cast<char>(majorVersion);
    edidBuffer[19] = static_cast<char>(minorVersion);
*/
    // 修改制造周和制造年
    edidBuffer[16] = static_cast<char>(edidInfo.manufactureWeek);
    edidBuffer[17] = static_cast<char>(edidInfo.manufactureYear - 1990);


    // 提取尺寸数据并转换单位
    edidBuffer[21] = edidInfo.horizontalSizeCm;;
    edidBuffer[22] = edidInfo.verticalSizeCm;

    // 修改显示器名称
    QByteArray nameData = edidInfo.monitorName.toUtf8();  // 使用 UTF-8 转换
    nameData = nameData.trimmed();  // 去除名称中的多余空格或 \0 字符
    for (int i = 108; i <= 125; i += 18) {
        if (edidBuffer[i] == 0x00 && edidBuffer[i + 1] == 0x00 &&
            edidBuffer[i + 2] == 0x00 && static_cast<unsigned char>(edidBuffer[i + 3]) == 0xFC) {
            edidBuffer.replace(i + 5, 13, QByteArray(13, '\0'));  // 将原名称部分清零
            // 然后插入新的显示器名称
            edidBuffer.replace(i + 5, nameData.size(), nameData);  // 替换名称
            break;
        }
    }

    // 更新校验和
    int checksum = 0;
    for (int i = 0; i < 127; ++i) {
        checksum += static_cast<unsigned char>(edidBuffer[i]);
    }
    edidBuffer[127] = static_cast<char>((256 - (checksum & 0xFF)) & 0xFF);

    return edidBuffer;
}





void Write_EDID(QByteArray& binBuffer, const QVector<EDID>& edidList) {
    for (const EDID& edid : edidList) {
        if (edid.addr + edid.len > binBuffer.size()) {
            qDebug() << "Invalid address or length for EDID, skipping.";
            continue;
        }
        binBuffer.replace(edid.addr, edid.len, edid.buffer);
        qDebug() << "EDID written back at position:" << edid.addr;
    }
}

QString EDID_Data_Convert_String(const EDID& edid)
{
    QString result;
    QByteArray buffer = edid.buffer;
    QStringList hexList;

    // 将每个字节转换为十六进制字符串
    for (unsigned char byte : buffer) {
        hexList.append(QString("0x%1").arg(byte, 2, 16, QChar('0')).toUpper());
    }

    // 每16个字节换行，并拼接结果
    for (int i = 0; i < hexList.size(); ++i) {
        result.append(hexList[i]);
        if (i < hexList.size() - 1) {
            result.append(", ");
        }

        // 每16个字节换行
        if ((i + 1) % 16 == 0) {
            result.append("\n");
        }
    }

    // 如果最后一行不足16个字节，手动添加换行
    if (!result.endsWith("\n")) {
        result.append("\n");
    }

    return result.trimmed(); // 移除末尾多余的换行符
}


