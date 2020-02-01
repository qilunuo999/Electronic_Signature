#ifndef PAGEJUMP_H
#define PAGEJUMP_H

#include <QDialog>
#include <QScrollArea>
#include <QMessageBox>
#include "include/controller/businessmodel.h"
//页面跳转窗口
namespace Ui
{
class PageJump;
}

class PageJump : public QDialog
{
    Q_OBJECT
public:
    explicit PageJump(QVector<QImage>** imageList, QScrollArea* tempStorage,QLabel * action,int numPages[], QWidget *parent = nullptr, QString fp = nullptr);
    ~PageJump();

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    QString fp;
    Ui::PageJump* ui;
    QString pageNum;
    QVector<QImage>** imageList;
    QScrollArea* tempStorage;
    QLabel * action;
    int* numPages;
};

#endif // PAGEJUMP_H
