#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QDesktopServices>
#include <QMessageBox>

#include "include/network/network.h"


namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

signals:
    void loginSuccess(QJsonObject);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void success(QJsonObject);

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
