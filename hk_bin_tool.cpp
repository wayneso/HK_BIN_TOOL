/*
 * @Author: Wayne 2546850503@qq.com
 * @Date: 2024-11-22 15:30:16
 * @LastEditors: Wayne 2546850503@qq.com
 * @LastEditTime: 2024-11-22 16:33:49
 * @FilePath: \HK_BIN_Tool\hk_bin_tool.cpp
 * @Description:
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */
#include "hk_bin_tool.h"
#include "./ui_hk_bin_tool.h"

#include "EDID_Func.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>


QVector<EDID> Bin_EDID = {};
QVector<EDID_Info> Bin_EDID_Info = {};
QByteArray Bin_Buffer = {};
QString filepath = {};
int EDID_index = 1;

uchar BacklightDef_Data_Buffer[6] = {0};
Bin_Data_String BinData_BackLightDef = {
    "This is BacklightDef Flag!",  // 目标字符串
    0,                             // 字符串初始位置
    27,                            // 偏移量
    BacklightDef_Data_Buffer,      // 输出缓冲区
    6                              // 输出缓冲区大小
};

uchar Key_Value_Data_Buffer[10] = {0};
Bin_Data_String Key_Value_DataDef = {
    "This is Keyboardflag Flag!",  // 目标字符串
    0,                             // 字符串初始位置
    27,                            // 偏移量
    Key_Value_Data_Buffer,      // 输出缓冲区
    10                              // 输出缓冲区大小
};

HK_BIN_Tool::HK_BIN_Tool(QWidget *parent) : QWidget(parent), ui(new Ui::HK_BIN_Tool)
{
    ui->setupUi(this);
    ui->Measure_comboBox->addItem("快捷选项");
    ui->Measure_comboBox->addItem("19.0");
    ui->Measure_comboBox->addItem("21.0");
    ui->Measure_comboBox->addItem("23.8");
    ui->Measure_comboBox->addItem("24.5");
    ui->Measure_comboBox->addItem("27.0");
    ui->Measure_comboBox->addItem("32.0");
}

HK_BIN_Tool::~HK_BIN_Tool()
{
    delete ui;
}

/**
 * @description: 加载bin文件按键
 * @return {*}
 */
void HK_BIN_Tool::on_Add_Bin_pushButton_clicked()
{
    filepath = QFileDialog::getOpenFileName(this, "选择BIN文件", "", "Bin文件 (*.bin)");
    if (filepath.isEmpty())
        return;
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(nullptr, "错误", "无法打开文件: " + filepath);
        return;
    }
    // 读取文件中的所有数据
    Bin_Buffer = file.readAll();
    QString fileName = QFileInfo(filepath).fileName();
    file.close();
    ui->Bin_File_label->setText(fileName);

    /************* EDID *************/
    // 提取所有的 EDID 信息
    Bin_EDID = Find_EDID(Bin_Buffer);
    ui->EDID_textEdit->setText(EDID_Data_Convert_String(Bin_EDID[0]));
    ui->EDID_lineEdit->setText("0");

    // 获取当前 EDID
    const EDID& edid = Bin_EDID[EDID_index];
    EDID_Info edidInfo = Read_EDID(edid.buffer);

    // 输出解析结果
    ui->Factory_lineEdit->setText(edidInfo.manufacturerID);
    ui->CODE_lineEdit->setText(edidInfo.productID);
    ui->EDIDVER_lineEdit->setText(edidInfo.version);
    ui->DATA_YEAR_lineEdit->setText(QString::number(edidInfo.manufactureYear));
    ui->DATA_WEEK_lineEdit->setText(QString::number(edidInfo.manufactureWeek));
    ui->Measure_H_lineEdit->setText(QString::number(edidInfo.horizontalSizeCm));
    ui->Measure_V_lineEdit->setText(QString::number(edidInfo.verticalSizeCm));
    ui->Measure_lineEdit->setText(QString::number(edidInfo.diagonalSizeInches, 'f', 1));
    ui->InputName_lineEdit->setText(edidInfo.monitorName);

    ui->Pixel_Clock->setText(QString::number(edidInfo.EDID_DTD.first().Pixel_Clock, 'f', 2));  // 保留两位小数
    ui->H_Adressable->setText(QString::number(edidInfo.EDID_DTD.first().Horizontal_Active_Pixels));
    ui->H_Blanking->setText(QString::number(edidInfo.EDID_DTD.first().Horizontal_Blanking_Pixels));
    ui->V_Adressable->setText(QString::number(edidInfo.EDID_DTD.first().Vertical_Active_Pixels));
    ui->V_Blanking->setText(QString::number(edidInfo.EDID_DTD.first().Vertical_Blanking_Pixels));
    ui->H_Front_Porch->setText(QString::number(edidInfo.EDID_DTD.first().Horizontal_Sync_Offset));
    ui->H_Sync_Width->setText(QString::number(edidInfo.EDID_DTD.first().Horizontal_Sync_Width));
    ui->V_Front_Porch->setText(QString::number(edidInfo.EDID_DTD.first().Vertical_Sync_Offset));
    ui->V_Sync_Width->setText(QString::number(edidInfo.EDID_DTD.first().Vertical_Sync_Width));

    float FPS_HZ = (edidInfo.EDID_DTD.first().Pixel_Clock * 1000000) / ((edidInfo.EDID_DTD.first().Horizontal_Active_Pixels + edidInfo.EDID_DTD.first().Horizontal_Blanking_Pixels) * ((edidInfo.EDID_DTD.first().Vertical_Active_Pixels) + edidInfo.EDID_DTD.first().Vertical_Blanking_Pixels));
    ui->FPS_HZ->setText(QString::number(FPS_HZ, 'f', 2));

    /************* EDID *************/

    /************* BIN *************/

    bool State = Find_TargetString_InBinFile(Bin_Buffer,BinData_BackLightDef);
    ui->_BACKLIGHT_MIN->setText(QString::number(BinData_BackLightDef.outputBuffer[0], 16).toUpper());
    ui->_BACKLIGHT_DEF_PWM->setText(QString::number(BinData_BackLightDef.outputBuffer[1], 16).toUpper());
    ui->_BACKLIGHT_MAX->setText(QString::number(BinData_BackLightDef.outputBuffer[2], 16).toUpper());
    ui->_MPRT_PWM_MIN->setText(QString::number(BinData_BackLightDef.outputBuffer[3], 16).toUpper());
    ui->_MPRT_PWM_DEF->setText(QString::number(BinData_BackLightDef.outputBuffer[4], 16).toUpper());
    ui->_MPRT_PWM_MAX->setText(QString::number(BinData_BackLightDef.outputBuffer[5], 16).toUpper());

    State = Find_TargetString_InBinFile(Bin_Buffer,Key_Value_DataDef);
    ui->POWER_KEY->setText(QString::number(Key_Value_DataDef.outputBuffer[0], 16).toUpper());
    ui->MENU_KEY->setText(QString::number(Key_Value_DataDef.outputBuffer[1], 16).toUpper());
    ui->EXIT_KEY->setText(QString::number(Key_Value_DataDef.outputBuffer[2], 16).toUpper());
    ui->RIGHT_KEY->setText(QString::number(Key_Value_DataDef.outputBuffer[3], 16).toUpper());
    ui->LEFT_KEY->setText(QString::number(Key_Value_DataDef.outputBuffer[4], 16).toUpper());
    ui->POWER_REG->setText(QString::number(Key_Value_DataDef.outputBuffer[5], 16).toUpper());
    ui->MENU_REG->setText(QString::number(Key_Value_DataDef.outputBuffer[6], 16).toUpper());
    ui->EXIT_REG->setText(QString::number(Key_Value_DataDef.outputBuffer[7], 16).toUpper());
    ui->RIGHT_REG->setText(QString::number(Key_Value_DataDef.outputBuffer[8], 16).toUpper());
    ui->LEFT_REG->setText(QString::number(Key_Value_DataDef.outputBuffer[9], 16).toUpper());

    if (State == true)
    {
        ui->Bin_Data_Flag_label->setText("该BIN文件适用修改工具");
    }
    else
    {
        ui->Bin_Data_Flag_label->setText("该BIN文件不适用修改工具");
        ui->_BACKLIGHT_MIN->setText("0");
        ui->_BACKLIGHT_DEF_PWM->setText("0");
        ui->_BACKLIGHT_MAX->setText("0");
        ui->_MPRT_PWM_MIN->setText("0");
        ui->_MPRT_PWM_DEF->setText("0");
        ui->_MPRT_PWM_MAX->setText("0");
        ui->POWER_KEY->setText("0");
        ui->MENU_KEY->setText("0");
        ui->EXIT_KEY->setText("0");
        ui->RIGHT_KEY->setText("0");
        ui->LEFT_KEY->setText("0");
        ui->POWER_REG->setText("0");
        ui->MENU_REG->setText("0");
        ui->EXIT_REG->setText("0");
        ui->RIGHT_REG->setText("0");
        ui->LEFT_REG->setText("0");
    }


    /************* BIN *************/
}

void HK_BIN_Tool::on_Next_EDID_Button_clicked()
{
    int edidCount = Bin_EDID.size();  // 获取 Bin_EDID 中的 EDID 数量

    ui->EDID_lineEdit->setText(QString::number(EDID_index));

    if (edidCount == 0) {
        qDebug() << "No EDID data available.";
        return;  // 如果 Bin_EDID 为空，直接返回
    }

    // 如果索引超出边界，则从头开始
    if (EDID_index >= edidCount) {
        EDID_index = 0;
    }

    // 显示当前 EDID 的数据
    ui->EDID_textEdit->setText(EDID_Data_Convert_String(Bin_EDID[EDID_index]));

    // 获取当前 EDID
    const EDID& edid = Bin_EDID[EDID_index];
    EDID_Info edidInfo = Read_EDID(edid.buffer);

    ui->Factory_lineEdit->setText(edidInfo.manufacturerID);
    ui->CODE_lineEdit->setText(edidInfo.productID);
    ui->EDIDVER_lineEdit->setText(edidInfo.version);
    ui->DATA_YEAR_lineEdit->setText(QString::number(edidInfo.manufactureYear));
    ui->DATA_WEEK_lineEdit->setText(QString::number(edidInfo.manufactureWeek));
    ui->Measure_H_lineEdit->setText(QString::number(edidInfo.horizontalSizeCm));
    ui->Measure_V_lineEdit->setText(QString::number(edidInfo.verticalSizeCm));
    ui->Measure_lineEdit->setText(QString::number(edidInfo.diagonalSizeInches, 'f', 1));
    ui->InputName_lineEdit->setText(edidInfo.monitorName);

    ui->Pixel_Clock->setText(QString::number(edidInfo.EDID_DTD.first().Pixel_Clock, 'f', 2));  // 保留两位小数
    ui->H_Adressable->setText(QString::number(edidInfo.EDID_DTD.first().Horizontal_Active_Pixels));
    ui->H_Blanking->setText(QString::number(edidInfo.EDID_DTD.first().Horizontal_Blanking_Pixels));
    ui->V_Adressable->setText(QString::number(edidInfo.EDID_DTD.first().Vertical_Active_Pixels));
    ui->V_Blanking->setText(QString::number(edidInfo.EDID_DTD.first().Vertical_Blanking_Pixels));
    ui->H_Front_Porch->setText(QString::number(edidInfo.EDID_DTD.first().Horizontal_Sync_Offset));
    ui->H_Sync_Width->setText(QString::number(edidInfo.EDID_DTD.first().Horizontal_Sync_Width));
    ui->V_Front_Porch->setText(QString::number(edidInfo.EDID_DTD.first().Vertical_Sync_Offset));
    ui->V_Sync_Width->setText(QString::number(edidInfo.EDID_DTD.first().Vertical_Sync_Width));

    // 更新索引以指向下一个 EDID
    EDID_index++;
}


/**
 * @description: 保存bin文件按键
 * @return {*}
 */
void HK_BIN_Tool::on_Save_Bin_pushButton_clicked()
{
    QString currentDate = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString originalPath = filepath;
    // 提取文件名和路径
    QFileInfo fileInfo(originalPath);
    QString dirPath = fileInfo.absolutePath();      // 获取文件所在目录
    QString baseName = fileInfo.completeBaseName(); // 文件名（不含扩展名）
    QString extension = fileInfo.suffix();          // 文件扩展名

    // 生成新文件名
    QString newFileName = QString("%1/%2_%3.%4").arg(dirPath).arg(currentDate).arg(baseName).arg(extension);

    // 打开文件并写入数据
    QFile file(newFileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        // 如果打开失败，可以记录日志或者执行其他错误处理操作
        qDebug() << "Failed to open file for writing:" << newFileName;
        return;
    }

    bool DATA_checkBox_isChecked = ui->DATA_checkBox->isChecked();
    bool CODE_INC_checkBox_isChecked = ui->CODE_INC_checkBox->isChecked();
    if (DATA_checkBox_isChecked)
    {
        // 获取当前日期
        QDate currentDate = QDate::currentDate();

        // 获取当前年份
        int currentYear = currentDate.year();

        // 获取当前周数，注意：默认周一为一周的开始
        int currentWeek = currentDate.weekNumber();

        // 将当前年份和周数写入到文本框
        ui->DATA_YEAR_lineEdit->setText(QString::number(currentYear));
        ui->DATA_WEEK_lineEdit->setText(QString::number(currentWeek));
    }
    if(CODE_INC_checkBox_isChecked)
    {
        float CODE = ui->Measure_lineEdit->text().toFloat();
        int CODE_Int = static_cast<int>(std::round(CODE));
        ui->CODE_lineEdit->setText(QString::number(CODE_Int*100));
    }
    EDID_Info edidInfo;
    edidInfo.horizontalSizeCm = ui->Measure_H_lineEdit->text().toFloat();
    edidInfo.verticalSizeCm = ui->Measure_V_lineEdit->text().toFloat();
    edidInfo.diagonalSizeInches = ui->Measure_lineEdit->text().toFloat();
    edidInfo.manufactureWeek = ui->DATA_WEEK_lineEdit->text().toInt();
    edidInfo.manufactureYear = ui->DATA_YEAR_lineEdit->text().toInt();
    edidInfo.manufacturerID = ui->Factory_lineEdit->text();
    edidInfo.productID = ui->CODE_lineEdit->text();
    edidInfo.monitorName = ui->InputName_lineEdit->text();
    edidInfo.version = ui->EDIDVER_lineEdit->text();
    // 修改每个 EDID（例如修改厂商信息）
    for (EDID& edid : Bin_EDID) {
        edid.buffer = Modify_EDID(edid.buffer, edidInfo);
    }
    // 将修改后的 EDID 写回 binBuffer
    Write_EDID(Bin_Buffer, Bin_EDID);
    /************* BIN *************/
    bool State = false;
    State = Find_TargetString_InBinFile(Bin_Buffer, BinData_BackLightDef);
    State = Find_TargetString_InBinFile(Bin_Buffer, Key_Value_DataDef);
    if (State == true)
    {
        bool ok;
        BinData_BackLightDef.outputBuffer[0] = ui->_BACKLIGHT_MIN->text().toInt(&ok, 16);
        BinData_BackLightDef.outputBuffer[1] = ui->_BACKLIGHT_DEF_PWM->text().toInt(&ok, 16);
        BinData_BackLightDef.outputBuffer[2] = ui->_BACKLIGHT_MAX->text().toInt(&ok, 16);
        BinData_BackLightDef.outputBuffer[3] = ui->_MPRT_PWM_MIN->text().toInt(&ok, 16);
        BinData_BackLightDef.outputBuffer[4] = ui->_MPRT_PWM_DEF->text().toInt(&ok, 16);
        BinData_BackLightDef.outputBuffer[5] = ui->_MPRT_PWM_MAX->text().toInt(&ok, 16);

        Key_Value_DataDef.outputBuffer[0] = ui->POWER_KEY->text().toInt(&ok, 16);
        Key_Value_DataDef.outputBuffer[1] = ui->MENU_KEY->text().toInt(&ok, 16);
        Key_Value_DataDef.outputBuffer[2] = ui->EXIT_KEY->text().toInt(&ok, 16);
        Key_Value_DataDef.outputBuffer[3] = ui->RIGHT_KEY->text().toInt(&ok, 16);
        Key_Value_DataDef.outputBuffer[4] = ui->LEFT_KEY->text().toInt(&ok, 16);
        Key_Value_DataDef.outputBuffer[5] = ui->POWER_REG->text().toInt(&ok, 16);
        Key_Value_DataDef.outputBuffer[6] = ui->MENU_REG->text().toInt(&ok, 16);
        Key_Value_DataDef.outputBuffer[7] = ui->EXIT_REG->text().toInt(&ok, 16);
        Key_Value_DataDef.outputBuffer[8] = ui->RIGHT_REG->text().toInt(&ok, 16);
        Key_Value_DataDef.outputBuffer[9] = ui->LEFT_REG->text().toInt(&ok, 16);

        State = Write_TargetString_InBinFile(Bin_Buffer, BinData_BackLightDef);
        State = Write_TargetString_InBinFile(Bin_Buffer, Key_Value_DataDef);
        if (State == true)
            ui->Bin_Data_Flag_label->setText("Successful！");
        else
            ui->Bin_Data_Flag_label->setText("False！");
    }
    /************* BIN *************/
    // 写入数据并关闭文件
    file.write(Bin_Buffer);
    file.close();

    // 保存成功提示
    QMessageBox::information(this, tr("成功"), tr("文件已保存到:\n%1").arg(newFileName));
}



void HK_BIN_Tool::on_Measure_comboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 != "快捷选项")
        ui->Measure_lineEdit->setText(arg1);
}

void HK_BIN_Tool::on_Measure_lineEdit_textChanged(const QString &arg1)
{
    // 将 arg1 转换为浮点数（输入的对角线尺寸，单位：英寸）
    bool ok = false;
    float diagonalInInches = arg1.toFloat(&ok);

    // 如果输入无效（非数字或负数/零），打印错误信息并清空相关文本框
    if (!ok || diagonalInInches <= 0) {
        qDebug() << "无效的对角线尺寸（英寸）：" << arg1;
        ui->Measure_H_lineEdit->clear();  // 清空宽度 (cm)
        ui->Measure_V_lineEdit->clear();  // 清空高度 (cm)
        return;
    }

    // 将对角线尺寸从英寸转换为厘米
    const float inchToCm = 2.54;  // 英寸到厘米的转换因子
    float diagonalInCm = diagonalInInches * inchToCm;

    // 宽高比 (宽:高 = 16:9)
    const float aspectRatio = 16.0 / 9.0;

    // 根据对角线尺寸和宽高比计算宽度和高度（单位：厘米）
    float width = std::sqrt((diagonalInCm * diagonalInCm) / (1 + aspectRatio * aspectRatio)); // 宽度 (cm)
    float height = width * aspectRatio;  // 高度 (cm)

    // 将宽度和高度转换为整数（四舍五入处理）
    int roundedWidth = static_cast<int>(std::round(width));
    int roundedHeight = static_cast<int>(std::round(height));

    // 更新 UI 控件中的文本框
    ui->Measure_V_lineEdit->setText(QString::number(roundedWidth));    // 宽度 (cm)
    ui->Measure_H_lineEdit->setText(QString::number(roundedHeight));   // 高度 (cm)
}


void HK_BIN_Tool::on_DATA_checkBox_clicked(bool checked)
{
    // 获取当前日期
    QDate currentDate = QDate::currentDate();

    // 获取当前年份
    int currentYear = currentDate.year();

    // 获取当前周数，注意：默认周一为一周的开始
    int currentWeek = currentDate.weekNumber();

    // 将当前年份和周数写入到文本框
    ui->DATA_YEAR_lineEdit->setText(QString::number(currentYear));
    ui->DATA_WEEK_lineEdit->setText(QString::number(currentWeek));
}

void HK_BIN_Tool::on_CODE_INC_checkBox_clicked(bool checked)
{
    float CODE = ui->Measure_lineEdit->text().toFloat();
    int CODE_Int = static_cast<int>(std::round(CODE));
    ui->CODE_lineEdit->setText(QString::number(CODE_Int*100));

}

void HK_BIN_Tool::Bandwidth_Calculation()
{
    float FPS_HZ = ui->FPS_HZ->text().toFloat();
    int H_Adressable = ui->H_Adressable->text().toInt();
    int H_Blanking = ui->H_Blanking->text().toInt();
    int V_Adressable = ui->V_Adressable->text().toInt();
    int V_Blanking = ui->V_Blanking->text().toInt();
    float Pixel_Clock = (FPS_HZ * (H_Adressable + H_Blanking) * (V_Adressable + V_Blanking)) / 1000000;
    ui->Pixel_Clock->setText(QString::number(Pixel_Clock, 'f', 2));  // 保留两位小数
}
void HK_BIN_Tool::on_FPS_HZ_textChanged(const QString &arg1)
{
    Bandwidth_Calculation();

}

void HK_BIN_Tool::on_H_Adressable_textChanged(const QString &arg1)
{
    Bandwidth_Calculation();
}


void HK_BIN_Tool::on_H_Blanking_textChanged(const QString &arg1)
{
    Bandwidth_Calculation();
}


void HK_BIN_Tool::on_V_Adressable_textChanged(const QString &arg1)
{
    Bandwidth_Calculation();
}


void HK_BIN_Tool::on_V_Blanking_textChanged(const QString &arg1)
{
    Bandwidth_Calculation();
}

