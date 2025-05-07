/*
 * @Author: Wayne 2546850503@qq.com
 * @Date: 2024-11-22 15:30:16
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-23 00:53:37
 * @FilePath: \HK_BIN_Tool\hk_bin_tool.cpp
 * @Description:
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */
#include "hk_bin_tool.h"
#include "./ui_hk_bin_tool.h"

#include "BIN_Data_Func.h"
#include "EDID_Func.h"
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>

QVector<EDID> Bin_EDID = {};
QVector<EDID_Info> Bin_EDID_Info = {};
QByteArray Bin_Buffer = {};
QByteArray Add_EDID_Buffer = {};
QString Bin_Filer = {};
QString Add_EDID_fileName = {};
int EDID_index = 0;

HK_BIN_Tool::HK_BIN_Tool(QWidget *parent) : QWidget(parent), ui(new Ui::HK_BIN_Tool)
{
    ui->setupUi(this);
    ui->Measure_comboBox->addItem("快捷选项");
    ui->Measure_comboBox->addItem("19.0");
    ui->Measure_comboBox->addItem("21.0");
    ui->Measure_comboBox->addItem("23.8");
    ui->Measure_comboBox->addItem("24.0");
    ui->Measure_comboBox->addItem("24.5");
    ui->Measure_comboBox->addItem("27.0");
    ui->Measure_comboBox->addItem("31.5");
    ui->Measure_comboBox->addItem("32.0");
    if (ui->Add_Edid_checkBox->isChecked())
    {
        ui->Add_Edid_pushButton->setEnabled(true);
        ui->Save_Edid_pushButton->setEnabled(true);
        ui->Save_Bin_pushButton->setEnabled(false);
    }
    else
    {
        ui->Add_Edid_pushButton->setEnabled(false);
        ui->Save_Edid_pushButton->setEnabled(false);
        ui->Save_Bin_pushButton->setEnabled(true);
    }

    ui->Resolution_comboBox->addItem("FHD");
    ui->Resolution_comboBox->addItem("QHD");
    ui->Resolution_comboBox->addItem("UHD");
    ui->Resolution_comboBox->addItem("特殊分辨率");

    ui->FPS_lineEdit->setText("60");
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
    EDID_index = 0;
    Bin_Filer = QFileDialog::getOpenFileName(this, "选择BIN文件", "", "Bin文件 (*.bin)");
    if (Bin_Filer.isEmpty())
        return;
    QFile file(Bin_Filer);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(nullptr, "错误", "无法打开文件: " + Bin_Filer);
        return;
    }
    // 读取文件中的所有数据
    Bin_Buffer = file.readAll();
    QString fileName = QFileInfo(Bin_Filer).fileName();
    file.close();
    ui->Bin_File_label->setText(fileName);

    /************* EDID *************/
    // 提取所有的 EDID 信息
    Bin_EDID = Find_EDID(Bin_Buffer);

    ui->EDID_textEdit->setText(EDID_Data_Convert_String(Bin_EDID[0]));
    ui->EDID_lineEdit->setText("0");

    // 获取当前 EDID
    const EDID &edid = Bin_EDID[EDID_index];
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
    ui->Red_X_lineEdit->setText(QString::number(edidInfo.redX, 'f', 4));
    ui->Red_Y_lineEdit->setText(QString::number(edidInfo.redY, 'f', 4));
    ui->Green_X_lineEdit->setText(QString::number(edidInfo.greenX, 'f', 4));
    ui->Green_Y_lineEdit->setText(QString::number(edidInfo.greenY, 'f', 4));
    ui->Blue_X_lineEdit->setText(QString::number(edidInfo.blueX, 'f', 4));
    ui->Blue_Y_lineEdit->setText(QString::number(edidInfo.blueY, 'f', 4));
    ui->White_X_lineEdit->setText(QString::number(edidInfo.whiteX, 'f', 4));
    ui->White_Y_lineEdit->setText(QString::number(edidInfo.whiteY, 'f', 4));

    /************* EDID *************/

    /************* BIN *************/

    bool State = Find_TargetString_InBinFile(Bin_Buffer, BinData_BackLightDef);
    if (State == true)
    {
        ui->_BACKLIGHT_MIN->setText(QString::number(BinData_BackLightDef.outputBuffer[0], 16).toUpper());
        ui->_BACKLIGHT_DEF_PWM->setText(QString::number(BinData_BackLightDef.outputBuffer[1], 16).toUpper());
        ui->_BACKLIGHT_MAX->setText(QString::number(BinData_BackLightDef.outputBuffer[2], 16).toUpper());
        ui->_MPRT_PWM_MIN->setText(QString::number(BinData_BackLightDef.outputBuffer[3], 16).toUpper());
        ui->_MPRT_PWM_DEF->setText(QString::number(BinData_BackLightDef.outputBuffer[4], 16).toUpper());
        ui->_MPRT_PWM_MAX->setText(QString::number(BinData_BackLightDef.outputBuffer[5], 16).toUpper());
    }
    State = Find_TargetString_InBinFile(Bin_Buffer, Key_Value_DataDef);
    if (State == true)
    {
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
        //ui->Bin_Data_Flag_label->setText("该BIN文件适用修改工具");
    }
    else
    {
        ui->Bin_Data_Flag_label->setText("该BIN文件不适用修改工具");
    }
    State = Find_TargetString_InBinFile(Bin_Buffer, HKC_Osd_DataDef);
    if(State == true)
    {
        bool ok;
        ui->HKC_BACKLIGHT_MIN->setText(QString::number(HKC_Osd_DataDef.outputBuffer[4], 16).toUpper());
        ui->HKC_BACKLIGHT_DEF_PWM->setText(QString::number(HKC_Osd_DataDef.outputBuffer[5], 16).toUpper());
        ui->HKC_BACKLIGHT_MAX->setText(QString::number(HKC_Osd_DataDef.outputBuffer[6], 16).toUpper());
        ui->HKC_Language_DEF->setText(QString::number(HKC_Osd_DataDef.outputBuffer[8], 10).toUpper());
        ui->HKC_Color_Temp_DEF->setText(QString::number(HKC_Osd_DataDef.outputBuffer[10], 10).toUpper());
        for (int var = 0; var < 14; ++var) {
            qDebug() << HKC_Osd_DataDef.outputBuffer[var];
        }
    }
    /*
    State = Find_TargetString_InBinFile(Bin_Buffer, Osd_DataDef);
    for (int var = 0; var < 10; ++var)
    {
        qDebug() << Osd_DataDef.outputBuffer[var];
    }
    */
    /******************************LOGO******************************/
    ui->LOGO_MESSAGE_textEdit->clear();
    State = Find_TargetString_InBinFile(Bin_Buffer, LOGO_BASE_Default_DataDef);
    if(State == true)
    {
        ui->_LOGO_FG_COLOR_lineEdit->setText(QString::number(LOGO_BASE_Default_DataDef.outputBuffer[0]));
        ui->_LOGO_BG_COLOR_lineEdit->setText(QString::number(LOGO_BASE_Default_DataDef.outputBuffer[1]));
        ui->_LOGO_COL_lineEdit->setText(QString::number(LOGO_BASE_Default_DataDef.outputBuffer[2]));
        ui->_LOGO_ROW_lineEdit->setText(QString::number(LOGO_BASE_Default_DataDef.outputBuffer[3]));
        ui->_LOGO_BG_RED_lineEdit->setText(QString::number(LOGO_BASE_Default_DataDef.outputBuffer[4]));
        ui->_LOGO_BG_GREEN_lineEdit->setText(QString::number(LOGO_BASE_Default_DataDef.outputBuffer[5]));
        ui->_LOGO_BG_BLUE_lineEdit->setText(QString::number(LOGO_BASE_Default_DataDef.outputBuffer[6]));
    }
    State = Find_TargetString_InBinFile(Bin_Buffer, LOGO_INDEX_DataDef);
    State = Find_TargetString_InBinFile(Bin_Buffer, LOGO1_INDEX_DataDef);
    /*
    for (int var = 0; var < 215; ++var)
    {
        qDebug() << LOGO_INDEX_DataDef.outputBuffer[var];
    }
    /*

    State = Find_TargetString_InBinFile(Bin_Buffer, LOGO_DataDef);
    for (int var = 0; var < 10; ++var)
    {
        qDebug() << LOGO_DataDef.outputBuffer[var];
    }
    */

    if (State == true)
        ui->BIN_LOGO_flag_label->setText("该BIN文件适用修改工具");
    else
        ui->BIN_LOGO_flag_label->setText("该BIN文件不适用修改工具");
    /******************************LOGO******************************/


    /************* BIN *************/
}

void HK_BIN_Tool::on_Next_EDID_Button_clicked()
{
    // 更新索引以指向下一个 EDID
    EDID_index++;

    int edidCount = Bin_EDID.size(); // 获取 Bin_EDID 中的 EDID 数量
    // 如果索引超出边界，则从头开始
    if (EDID_index >= edidCount)
    {
        EDID_index = 0;
    }
    ui->EDID_lineEdit->setText(QString::number(EDID_index));

    if (edidCount == 0)
    {
        qDebug() << "No EDID data available.";
        return; // 如果 Bin_EDID 为空，直接返回
    }
    // 显示当前 EDID 的数据
    ui->EDID_textEdit->setText(EDID_Data_Convert_String(Bin_EDID[EDID_index]));

    // 获取当前 EDID
    const EDID &edid = Bin_EDID[EDID_index];
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
    ui->Red_X_lineEdit->setText(QString::number(edidInfo.redX, 'f', 4));
    ui->Red_Y_lineEdit->setText(QString::number(edidInfo.redY, 'f', 4));
    ui->Green_X_lineEdit->setText(QString::number(edidInfo.greenX, 'f', 4));
    ui->Green_Y_lineEdit->setText(QString::number(edidInfo.greenY, 'f', 4));
    ui->Blue_X_lineEdit->setText(QString::number(edidInfo.blueX, 'f', 4));
    ui->Blue_Y_lineEdit->setText(QString::number(edidInfo.blueY, 'f', 4));
    ui->White_X_lineEdit->setText(QString::number(edidInfo.whiteX, 'f', 4));
    ui->White_Y_lineEdit->setText(QString::number(edidInfo.whiteY, 'f', 4));
}

/**
 * @description: 保存bin文件按键
 * @return {*}
 */
void HK_BIN_Tool::on_Save_Bin_pushButton_clicked()
{
    QString currentDate = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString originalPath = Bin_Filer;
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

    if (ui->DATA_checkBox->isChecked())
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
    if (ui->CODE_INC_checkBox->isChecked())
    {
        float CODE = ui->Measure_lineEdit->text().toFloat();
        int CODE_Int = static_cast<int>(CODE * 100);
        ui->CODE_lineEdit->setText(QString::number(CODE_Int));
    }
    if (ui->Manufacturer_checkBox->isChecked())
    {
        ui->Factory_lineEdit->setText("HKM");
    }
    if (ui->Monitor_name_checkBox->isChecked())
    {
        ui->InputName_lineEdit->setText("Monitor");
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
    edidInfo.redX = ui->Red_X_lineEdit->text().toFloat();
    edidInfo.redY = ui->Red_Y_lineEdit->text().toFloat();
    edidInfo.greenX = ui->Green_X_lineEdit->text().toFloat();
    edidInfo.greenY = ui->Green_Y_lineEdit->text().toFloat();
    edidInfo.blueX = ui->Blue_X_lineEdit->text().toFloat();
    edidInfo.blueY = ui->Blue_Y_lineEdit->text().toFloat();
    edidInfo.whiteX = ui->White_X_lineEdit->text().toFloat();
    edidInfo.whiteY = ui->White_Y_lineEdit->text().toFloat();

    // 修改每个 EDID（例如修改厂商信息）
    for (EDID &edid : Bin_EDID)
    {
        edid.buffer = Modify_EDID(edid.buffer, edidInfo);
    }
    // 将修改后的 EDID 写回 binBuffer
    Write_EDID(Bin_Buffer, Bin_EDID);
    /************* BIN *************/
    bool State = false;
    bool ok;
    State = Find_TargetString_InBinFile(Bin_Buffer, BinData_BackLightDef);
    if (State == true)
    {
        BinData_BackLightDef.outputBuffer[0] = ui->_BACKLIGHT_MIN->text().toInt(&ok, 16);
        BinData_BackLightDef.outputBuffer[1] = ui->_BACKLIGHT_DEF_PWM->text().toInt(&ok, 16);
        BinData_BackLightDef.outputBuffer[2] = ui->_BACKLIGHT_MAX->text().toInt(&ok, 16);
        BinData_BackLightDef.outputBuffer[3] = ui->_MPRT_PWM_MIN->text().toInt(&ok, 16);
        BinData_BackLightDef.outputBuffer[4] = ui->_MPRT_PWM_DEF->text().toInt(&ok, 16);
        BinData_BackLightDef.outputBuffer[5] = ui->_MPRT_PWM_MAX->text().toInt(&ok, 16);
        State = Write_TargetString_InBinFile(Bin_Buffer, BinData_BackLightDef);
    }
    State = Find_TargetString_InBinFile(Bin_Buffer, Key_Value_DataDef);
    if (State == true)
    {
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

        State = Write_TargetString_InBinFile(Bin_Buffer, Key_Value_DataDef);
        if (State == true)
            ui->Bin_Data_Flag_label->setText("Successful！");
        else
            ui->Bin_Data_Flag_label->setText("False！");
    }

    State = Find_TargetString_InBinFile(Bin_Buffer, HKC_Osd_DataDef);
    if(State == true)
    {
        HKC_Osd_DataDef.outputBuffer[4] = ui->HKC_BACKLIGHT_MIN->text().toInt(&ok, 16);
        HKC_Osd_DataDef.outputBuffer[5] = ui->HKC_BACKLIGHT_DEF_PWM->text().toInt(&ok, 16);
        HKC_Osd_DataDef.outputBuffer[6] = ui->HKC_BACKLIGHT_MAX->text().toInt(&ok, 16);
        HKC_Osd_DataDef.outputBuffer[8] = ui->HKC_Language_DEF->text().toInt(&ok, 10);
        HKC_Osd_DataDef.outputBuffer[10] = ui->HKC_Color_Temp_DEF->text().toInt(&ok, 10);
        State = Write_TargetString_InBinFile(Bin_Buffer, HKC_Osd_DataDef);

    }

    State = Find_TargetString_InBinFile(Bin_Buffer, LOGO_BASE_Default_DataDef);
    if(State == true)
    {
        bool ok;
        LOGO_BASE_Default_DataDef.outputBuffer[0] = ui->_LOGO_FG_COLOR_lineEdit->text().toInt(&ok, 10);
        LOGO_BASE_Default_DataDef.outputBuffer[1] = ui->_LOGO_BG_COLOR_lineEdit->text().toInt(&ok, 10);
        LOGO_BASE_Default_DataDef.outputBuffer[2] = ui->_LOGO_COL_lineEdit->text().toInt(&ok, 10);
        LOGO_BASE_Default_DataDef.outputBuffer[3] = ui->_LOGO_ROW_lineEdit->text().toInt(&ok, 10);
        LOGO_BASE_Default_DataDef.outputBuffer[4] = ui->_LOGO_BG_RED_lineEdit->text().toInt(&ok, 10);
        LOGO_BASE_Default_DataDef.outputBuffer[5] = ui->_LOGO_BG_GREEN_lineEdit->text().toInt(&ok, 10);
        LOGO_BASE_Default_DataDef.outputBuffer[6] = ui->_LOGO_BG_BLUE_lineEdit->text().toInt(&ok, 10);
        State = Write_TargetString_InBinFile(Bin_Buffer, LOGO_BASE_Default_DataDef);
        if (State == true)
            ui->BIN_LOGO_flag_label->setText("Successful！");
        else
            ui->BIN_LOGO_flag_label->setText("False！");
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
    if (!ok || diagonalInInches <= 0)
    {
        qDebug() << "无效的对角线尺寸（英寸）：" << arg1;
        ui->Measure_H_lineEdit->clear(); // 清空宽度 (cm)
        ui->Measure_V_lineEdit->clear(); // 清空高度 (cm)
        return;
    }

    // 预定义已知显示器数据：对角线尺寸 (英寸) -> 宽度 (cm), 高度 (cm)
    QMap<float, QPair<int, int>> knownData = {{19.0, {42, 24}}, {21.5, {48, 26}}, {24.0, {53, 30}},
                                              {24.5, {54, 31}}, {23.8, {53, 29}}, {27, {60, 33}},
                                              {28, {62, 35}},   {31.5, {70, 39}}, {32, {73, 36}}};

    // 检查是否有匹配的显示器尺寸
    if (knownData.contains(diagonalInInches))
    {
        int width = knownData[diagonalInInches].second; // 宽度 (cm)
        int height = knownData[diagonalInInches].first; // 高度 (cm)

        // 更新 UI 控件中的文本框
        ui->Measure_V_lineEdit->setText(QString::number(width));  // 宽度 (cm)
        ui->Measure_H_lineEdit->setText(QString::number(height)); // 高度 (cm)
    }
    else
    {
        // 如果没有匹配的数据，根据对角线尺寸计算宽高

        // 将对角线尺寸从英寸转换为厘米
        const float inchToCm = 2.54; // 英寸到厘米的转换因子
        float diagonalInCm = diagonalInInches * inchToCm;

        // 宽高比 (宽:高 = 16:9)
        const float aspectRatio = 16.0 / 9.0;

        // 根据对角线尺寸和宽高比计算宽度和高度（单位：厘米）
        float width = std::sqrt((diagonalInCm * diagonalInCm) / (1 + aspectRatio * aspectRatio)); // 宽度 (cm)
        float height = width * aspectRatio;                                                       // 高度 (cm)

        // 将宽度和高度转换为整数（四舍五入处理）
        int roundedWidth = static_cast<int>(std::round(width));
        int roundedHeight = static_cast<int>(std::round(height));

        // 更新 UI 控件中的文本框
        ui->Measure_V_lineEdit->setText(QString::number(roundedWidth));  // 宽度 (cm)
        ui->Measure_H_lineEdit->setText(QString::number(roundedHeight)); // 高度 (cm)
    }
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
    ui->CODE_lineEdit->setText(QString::number(CODE_Int * 100));
}

void HK_BIN_Tool::on_Add_Edid_checkBox_clicked(bool checked)
{
    if (ui->Add_Edid_checkBox->isChecked())
    {
        ui->Add_Edid_pushButton->setEnabled(true);
        ui->Save_Edid_pushButton->setEnabled(true);
        ui->Save_Bin_pushButton->setEnabled(false);
    }
    else
    {
        ui->Add_Edid_pushButton->setEnabled(false);
        ui->Save_Edid_pushButton->setEnabled(false);
        ui->Save_Bin_pushButton->setEnabled(true);
    }
}

void HK_BIN_Tool::on_Add_Edid_pushButton_clicked()
{
    // 当点击“添加”按钮时，弹出文件选择对话框，只允许选择后缀为txt或rtd的文件
    Add_EDID_fileName =
        QFileDialog::getOpenFileName(this, "选择文件", "", "EDID文本文件 (*.txt *.rtd)"); // 弹出文件选择对话框

    // 如果用户选择了文件，则进行后续处理
    if (!Add_EDID_fileName.isEmpty())
    {
        qDebug() << "选中的文件:" << Add_EDID_fileName; // 输出选中文件的路径
        // TODO: 在此处添加对选中文件的处理代码

        // 创建文件对象，并以只读方式打开文件
        QFile file(Add_EDID_fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            // 使用文本流读取文件内容
            QTextStream in(&file);
            QString Add_Edid = in.readAll(); // 读取所有数据
            ui->Add_Edid_textEdit->setText(Add_Edid);
            // 先处理原始字符串（保持你的原有逻辑）
            Add_Edid = Add_Edid.replace("0X", "", Qt::CaseInsensitive).replace(", ", "").replace("\n", "");

            // 将处理后的HEX字符串转为QByteArray
            Add_EDID_Buffer = QByteArray::fromHex(Add_Edid.toLatin1());

            // 验证结果（例如打印字节数组的16进制形式）
            qDebug() << "EDID Buffer:" << Add_EDID_Buffer.toHex().toUpper();

            qDebug() << Add_Edid;
            Bin_EDID[EDID_index].buffer = Add_EDID_Buffer;
        }
        else
        {
            qDebug() << "无法打开文件"; // 打开文件失败时输出提示信息
        }
    }
}

void HK_BIN_Tool::on_Save_Edid_pushButton_clicked()
{
    QString currentDate = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString originalPath = Bin_Filer;
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

    Write_EDID(Bin_Buffer, Bin_EDID);
    // 写入数据并关闭文件
    file.write(Bin_Buffer);
    file.close();

    // 保存成功提示
    QMessageBox::information(this, tr("成功"), tr("文件已保存到:\n%1").arg(newFileName));
}

void HK_BIN_Tool::on_checkBox_clicked(bool checked)
{
    if (checked == true)
    {
        if (ui->Resolution_comboBox->currentText() == "FHD")
        {
            ui->HBANK_lineEdit->setText("160");
            ui->VBANK_lineEdit->setText("41");
        }
        else if (ui->Resolution_comboBox->currentText() == "QHD")
        {
            ui->HBANK_lineEdit->setText("160");
            ui->VBANK_lineEdit->setText("41");
        }
        else if (ui->Resolution_comboBox->currentText() == "UHD")
        {
            ui->HBANK_lineEdit->setText("260");
            ui->VBANK_lineEdit->setText("45");
        }
    }
    else
    {
        ui->HBANK_lineEdit->clear();
        ui->VBANK_lineEdit->clear();
    }
}

void HK_BIN_Tool::on_Resolution_comboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == "FHD")
    {
        ui->H_lineEdit->setText("1920");
        ui->V_lineEdit->setText("1080");
        if (ui->H_V_checkBox->isChecked())
        {
            ui->HBANK_lineEdit->setText("160");
            ui->VBANK_lineEdit->setText("41");
        }
    }
    else if (arg1 == "QHD")
    {
        ui->H_lineEdit->setText("2560");
        ui->V_lineEdit->setText("1440");
        if (ui->H_V_checkBox->isChecked())
        {
            ui->HBANK_lineEdit->setText("160");
            ui->VBANK_lineEdit->setText("41");
        }
    }
    else if (arg1 == "UHD")
    {
        ui->H_lineEdit->setText("3840");
        ui->V_lineEdit->setText("2160");
        if (ui->H_V_checkBox->isChecked())
        {
            ui->HBANK_lineEdit->setText("280");
            ui->VBANK_lineEdit->setText("45");
        }
    }
    else
    {
        ui->H_lineEdit->clear();
        ui->V_lineEdit->clear();
        ui->HBANK_lineEdit->clear();
        ui->VBANK_lineEdit->clear();
    }
}

void HK_BIN_Tool::on_calculate_pushButton_clicked()
{
    ui->Clock_textEdit->clear();
    int H = ui->H_lineEdit->text().toInt();
    int V = ui->V_lineEdit->text().toInt();
    int H_BANK = ui->HBANK_lineEdit->text().toInt();
    int V_BANK = ui->VBANK_lineEdit->text().toInt();
    int FPS = ui->FPS_lineEdit->text().toInt();
    float NUM = ((H + H_BANK) * (V + V_BANK)) * FPS / 1000000.00;
    ui->BANDWIDTH_lineEdit->setText(QString::number(NUM, 'f', 2));

    int HZ_Aarray[] = {50, 60, 75, 100, 120, 144, 165, 180, 240, 280, 300, 320, 360, 400, 480, 500, 540, 600};
    for (int var = 0; var < 18; ++var)
    {
        float NUM_Clock = ((H + H_BANK) * (V + V_BANK)) * HZ_Aarray[var] / 1000000.00;
        QString HZ_NUM = QString::number(HZ_Aarray[var]);
        QString NUM_Clock_Str = QString::number(NUM_Clock, 'f', 2);
        QString ALL = HZ_NUM + "HZ" + " = " + NUM_Clock_Str + "MHz";
        ui->Clock_textEdit->append(ALL);
    }
}

void HK_BIN_Tool::on_MA_pushButton_clicked()
{
    // 安全读取输入值
    bool ok1, ok2, ok3, ok4;
    int MA_MAX = ui->MA_MAX_lineEdit->text().toInt(&ok1);
    int MAX = ui->MAX_lineEdit->text().toInt(&ok2, 16);
    int MA_TYPE = ui->MA_TYPE_lineEdit->text().toInt(&ok3);
    int TYPE = ui->TYPE_lineEdit->text().toInt(&ok4, 16);

    // 确保所有输入转换成功，并防止除数为 0
    if (!ok1 || !ok2 || !ok3 || !ok4 || (MAX == TYPE))
    {
        ui->MA_textEdit->append("输入值无效！");
        return;
    }

    // 计算比例，确保浮点除法
    float Rate = static_cast<float>(MAX - TYPE) / (MA_MAX - MA_TYPE);

    ui->MA_textEdit->clear(); // 清空旧结果

    for (int New_MA = 100; New_MA <= 750; New_MA += 10)
    {
        // 计算对应的 New_A 值
        double New_A = TYPE + (New_MA - MA_TYPE) * Rate;
        int New_A_Int = static_cast<int>(std::round(New_A));

        // 构造输出字符串
        QString hexValue = QString::number(New_A_Int, 16).toUpper();
        QString displayText = QString("0x%1 = %2 ma").arg(hexValue).arg(New_MA);

        // 输出到界面和调试控制台
        ui->MA_textEdit->append(displayText); // 显示到文本框
        qDebug() << displayText;              // 调试输出
    }
}

void HK_BIN_Tool::on_LOGO_SIZE_COL_ROW_pushButton_clicked()
{
    bool ok;
    int COL = ui->COL_lineEdit->text().toInt(&ok, 10);
    int ROW = ui->ROW_lineEdit->text().toInt(&ok, 10);

    int NEW_COL = COL/12 * 12;
    int NEW_ROW = ROW/18 * 18;

    ui->NEW_COL_lineEdit->setText(QString::number(NEW_COL));
    ui->NEW_ROW_lineEdit->setText(QString::number(NEW_ROW));
}

