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
#include <QRegularExpression>
#include <QStack>

QVector<EDID> Bin_EDID = {};
QVector<EDID_Info> Bin_EDID_Info = {};
QByteArray Bin_Buffer = {};
QByteArray Add_EDID_Buffer = {};
QString Bin_Filer = {};
QString Add_EDID_fileName = {};
int EDID_index = 0;
QMap<QString, QMap<QString, QString>> keyConditionData;
QStringList keyConditionOrder;  // 按照出现顺序记录条件
QMap<QString, QString> Logo_Base_Default;
QList<QString> Logo_Base_Default_valueList;
QVector<quint8> Logo_paletteData;
QVector<quint8> Logo_Data;

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
    ui->Resolution_comboBox->addItem("WQHD");
    ui->Resolution_comboBox->addItem("UHD");
    ui->Resolution_comboBox->addItem("特殊分辨率");

    ui->FPS_lineEdit->setText("60");
}

HK_BIN_Tool::~HK_BIN_Tool()
{
    delete ui;
}
bool HK_BIN_Tool::BIN_Find_Func(const QByteArray &bin,
                                Bin_Data_String &Bin_Data_String,
                                QLineEdit *edit,
                                int index,
                                QTextEdit *msgEdit,
                                const QString &successMsg,
                                const QString &failMsg,
                                int base)
{
    if (Find_TargetString_InBinFile(bin, Bin_Data_String))
    {
        if (edit && index < Bin_Data_String.outputBufferSize)
            edit->setText(QString::number(static_cast<int>(Bin_Data_String.outputBuffer[index]), base).toUpper());

        if (msgEdit && !successMsg.isEmpty())
            msgEdit->append(successMsg);

        return true;
    }
    else
    {
        if (msgEdit && !failMsg.isEmpty())
            msgEdit->append(failMsg);

        return false;
    }
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
    bool State = false;
    //背光值
    BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_BACKLIGHT_MIN, 0, ui->FUNC_textEdit, "支持修改电流", "不支持修改电流", 16);
    BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_BACKLIGHT_DEF_PWM, 1, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_BACKLIGHT_MAX, 2, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_MPRT_PWM_MIN, 3, ui->FUNC_textEdit, "支持修改MPRT", "不支持修改MPRT", 16);
    BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_MPRT_PWM_DEF, 4, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_MPRT_PWM_MAX, 5, ui->FUNC_textEdit, "", "", 16);
    //按键
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->POWER_KEY, 0, ui->FUNC_textEdit, "支持修改按键", "不支持修改按键", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->MENU_KEY, 1, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->EXIT_KEY, 2, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->RIGHT_KEY, 3, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->LEFT_KEY, 4, ui->FUNC_textEdit, "", "", 16);

    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->POWER_REG, 5, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->MENU_REG, 6, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->EXIT_REG, 7, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->RIGHT_REG, 8, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->LEFT_REG, 9, ui->FUNC_textEdit, "", "", 16);

    State = Find_TargetString_InBinFile(Bin_Buffer, Osd_DataDef);
    // if(State == true)
    // {
    //     bool ok;
    //     for (int var = 0; var < 10; ++var) {
    //         qDebug()<< Osd_DataDef.outputBuffer[var];
    //     }

    // }

    BIN_Find_Func(Bin_Buffer, HKC_Osd_DataDef, ui->HKC_BACKLIGHT_MIN, 4, ui->FUNC_textEdit, "支持修改2383电流", "不支持修改2383电流", 16);
    BIN_Find_Func(Bin_Buffer, HKC_Osd_DataDef, ui->HKC_BACKLIGHT_DEF_PWM, 5, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, HKC_Osd_DataDef, ui->HKC_BACKLIGHT_MAX, 6, ui->FUNC_textEdit, "", "", 16);

    BIN_Find_Func(Bin_Buffer, HKC_Osd_DataDef, ui->HKC_Language_DEF, 8, ui->FUNC_textEdit, "支持修改2383语言", "不支持修改2383语言", 10);
    BIN_Find_Func(Bin_Buffer, HKC_Osd_DataDef, ui->HKC_Color_Temp_DEF, 10, ui->FUNC_textEdit, "支持修改2383色温定义", "不支持修改2383色温定义", 10);

    BIN_Find_Func(Bin_Buffer, HKC_TEMP_COLOR_DataDef, ui->_CT9300_RED, 1, ui->FUNC_textEdit, "支持修改2383色温值", "不支持修改2383色温值", 10);
    BIN_Find_Func(Bin_Buffer, HKC_TEMP_COLOR_DataDef, ui->_CT9300_GREEN, 3, ui->FUNC_textEdit, "", "", 10);
    BIN_Find_Func(Bin_Buffer, HKC_TEMP_COLOR_DataDef, ui->_CT9300_BLUE, 5, ui->FUNC_textEdit, "", "", 10);
    BIN_Find_Func(Bin_Buffer, HKC_TEMP_COLOR_DataDef, ui->_CT6500_RED, 7, ui->FUNC_textEdit, "", "", 10);
    BIN_Find_Func(Bin_Buffer, HKC_TEMP_COLOR_DataDef, ui->_CT6500_GREEN, 9, ui->FUNC_textEdit, "", "", 10);
    BIN_Find_Func(Bin_Buffer, HKC_TEMP_COLOR_DataDef, ui->_CT6500_BLUE, 11, ui->FUNC_textEdit, "", "", 10);
    BIN_Find_Func(Bin_Buffer, HKC_TEMP_COLOR_DataDef, ui->_CTUSER_RED, 13, ui->FUNC_textEdit, "", "", 10);
    BIN_Find_Func(Bin_Buffer, HKC_TEMP_COLOR_DataDef, ui->_CTUSER_GREEN, 15, ui->FUNC_textEdit, "", "", 10);
    BIN_Find_Func(Bin_Buffer, HKC_TEMP_COLOR_DataDef, ui->_CTUSER_BLUE, 17, ui->FUNC_textEdit, "", "", 10);


    /******************************LOGO******************************/
    ui->LOGO_MESSAGE_textEdit->clear();
    BIN_Find_Func(Bin_Buffer, LOGO_BASE_Default_DataDef, nullptr, 0, ui->LOGO_MESSAGE_textEdit, "支持修改LOGO!", "不支持修改LOGO!", 16);

    State = BIN_Find_Func(Bin_Buffer, LOGO_2383_BASE_Default_DataDef, nullptr, 0, ui->LOGO_MESSAGE_textEdit, "支持修改2383LOGO!", "不支持修改2383LOGO!", 16);
    BIN_Find_Func(Bin_Buffer, LOGO_2383_BASE_Default1_DataDef, nullptr, 0, ui->LOGO_MESSAGE_textEdit, "", "", 16);
    if(State == true)
    {
        ui->RTD2383_Project_checkBox->setCheckState(Qt::Checked);
        ui->_OSD_REG_MAPPING_GEN_lineEdit->setText("_USER_OSD_GEN_1");
    }

    BIN_Find_Func(Bin_Buffer, LOGO_Palette_DataDef, nullptr, 0, ui->LOGO_MESSAGE_textEdit, "支持修改LOGO色盘", "不支持修改LOGO色盘", 16);
    BIN_Find_Func(Bin_Buffer, LOGO_DataDef, nullptr, 0, ui->LOGO_MESSAGE_textEdit, "支持修改LOGO_Data", "不支持修改LOGO_Data", 16);

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
    if (ui->sRGB_DCI_P3_checkBox->isChecked())
    {
        ui->Red_X_lineEdit->setText("0.6400");
        ui->Red_Y_lineEdit->setText("0.3300");
        ui->Green_X_lineEdit->setText("0.3000");
        ui->Green_Y_lineEdit->setText("0.6000");
        ui->Blue_X_lineEdit->setText("0.1500");
        ui->Blue_Y_lineEdit->setText("0.0600");
        ui->White_X_lineEdit->setText("0.3127");
        ui->White_Y_lineEdit->setText("0.3290");
    }
    else
    {
        ui->Red_X_lineEdit->setText("0.6800");
        ui->Red_Y_lineEdit->setText("0.3200");
        ui->Green_X_lineEdit->setText("0.2650");
        ui->Green_Y_lineEdit->setText("0.6900");
        ui->Blue_X_lineEdit->setText("0.1500");
        ui->Blue_Y_lineEdit->setText("0.0600");
        ui->White_X_lineEdit->setText("0.3140");
        ui->White_Y_lineEdit->setText("0.3510");
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
    // State = Find_TargetString_InBinFile(Bin_Buffer, BinData_BackLightDef);
    // if (State == true)
    // {
    //     BinData_BackLightDef.outputBuffer[0] = ui->_BACKLIGHT_MIN->text().toInt(&ok, 16);
    //     BinData_BackLightDef.outputBuffer[1] = ui->_BACKLIGHT_DEF_PWM->text().toInt(&ok, 16);
    //     BinData_BackLightDef.outputBuffer[2] = ui->_BACKLIGHT_MAX->text().toInt(&ok, 16);
    //     BinData_BackLightDef.outputBuffer[3] = ui->_MPRT_PWM_MIN->text().toInt(&ok, 16);
    //     BinData_BackLightDef.outputBuffer[4] = ui->_MPRT_PWM_DEF->text().toInt(&ok, 16);
    //     BinData_BackLightDef.outputBuffer[5] = ui->_MPRT_PWM_MAX->text().toInt(&ok, 16);
    //     State = Write_TargetString_InBinFile(Bin_Buffer, BinData_BackLightDef);
    //     if (State == true)
    //         ui->FUNC_textEdit->append("电流值修改成功");
    //     else
    //         ui->FUNC_textEdit->setText("电流值修改失败");
    // }
    // State = Find_TargetString_InBinFile(Bin_Buffer, Key_Value_DataDef);
    // if (State == true)
    // {
    //     Key_Value_DataDef.outputBuffer[0] = ui->POWER_KEY->text().toInt(&ok, 16);
    //     Key_Value_DataDef.outputBuffer[1] = ui->MENU_KEY->text().toInt(&ok, 16);
    //     Key_Value_DataDef.outputBuffer[2] = ui->EXIT_KEY->text().toInt(&ok, 16);
    //     Key_Value_DataDef.outputBuffer[3] = ui->RIGHT_KEY->text().toInt(&ok, 16);
    //     Key_Value_DataDef.outputBuffer[4] = ui->LEFT_KEY->text().toInt(&ok, 16);

    //     Key_Value_DataDef.outputBuffer[5] = ui->POWER_REG->text().toInt(&ok, 16);
    //     Key_Value_DataDef.outputBuffer[6] = ui->MENU_REG->text().toInt(&ok, 16);
    //     Key_Value_DataDef.outputBuffer[7] = ui->EXIT_REG->text().toInt(&ok, 16);
    //     Key_Value_DataDef.outputBuffer[8] = ui->RIGHT_REG->text().toInt(&ok, 16);
    //     Key_Value_DataDef.outputBuffer[9] = ui->LEFT_REG->text().toInt(&ok, 16);

    //     State = Write_TargetString_InBinFile(Bin_Buffer, Key_Value_DataDef);
    //     if (State == true)
    //         ui->FUNC_textEdit->append("键值修改成功");
    //     else
    //         ui->FUNC_textEdit->setText("键值修改失败");
    // }

    //背光值
    State = BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_BACKLIGHT_MIN, 0, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_BACKLIGHT_DEF_PWM, 1, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_BACKLIGHT_MAX, 2, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_MPRT_PWM_MIN, 3, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_MPRT_PWM_DEF, 4, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, BinData_BackLightDef, ui->_MPRT_PWM_MAX, 5, ui->FUNC_textEdit, "", "", 16);
    if (State == true)
    {
        State = Write_TargetString_InBinFile(Bin_Buffer, BinData_BackLightDef);
        if (State == true)
            ui->FUNC_textEdit->append("电流值修改成功");
        else
            ui->FUNC_textEdit->setText("电流值修改失败");
    }
    //按键
    State = BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->POWER_KEY, 0, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->MENU_KEY, 1, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->EXIT_KEY, 2, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->RIGHT_KEY, 3, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->LEFT_KEY, 4, ui->FUNC_textEdit, "", "", 16);

    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->POWER_REG, 5, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->MENU_REG, 6, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->EXIT_REG, 7, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->RIGHT_REG, 8, ui->FUNC_textEdit, "", "", 16);
    BIN_Find_Func(Bin_Buffer, Key_Value_DataDef, ui->LEFT_REG, 9, ui->FUNC_textEdit, "", "", 16);

    if (State == true)
    {
        State = Write_TargetString_InBinFile(Bin_Buffer, Key_Value_DataDef);
        if (State == true)
            ui->FUNC_textEdit->append("键值修改成功");
        else
            ui->FUNC_textEdit->setText("键值修改失败");
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
        if (State == true)
            ui->FUNC_textEdit->append("2383E04 OSD修改成功");
        else
            ui->FUNC_textEdit->setText("2383E04 OSD修改失败");
    }

    State = Find_TargetString_InBinFile(Bin_Buffer, HKC_TEMP_COLOR_DataDef);
    if(State == true)
    {
        bool ok;
        HKC_TEMP_COLOR_DataDef.outputBuffer[1] = ui->_CT9300_RED->text().toInt(&ok, 10);
        HKC_TEMP_COLOR_DataDef.outputBuffer[3] = ui->_CT9300_GREEN->text().toInt(&ok, 10);
        HKC_TEMP_COLOR_DataDef.outputBuffer[5] = ui->_CT9300_BLUE->text().toInt(&ok, 10);
        HKC_TEMP_COLOR_DataDef.outputBuffer[7] =  ui->_CT6500_RED->text().toInt(&ok, 10);
        HKC_TEMP_COLOR_DataDef.outputBuffer[9] =  ui->_CT6500_GREEN->text().toInt(&ok, 10);
        HKC_TEMP_COLOR_DataDef.outputBuffer[11] = ui->_CT6500_BLUE->text().toInt(&ok, 10);
        HKC_TEMP_COLOR_DataDef.outputBuffer[13] = ui->_CTUSER_RED->text().toInt(&ok, 10);
        HKC_TEMP_COLOR_DataDef.outputBuffer[15] = ui->_CTUSER_GREEN->text().toInt(&ok, 10);
        HKC_TEMP_COLOR_DataDef.outputBuffer[17] = ui->_CTUSER_BLUE->text().toInt(&ok, 10);
        State = Write_TargetString_InBinFile(Bin_Buffer, HKC_TEMP_COLOR_DataDef);
        if (State == true)
            ui->FUNC_textEdit->append("色温值修改成功");
        else
            ui->FUNC_textEdit->setText("色温值修改失败");
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

void HK_BIN_Tool::on_Resolution_comboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == "FHD")
    {
        ui->H_lineEdit->setText("1920");
        ui->V_lineEdit->setText("1080");
        if (ui->H_V_checkBox->isChecked())
        {
            ui->HBANK_lineEdit->setText("160");
            //ui->VBANK_lineEdit->setText("41");
        }
    }
    else if (arg1 == "QHD")
    {
        ui->H_lineEdit->setText("2560");
        ui->V_lineEdit->setText("1440");
        if (ui->H_V_checkBox->isChecked())
        {
            ui->HBANK_lineEdit->setText("160");
            //ui->VBANK_lineEdit->setText("41");
        }
    }
    else if (arg1 == "WQHD")
    {
        ui->H_lineEdit->setText("3440");
        ui->V_lineEdit->setText("1440");
        if (ui->H_V_checkBox->isChecked())
        {
            ui->HBANK_lineEdit->setText("160");
            //ui->VBANK_lineEdit->setText("41");
        }
    }
    else if (arg1 == "UHD")
    {
        ui->H_lineEdit->setText("3840");
        ui->V_lineEdit->setText("2160");
        if (ui->H_V_checkBox->isChecked())
        {
            ui->HBANK_lineEdit->setText("160");
            //ui->VBANK_lineEdit->setText("41");
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
    static const int HZ_Array[] = {
        48, 50, 60, 75, 100, 120, 144, 160, 165, 180,
        240, 280, 300, 320, 360, 400, 480, 500, 540, 600
    };

    bool okH, okV, okHB, okFPS;
    int H      = ui->H_lineEdit->text().toInt(&okH, 10);
    float V    = ui->V_lineEdit->text().toInt(&okV, 10);
    int H_BANK = ui->HBANK_lineEdit->text().toInt(&okHB, 10);
    int FPS    = ui->FPS_lineEdit->text().toInt(&okFPS, 10);
    if (!okH || !okV || !okHB || !okFPS) {
        qWarning() << "输入格式错误，请输入有效的整数";
        return;
    }

    // 判断是否用标准 H/VBank
    if (ui->H_V_checkBox->checkState())
    {
        // 原始 VBank 计算后进一位取整
        double rawVBank = (23.0 * FPS * V) / (50000 - 23.0 * FPS);
        int V_BANK = static_cast<int>(std::ceil(rawVBank));
        ui->VBANK_lineEdit->setText(QString::number(V_BANK)); // 显示取整后的 VBank

        double pixels = double(H + H_BANK) * double(V + V_BANK);
        double bandwidthMHz = pixels * FPS / 1e6;
        ui->BANDWIDTH_lineEdit->setText(QString::number(bandwidthMHz, 'f', 2));

        // 遍历所有刷新率，计算并显示对应 VBank 和时钟频率
        for (int var = 0; var < sizeof(HZ_Array)/sizeof(HZ_Array[0]); ++var)
        {
            float refresh = HZ_Array[var];
            rawVBank = (23.0 * refresh * V) / (50000 - 23.0 * refresh);
            int curVBank = static_cast<int>(std::ceil(rawVBank));
            pixels = double(H + H_BANK) * double(V + curVBank);
            double clkMHz = pixels * refresh / 1e6;
            ui->Clock_textEdit->append(
                QString("%1 Hz = %2 MHz    ( VBank = %3 )")
                    .arg(refresh)
                    .arg(QString::number(clkMHz, 'f', 2))
                    .arg(curVBank)
                );
        }
    }
    else
    {
        // 手动输入 VBank
        int V_BANK = ui->VBANK_lineEdit->text().toInt();
        double pixels = double(H + H_BANK) * double(V + V_BANK);
        double bandwidthMHz = pixels * FPS / 1e6;
        ui->BANDWIDTH_lineEdit->setText(QString::number(bandwidthMHz, 'f', 2));

        for (int hz : HZ_Array)
        {
            double clkMHz = pixels * hz / 1e6;
            ui->Clock_textEdit->append(
                QString("%1 Hz = %2 MHz    ( VBank = %3 )")
                    .arg(hz)
                    .arg(QString::number(clkMHz, 'f', 2))
                    .arg(V_BANK)
                );
        }
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



void HK_BIN_Tool::on_Add_Key_H_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择C源文件"), "", tr("C源文件 (*.c)"));
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("错误"), tr("无法打开文件: %1").arg(fileName));
        return;
    }

    QTextStream in(&file);
    QString currentCondition;
    keyConditionData.clear();
    keyConditionOrder.clear();  // 清空顺序记录

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        // 条件编译语句
        if (line.startsWith("#if") || line.startsWith("#elif"))
        {
            QRegularExpression re(R"#((KEY_MODE\s*==\s*\w+))#");
            QRegularExpressionMatch match = re.match(line);
            if (match.hasMatch())
            {
                currentCondition = match.captured(1);
                if (!keyConditionData.contains(currentCondition)) {
                    keyConditionOrder.append(currentCondition); // 记录顺序
                    keyConditionData[currentCondition] = QMap<QString, QString>();
                }
            }
        }
        // 宏定义语句
        else if (line.startsWith("#define") && !currentCondition.isEmpty())
        {
            QStringList parts = line.split(QRegularExpression(R"(\s+)"), Qt::SkipEmptyParts);
            if (parts.size() >= 3)
            {
                QString key = parts[1];
                QString value = parts[2];
                // 如果值以 "0x" 或 "0X" 开头，则去掉前缀
                if (value.startsWith("0x", Qt::CaseInsensitive)) {
                    value = value.mid(2);  // 去掉前两个字符
                }
                keyConditionData[currentCondition][key] = value;
            }
        }
    }
    file.close();

    // 按顺序添加到 comboBox
    ui->Key_H_List_comboBox->clear();
    for (const QString &condition : keyConditionOrder)
    {
        ui->Key_H_List_comboBox->addItem(condition);
    }
}


void HK_BIN_Tool::on_Key_H_List_comboBox_currentTextChanged(const QString &condition)
{
    if (!keyConditionData.contains(condition)) return;

    const auto &dataMap = keyConditionData[condition];

    // 设置按键值
    ui->POWER_KEY->setText(dataMap.value("POWER_KEY", ""));
    ui->MENU_KEY->setText(dataMap.value("MENU_KEY", ""));
    ui->EXIT_KEY->setText(dataMap.value("EXIT_KEY", ""));
    ui->RIGHT_KEY->setText(dataMap.value("RIGHT_KEY", ""));
    ui->LEFT_KEY->setText(dataMap.value("LEFT_KEY", ""));

    // 设置寄存器值
    ui->POWER_REG->setText(dataMap.value("POWER_REG", ""));
    ui->MENU_REG->setText(dataMap.value("MENU_REG", ""));
    ui->EXIT_REG->setText(dataMap.value("EXIT_REG", ""));
    ui->RIGHT_REG->setText(dataMap.value("RIGHT_REG", ""));
    ui->LEFT_REG->setText(dataMap.value("LEFT_REG", ""));
}

void HK_BIN_Tool::on_Key_Convert_pushButton_clicked()
{
    // 定义按键标识与对应控件映射（大小写都处理）
    QMap<QString, QString> keyNameMap = {
        {"P", "POWER"},
        {"M", "MENU"},
        {"E", "EXIT"},
        {"R", "RIGHT"},
        {"L", "LEFT"}
    };

    // 获取 Key_1 到 Key_5
    QStringList sourceOrder;
    for (int i = 1; i <= 5; ++i)
    {
        QString value = findChild<QLineEdit*>("Key_" + QString::number(i) + "_lineEdit")->text().trimmed().toUpper();
        if (keyNameMap.contains(value))
            sourceOrder << keyNameMap[value];
        else
        {
            QMessageBox::warning(this, "错误", QString("第 %1 个原始按键输入无效（%2）").arg(i).arg(value));
            return;
        }
    }

    // 获取 Key_6 到 Key_10
    QStringList targetOrder;
    for (int i = 6; i <= 10; ++i)
    {
        QString value = findChild<QLineEdit*>("Key_" + QString::number(i) + "_lineEdit")->text().trimmed().toUpper();
        if (keyNameMap.contains(value))
            targetOrder << keyNameMap[value];
        else
        {
            QMessageBox::warning(this, "错误", QString("第 %1 个目标按键输入无效（%2）").arg(i).arg(value));
            return;
        }
    }

    // 读取原始 KEY 值和 REG 值
    QMap<QString, QString> keyValues, regValues;
    for (const QString& name : keyNameMap.values())
    {
        QLineEdit* keyEdit = findChild<QLineEdit*>(name + "_KEY");
        QLineEdit* regEdit = findChild<QLineEdit*>(name + "_REG");
        if (keyEdit && regEdit)
        {
            keyValues[name] = keyEdit->text();
            regValues[name] = regEdit->text();
        }
    }

    // 按照新顺序设置对应 KEY 和 REG 值
    for (int i = 0; i < 5; ++i)
    {
        QString from = sourceOrder[i];  // 原始功能
        QString to = targetOrder[i];    // 目标功能

        QLineEdit* keyEdit = findChild<QLineEdit*>(to + "_KEY");
        QLineEdit* regEdit = findChild<QLineEdit*>(to + "_REG");
        if (keyEdit && regEdit)
        {
            keyEdit->setText(keyValues[from]);
            regEdit->setText(regValues[from]);
        }
    }
}

void HK_BIN_Tool::on_Add_LOGO_Define_pushButton_clicked()
{
    Logo_Base_Default.clear();
    Logo_Base_Default_valueList.clear();
    QString filePath = QFileDialog::getOpenFileName(this, "选择头文件", "", "Header Files (*.h);;All Files (*)");
    if (filePath.isEmpty())
    {
        qDebug() << "用户取消选择";
        return;
    }

    QStringList activeDefines;
    QString osdFont = ui->_OSD_EXTEND_256_2BIT_FONTS_lineEdit->text().trimmed();
    QString osdGen  = ui->_OSD_REG_MAPPING_GEN_lineEdit->text().trimmed();
    if (!osdFont.isEmpty())
        activeDefines << "_OSD_EXTEND_256_2BIT_FONTS=" + osdFont;

    if (!osdGen.isEmpty())
        activeDefines << "_OSD_REG_MAPPING_GEN=" + osdGen;
    // 要提取的宏列表（由你自定义）
    QStringList targetMacros = {
        "_OSD_LOGO_2BIT_OFFSET",
        "_OSD_LOGO_4BIT_OFFSET",
        "_OSD_LOGOA_WIDTH",
        "_OSD_LOGOA_HEIGHT",
        "_OSD_LOGOB_WIDTH",
        "_OSD_LOGOB_HEIGHT",
        "_OSD_LOGOA_ADDRESS_ROWCOMMAND",
        "_OSD_LOGOA_ADDRESS_CHARCOMMAND",
        "_OSD_LOGO_FONT_BASE_ADDRESS",
    };

    // 调用解析函数
    Logo_Base_Default = parseHeaderMacros(filePath, activeDefines, targetMacros);

    QString result;
    for (const QString &key : targetMacros)
    {
        QString value = Logo_Base_Default.value(key, "[未找到]");
        QString processedValue = value;

        bool ok = false;
        int numericValue = value.startsWith("0x", Qt::CaseInsensitive)
                               ? value.toInt(&ok, 16)
                               : value.toInt(&ok, 10);

        if (ok)
        {
            if (key.contains("WIDTH"))
            {
                numericValue /= 12;
            }
            else if (key.contains("HEIGHT"))
            {
                numericValue /= 18;
            }
            processedValue = QString::number(numericValue);

            // 只添加合法数值
            Logo_Base_Default_valueList.append(processedValue);
        }
        else
        {
            processedValue = "[非法数值]";
        }

        result += key + " = " + processedValue + "\n";
    }

    ui->LOGO_MESSAGE_textEdit->append(result);
}

void HK_BIN_Tool::on_Add_LOGO_Palette_pushButton_clicked()
{
    Logo_paletteData.clear();
    QString filePath = QFileDialog::getOpenFileName(this, "选择 Logo 调色板头文件", "", "Header Files (*.h);;All Files (*)");
    if (filePath.isEmpty())
        return;

    QStringList activeDefines;
    QString osdFont = ui->_OSD_EXTEND_256_2BIT_FONTS_lineEdit->text().trimmed();

    if (!osdFont.isEmpty())
        activeDefines << "_OSD_EXTEND_256_2BIT_FONTS=" + osdFont;

    Logo_paletteData = extractByteArray(filePath, activeDefines, "tPALETTE_LOGO");

    qDebug() << "调色板字节总数:" << Logo_paletteData.size();

    // 构造显示内容
    QString result = QString("调色板字节总数: %1\n\n").arg(Logo_paletteData.size());
    for (int i = 0; i < Logo_paletteData.size(); ++i)
    {
        result += QString("0x%1 ").arg(Logo_paletteData[i], 2, 16, QLatin1Char('0')).toUpper();
        if ((i + 1) % 3 == 0) result += "\n"; // 每3字节换行
    }

    ui->LOGO_MESSAGE_textEdit->append(result);
}


void HK_BIN_Tool::on_Add_LOGO_Draw_pushButton_clicked()
{
    Logo_Data.clear();
    QString filePath = QFileDialog::getOpenFileName(this, "选择 Logo Draw头文件", "", "Header Files (*.h);;All Files (*)");
    if (filePath.isEmpty())
    {
        qDebug() << "用户取消选择";
        return;
    }

    // 设置条件宏定义
    QStringList activeDefines;
    QString osdFont = ui->_OSD_EXTEND_256_2BIT_FONTS_lineEdit->text().trimmed();
    QString osdGen  = ui->_OSD_REG_MAPPING_GEN_lineEdit->text(). trimmed();

    if (!osdFont.isEmpty())
        activeDefines << "_OSD_EXTEND_256_2BIT_FONTS=" + osdFont;

    if (!osdGen.isEmpty())
        activeDefines << "_OSD_REG_MAPPING_GEN=" + osdGen;

    Logo_Data = extractDrawLogoArray(filePath, activeDefines);

    qDebug() << "提取到的 LOGO 数据字节数:" << Logo_Data.size();
    QString result = QString("提取到的 LOGO 数据字节数: %1\n\n").arg(Logo_Data.size());
    ui->LOGO_MESSAGE_textEdit->append(result);

}

void HK_BIN_Tool::on_Replace_LOGO_pushButton_clicked()
{
    bool State = false;
    if(ui->RTD2383_Project_checkBox->isChecked())
    {
        State = Find_TargetString_InBinFile(Bin_Buffer, LOGO_2383_BASE_Default_DataDef);
        if(State == true)
        {
            QVector<uchar> ucharList;  // 用于存储转换后的 uchar 数据
            for (int i = 0; i < Logo_Base_Default_valueList.size(); ++i) {
                const QString& str = Logo_Base_Default_valueList[i];
                bool ok;
                int value = str.toInt(&ok, 10); // 按十进制将字符串转换为整数
                if (ok) {
                    if (i == 6 || i == 7 || i == 8) {
                        // 第6,第7和第8项为word数据，按大端模式转换为两个字节
                        if (value <= 65535) {
                            uchar high = static_cast<uchar>((value >> 8) & 0xFF);  // 取高字节
                            uchar low  = static_cast<uchar>(value & 0xFF);         // 取低字节
                            ucharList.append(high);  // 先添加高字节
                            ucharList.append(low);   // 再添加低字节
                        } else {
                            qWarning() << "word数值超出范围：" << value;
                        }
                    } else {
                        // 其他项按byte处理
                        if (value <= 255) {
                            ucharList.append(static_cast<uchar>(value));
                        } else {
                            qWarning() << "byte数值超出范围：" << value;
                        }
                    }

                } else {
                    qWarning() << "转换失败:" << str;
                }
                for (int var = 0; var < LOGO_2383_BASE_Default_DataDef.outputBufferSize; ++var) {
                    LOGO_2383_BASE_Default_DataDef.outputBuffer[var] = ucharList[var];
                }
                for (int var = 0; var < LOGO_2383_BASE_Default1_DataDef.outputBufferSize; ++var) {
                    LOGO_2383_BASE_Default1_DataDef.outputBuffer[var] = ucharList[var + 6];
                }
            }
            State = Write_TargetString_InBinFile(Bin_Buffer, LOGO_2383_BASE_Default_DataDef);
            State = Write_TargetString_InBinFile(Bin_Buffer, LOGO_2383_BASE_Default1_DataDef);
            if(State == true)
                ui->LOGO_MESSAGE_textEdit->append("LOGO_2383_BASE_Default_DataDef Successful!");
            else
                ui->LOGO_MESSAGE_textEdit->append("LOGO_2383_BASE_Default_DataDef false!");
        }
        else
            ui->LOGO_MESSAGE_textEdit->append("Find LOGO_2383_BASE_Default_DataDef false!");
    }
    else
    {
        State = Find_TargetString_InBinFile(Bin_Buffer, LOGO_BASE_Default_DataDef);
        if(State == true)
        {
            QVector<uchar> ucharList;  // 用于存储转换后的 uchar 数据
            for (int i = 0; i < Logo_Base_Default_valueList.size(); ++i) {
                const QString& str = Logo_Base_Default_valueList[i];
                bool ok;
                int value = str.toInt(&ok, 10); // 按十进制将字符串转换为整数
                if (ok) {
                    if (i == 7 || i == 8) {
                        // 第7和第8项为word数据，按大端模式转换为两个字节
                        if (value <= 65535) {
                            uchar high = static_cast<uchar>((value >> 8) & 0xFF);  // 取高字节
                            uchar low  = static_cast<uchar>(value & 0xFF);         // 取低字节
                            ucharList.append(high);  // 先添加高字节
                            ucharList.append(low);   // 再添加低字节
                        } else {
                            qWarning() << "word数值超出范围：" << value;
                        }
                    } else {
                        // 其他项按byte处理
                        if (value <= 255) {
                            ucharList.append(static_cast<uchar>(value));
                        } else {
                            qWarning() << "byte数值超出范围：" << value;
                        }
                    }

                } else {
                    qWarning() << "转换失败:" << str;
                }
            }

            for (int var = 0; var < ucharList.size(); ++var) {
                qDebug() << ucharList[var];
            }

            memcpy(LOGO_BASE_Default_DataDef.outputBuffer, ucharList.data(), ucharList.size());
            State = Write_TargetString_InBinFile(Bin_Buffer, LOGO_BASE_Default_DataDef);
            if(State == true)
                ui->LOGO_MESSAGE_textEdit->append("LOGO_BASE_Default_DataDef Successful!");
            else
                ui->LOGO_MESSAGE_textEdit->append("LOGO_BASE_Default_DataDef false!");
        }
        else
            ui->LOGO_MESSAGE_textEdit->append("Find LOGO_BASE_Default_DataDef false!");
    }



    State = Find_TargetString_InBinFile(Bin_Buffer, LOGO_Palette_DataDef);
    if(State == true)
    {
        memcpy(LOGO_Palette_DataDef.outputBuffer, Logo_paletteData.data(), Logo_paletteData.size());
        State = Write_TargetString_InBinFile(Bin_Buffer, LOGO_Palette_DataDef);
        if(State == true)
            ui->LOGO_MESSAGE_textEdit->append("LOGO_Palette_DataDef Successful!");
        else
            ui->LOGO_MESSAGE_textEdit->append("LOGO_Palette_DataDef false!");
    }
    else
        ui->LOGO_MESSAGE_textEdit->append("Find LOGO_Palette_DataDef false!");

    State = Find_TargetString_InBinFile(Bin_Buffer, LOGO_DataDef);
    if(State == true)
    {
        memcpy(LOGO_DataDef.outputBuffer, Logo_Data.data(), Logo_Data.size());
        State = Write_TargetString_InBinFile(Bin_Buffer, LOGO_DataDef);
        ui->LOGO_MESSAGE_textEdit->append("LOGO_DataDef Successful!");

    }


}
