#ifndef USERMANUALDIALOG_H
#define USERMANUALDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class UserManualDialog;
}

class UserManualDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserManualDialog(QWidget *parent = nullptr);
    ~UserManualDialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::UserManualDialog *ui;
};

#endif // USERMANUALDIALOG_H
