#include "include/interface/search.h"
#include "ui_search.h"

Search::Search(QWidget *parent ):
    QDialog (parent),
    ui(new Ui::Search)
{
  ui->setupUi(this);
}

Search::~Search()
{
    delete ui;
}

void Search::on_findButton_clicked()
{

}

void Search::on_cancelButton_2_clicked()
{

}
