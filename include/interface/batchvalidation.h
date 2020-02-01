#ifndef BATCHVALIDATION_H
#define BATCHVALIDATION_H

#include <QDialog>
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "QVector"
#include "QByteArray"
#include <QListWidget>
#include <QMessageBox>
#include <QJsonObject>
#include "QString"
#include "display_info.h"
#include "ui_display_info.h"
#include "include/utils/calculator.h"
#include "include/network/network.h"
#include "include/interface/chaincodeinfodialog.h"

//验证窗口
namespace Ui
{
class BatchValidation;
}

class BatchValidation : public QDialog
{
    Q_OBJECT
public:
    explicit BatchValidation(QWidget *parent = nullptr, QVector<QVector<QByteArray>>*info = nullptr,QByteArray *digest = nullptr, QString pdfFile=nullptr);
    ~BatchValidation();

private slots:

    void on_detailsButton_clicked();

    void on_lookCertificateButton_clicked();

    void on_lookTimestampButton_clicked();

    void on_closeButton_4_clicked();


    std::string rsa_pub_decrypt(std::string cipherText,std::string pubKey);

    std::string Decode(const char* Data,int DataByte,int& OutByte);


    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushButton_clicked();

    void getServerInfo(QJsonObject);//获取服务器端的文件盖章信息

private:
    QString pdfFile;
    Ui::BatchValidation* ui;
    QVector<QVector<QByteArray>>*info;
    QByteArray publicKey;
    QByteArray *digest;
    QJsonObject json;
};

#endif // BATCHVALIDATION_H
