#include "BIN_Data_Func.h"


bool Find_TargetString_InBinFile(const QByteArray Bin_Buffer, Bin_Data_String &Bin_Data)
{
    bool State;

    // 查找目标字符串的位置
    int position = Bin_Buffer.indexOf(Bin_Data.target_string);

    // 如果找到目标字符串
    if (position != -1) {
        qDebug() << "找到目标字符串, 起始位置:" << position;
        // 拷贝目标字符串到输出缓冲区
        std::memcpy(Bin_Data.outputBuffer, Bin_Buffer.mid(position + Bin_Data.offset, Bin_Data.outputBufferSize).data(), Bin_Data.outputBufferSize);

        State = true;  // 成功找到目标字符串

    } else {
        State = false;
        qDebug() << "未找到目标字符串";
    }
    return State;
}
