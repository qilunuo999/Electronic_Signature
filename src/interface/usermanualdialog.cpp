#include "include/interface/usermanualdialog.h"
#include "ui_usermanualdialog.h"

UserManualDialog::UserManualDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserManualDialog)
{
    ui->setupUi(this);
}

UserManualDialog::~UserManualDialog()
{
    delete ui;
}

void UserManualDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    this->close();
}
