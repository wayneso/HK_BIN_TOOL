 #include "BIN_Data_Func.h"


bool Find_TargetString_InBinFile(const QByteArray Bin_Buffer, Bin_Data_String &Bin_Data)
{
    bool State;

    // 查找目标字符串的位置
    Bin_Data.positions = Bin_Buffer.indexOf(Bin_Data.target_string);

    // 如果找到目标字符串
    if (Bin_Data.positions != -1) {
        qDebug() << "找到目标字符串, 起始位置:" << Bin_Data.positions;
        // 拷贝目标字符串到输出缓冲区
        std::memcpy(Bin_Data.outputBuffer, Bin_Buffer.mid(Bin_Data.positions + Bin_Data.offset, Bin_Data.outputBufferSize).data(), Bin_Data.outputBufferSize);

        State = true;  // 成功找到目标字符串

    } else {
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
    if (Bin_Data.positions != -1) {
        // 检查偏移量和输出缓冲区是否符合条件
        if (Bin_Data.positions + Bin_Data.offset + Bin_Data.outputBufferSize <= Bin_Buffer.size()) {
            // 替换数据
            memcpy(Bin_Buffer.data() + Bin_Data.positions + Bin_Data.offset,
                   Bin_Data.outputBuffer,
                   Bin_Data.outputBufferSize);

            State = true; // 标记操作成功
            qDebug() << "写入成功!";
        } else {
            qDebug() << "写入失败: 超出缓冲区范围!";
        }
    } else {
        qDebug() << "写入失败: 无效的起始位置!";
    }

    return State;
}

