#include "include/interface/softinfodialog.h"
#include "ui_softinfodialog.h"

SoftInfoDialog::SoftInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoftInfoDialog)
{
    ui->setupUi(this);
}

SoftInfoDialog::~SoftInfoDialog()
{
    delete ui;
}

void SoftInfoDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    this->close();
}
