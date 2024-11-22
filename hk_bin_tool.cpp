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

#include <QFileDialog>
#include <QMessageBox>

QByteArray Bin_Buffer;
QString filepath;

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
}

/**
 * @description: 保存bin文件按键
 * @return {*}
 */
void HK_BIN_Tool::on_Save_Bin_pushButton_clicked()
{
    QString originalPath = filepath;
    // 提取文件名和路径
    QFileInfo fileInfo(originalPath);
    QString dirPath = fileInfo.absolutePath();      // 获取文件所在目录
    QString baseName = fileInfo.completeBaseName(); // 文件名（不含扩展名）
    QString extension = fileInfo.suffix();          // 文件扩展名

    // 生成新文件名
    QString newFileName = QString("%1/NEW_%2.%3").arg(dirPath).arg(baseName).arg(extension);

    // 打开文件并写入数据
    QFile file(newFileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        // 如果打开失败，可以记录日志或者执行其他错误处理操作
        qDebug() << "Failed to open file for writing:" << newFileName;
        return;
    }

    // 写入数据并关闭文件
    file.write(Bin_Buffer);
    file.close();

    // 保存成功提示
    QMessageBox::information(this, tr("成功"), tr("文件已保存到:\n%1").arg(newFileName));
}
