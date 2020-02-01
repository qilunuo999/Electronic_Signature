#ifndef CHAINCODEINFODIALOG_H
#define CHAINCODEINFODIALOG_H

#include <QDialog>
#include "include/network/network.h"
#include <QList>

namespace Ui {
class ChaincodeInfoDialog;
}

class ChaincodeInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChaincodeInfoDialog(QWidget *parent = nullptr);
    void display(QJsonObject json);
    ~ChaincodeInfoDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ChaincodeInfoDialog *ui;
};

#endif // CHAINCODEINFODIALOG_H
