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

    for (int i = 54; i <= 125; i += 18) {
        // 先判断 Descriptor Type == 0xFC (Monitor Name)
        unsigned char b0 = static_cast<unsigned char>(edidBuffer[i]);
        unsigned char b1 = static_cast<unsigned char>(edidBuffer[i+1]);
        unsigned char b2 = static_cast<unsigned char>(edidBuffer[i+2]);
        unsigned char b3 = static_cast<unsigned char>(edidBuffer[i+3]);
        if (b0==0x00 && b1==0x00 && b2==0x00 && b3==0xFC) {
            QByteArray nameData = edidBuffer.mid(i + 5, 13);  // 取文本区
            // 截断到换行符（LF）
            int lf = nameData.indexOf('\n');
            if (lf >= 0) nameData.truncate(lf);
            // 用 Latin1 解码以保留 ASCII 扩展
            info.monitorName = QString::fromLatin1(nameData);
            break;
        }
    }

    // 提取色度坐标 (CIE 1931 xy)
    unsigned char lsb1 = static_cast<unsigned char>(edidBuffer[0x19]); // 低位高 4 个值
    unsigned char lsb2 = static_cast<unsigned char>(edidBuffer[0x1A]); // 低位低 4 个值
    unsigned char msbRx = static_cast<unsigned char>(edidBuffer[0x1B]);
    unsigned char msbRy = static_cast<unsigned char>(edidBuffer[0x1C]);
    unsigned char msbGx = static_cast<unsigned char>(edidBuffer[0x1D]);
    unsigned char msbGy = static_cast<unsigned char>(edidBuffer[0x1E]);
    unsigned char msbBx = static_cast<unsigned char>(edidBuffer[0x1F]);
    unsigned char msbBy = static_cast<unsigned char>(edidBuffer[0x20]);
    unsigned char msbWx = static_cast<unsigned char>(edidBuffer[0x21]);
    unsigned char msbWy = static_cast<unsigned char>(edidBuffer[0x22]);

    // 分离 LSB
    unsigned char rxLsb = (lsb1 >> 6) & 0x03;
    unsigned char ryLsb = (lsb1 >> 4) & 0x03;
    unsigned char gxLsb = (lsb1 >> 2) & 0x03;
    unsigned char gyLsb =  lsb1        & 0x03;
    unsigned char bxLsb = (lsb2 >> 6) & 0x03;
    unsigned char byLsb = (lsb2 >> 4) & 0x03;
    unsigned char wxLsb = (lsb2 >> 2) & 0x03;
    unsigned char wyLsb =  lsb2        & 0x03;

    // 合并 MSB 和 LSB，并归一化
    auto toXY = [](unsigned char msb, unsigned char lsb) {
        int raw = (msb << 2) | lsb;
        return raw / 1024.0f;
    };

    info.redX   = toXY(msbRx, rxLsb);   // 红原色 x
    info.redY   = toXY(msbRy, ryLsb);   // 红原色 y
    info.greenX = toXY(msbGx, gxLsb);   // 绿原色 x
    info.greenY = toXY(msbGy, gyLsb);   // 绿原色 y
    info.blueX  = toXY(msbBx, bxLsb);   // 蓝原色 x
    info.blueY  = toXY(msbBy, byLsb);   // 蓝原色 y
    info.whiteX = toXY(msbWx, wxLsb);   // 白点   x
    info.whiteY = toXY(msbWy, wyLsb);   // 白点   y


    qDebug() << info.redX << info.redY;

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
    QByteArray nameData = edidInfo.monitorName.toLatin1().trimmed();  // Latin1 保留原字节
    if (nameData.size() > 12) nameData.truncate(12);

    for (int i = 54; i <= 125; i += 18) {
        unsigned char b0 = static_cast<unsigned char>(edidBuffer[i]);
        unsigned char b1 = static_cast<unsigned char>(edidBuffer[i+1]);
        unsigned char b2 = static_cast<unsigned char>(edidBuffer[i+2]);
        unsigned char b3 = static_cast<unsigned char>(edidBuffer[i+3]);
        if (b0==0x00 && b1==0x00 && b2==0x00 && b3==0xFC) {
            // 构造 13 字节名称区：名称 + LF + SP 填充
            QByteArray buf(13, '\x20');
            buf.replace(0, nameData.size(), nameData);
            buf[nameData.size()] = '\x0A';
            edidBuffer.replace(i + 5, 13, buf);
            break;
        }
    }

    // 修改色域坐标 (CIE 1931 xy)
    auto packXY = [&](float x, float y) {
        // 将浮点坐标转为 0-1023 之间的整数
        int rawX = qRound(x * 1024);
        int rawY = qRound(y * 1024);
        unsigned char msbX = (rawX >> 2) & 0xFF;
        unsigned char msbY = (rawY >> 2) & 0xFF;
        unsigned char lsbX = rawX & 0x03;
        unsigned char lsbY = rawY & 0x03;
        return std::tuple<unsigned char,unsigned char,unsigned char,unsigned char>(msbX, msbY, lsbX, lsbY);
    };
    // 打包各色度
    auto [msbRx, msbRy, rxLsb, ryLsb] = packXY(edidInfo.redX, edidInfo.redY);
    auto [msbGx, msbGy, gxLsb, gyLsb] = packXY(edidInfo.greenX, edidInfo.greenY);
    auto [msbBx, msbBy, bxLsb, byLsb] = packXY(edidInfo.blueX, edidInfo.blueY);
    auto [msbWx, msbWy, wxLsb, wyLsb] = packXY(edidInfo.whiteX, edidInfo.whiteY);

    // 写入低位字节
    unsigned char byte19 = (rxLsb << 6) | (ryLsb << 4) | (gxLsb << 2) | gyLsb;
    unsigned char byte1A = (bxLsb << 6) | (byLsb << 4) | (wxLsb << 2) | wyLsb;
    edidBuffer[0x19] = static_cast<char>(byte19);
    edidBuffer[0x1A] = static_cast<char>(byte1A);
    // 写入高位字节
    edidBuffer[0x1B] = static_cast<char>(msbRx);
    edidBuffer[0x1C] = static_cast<char>(msbRy);
    edidBuffer[0x1D] = static_cast<char>(msbGx);
    edidBuffer[0x1E] = static_cast<char>(msbGy);
    edidBuffer[0x1F] = static_cast<char>(msbBx);
    edidBuffer[0x20] = static_cast<char>(msbBy);
    edidBuffer[0x21] = static_cast<char>(msbWx);
    edidBuffer[0x22] = static_cast<char>(msbWy);


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

// void Replace_EDID(QByteArray& binBuffer, const QVector<EDID>& edid) {

//     //binBuffer.replace(edid.addr, edid.len, edid.buffer);


// }

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


