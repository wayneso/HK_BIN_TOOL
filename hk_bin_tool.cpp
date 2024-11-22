#include "hk_bin_tool.h"
#include "./ui_hk_bin_tool.h"

HK_BIN_Tool::HK_BIN_Tool(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HK_BIN_Tool)
{
    ui->setupUi(this);
}

HK_BIN_Tool::~HK_BIN_Tool()
{
    delete ui;
}
