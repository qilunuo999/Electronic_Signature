#ifndef DISPLAY_INFO_H
#define DISPLAY_INFO_H

#include <QDialog>
#include <string>
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "QVector"
#include "QByteArray"
#include <QListWidget>
#include "QString"
#include "time.h"
#include "cstdio"
#include "QDateTime"

namespace Ui {
class display_info;
}

class display_info : public QDialog
{
    Q_OBJECT

public:
    explicit display_info(QWidget *parent = nullptr,QVector<QVector<QByteArray>>*info = nullptr);
    void Cer_verifi(const char *bytes);
    ~display_info();

private slots:
    void on_pushButton_2_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::display_info *ui;
    QVector<QVector<QByteArray>>*info;
    //解析ASN1_TIME转为time_t
    time_t ASN1_TIMEGet(ASN1_TIME* a, int* err);
    int mypint(unsigned const char** s, int n, int min, int max, int* e);
};

#endif // DISPLAY_INFO_H
