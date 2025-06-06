#ifndef HK_BIN_TOOL_H
#define HK_BIN_TOOL_H

#include <QWidget>
#include "BIN_Data_Func.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class HK_BIN_Tool;
}
QT_END_NAMESPACE

class HK_BIN_Tool : public QWidget
{
    Q_OBJECT

public:
    HK_BIN_Tool(QWidget *parent = nullptr);
    ~HK_BIN_Tool();

private slots:
    void on_Add_Bin_pushButton_clicked();

    void on_Save_Bin_pushButton_clicked();

    void on_Next_EDID_Button_clicked();

    void on_Measure_comboBox_currentTextChanged(const QString &arg1);

    void on_Measure_lineEdit_textChanged(const QString &arg1);


    void on_DATA_checkBox_clicked(bool checked);

    void on_CODE_INC_checkBox_clicked(bool checked);



    void on_Add_Edid_checkBox_clicked(bool checked);

    void on_Add_Edid_pushButton_clicked();

    void on_Save_Edid_pushButton_clicked();

    void on_checkBox_clicked(bool checked);

    void on_Resolution_comboBox_currentTextChanged(const QString &arg1);

    void on_calculate_pushButton_clicked();

    void on_MA_pushButton_clicked();

    void on_LOGO_SIZE_COL_ROW_pushButton_clicked();

    void on_Add_LOGO_Data_pushButton_clicked();

    void on_Replace_LOGO_pushButton_clicked();

    void on_Add_Key_H_pushButton_clicked();

    void on_Key_H_List_comboBox_currentTextChanged(const QString &arg1);

    void on_Key_Convert_pushButton_clicked();

private:
    Ui::HK_BIN_Tool *ui;
};
#endif // HK_BIN_TOOL_H

