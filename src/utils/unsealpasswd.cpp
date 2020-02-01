#include "include/utils/unsealpasswd.h"
#include "include/interface/ui_unsealpasswd.h"

UnsealPasswd::UnsealPasswd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UnsealPasswd)
{
    ui->setupUi(this);
}

UnsealPasswd::~UnsealPasswd()
{
    delete ui;
}
