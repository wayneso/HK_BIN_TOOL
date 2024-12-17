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


QVector<EDID> Bin_EDID;
QVector<EDID_Info> Bin_EDID_Info;
QByteArray Bin_Buffer;
QString filepath;
int EDID_index = 1;

HK_BIN_Tool::HK_BIN_Tool(QWidget *parent) : QWidget(parent), ui(new Ui::HK_BIN_Tool)
{
    ui->setupUi(this);
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
    ui->Measure_lineEdit->setText(QString::number(edidInfo.diagonalSizeInches));
    ui->InputName_lineEdit->setText(edidInfo.monitorName);

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
    ui->Measure_lineEdit->setText(QString::number(edidInfo.diagonalSizeInches));
    ui->InputName_lineEdit->setText(edidInfo.monitorName);

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

    EDID_Info edidInfo;


    // 修改每个 EDID（例如修改厂商信息）
    for (EDID& edid : Bin_EDID) {
        edid.buffer = Modify_EDID(edid.buffer, edidInfo);
    }


    // 将修改后的 EDID 写回 binBuffer
    Write_EDID(Bin_Buffer, Bin_EDID);

    // 写入数据并关闭文件
    file.write(Bin_Buffer);
    file.close();

    // 保存成功提示
    QMessageBox::information(this, tr("成功"), tr("文件已保存到:\n%1").arg(newFileName));
}


