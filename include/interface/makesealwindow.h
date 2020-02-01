#ifndef MAKESEALWINDOW_H
#define MAKESEALWINDOW_H

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QUuid>

#include <openssl/x509v3.h>
#include <openssl/bn.h>
#include <openssl/asn1.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <openssl/pem.h>
#include <openssl/bio.h>

#include <fstream>
#include <cassert>
#include <string>
#include <vector>

#include "include/network/network.h"

namespace Ui {
class MakeSealWindow;
}

class MakeSealWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MakeSealWindow(QWidget *parent = nullptr,QString user = nullptr);
    ~MakeSealWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_getCertificateButton_clicked();

    void on_pushButton_3_clicked();

    void on_keyButton_clicked();

    void getVerifyInfoByNetwork(QJsonObject);

    void getResultInfo(QJsonObject);

private:
    Ui::MakeSealWindow *ui;
    bool isError = false;
    QByteArray cerInfo;//证书二进制数据
    QByteArray cerSerialNumber;//证书序列号
    QString certificateName;//印章名
    QByteArray sealinfo;//印章认证信息
    QByteArray sealName;//印章名
    QByteArray sealId;//印章id
    QByteArray keyInfo;//提取得到的公钥
    QByteArray prikeyInfo;//私钥信息
    QString user;//印章制作人
    QString file_fullpath,//图片路径
            file_fullpath2,
            file_name,//图片名
            file_suffix;//图片后缀
    QFileInfo  info; //详细路径信息
    QString certificate;//证书
    QString certificateKey;//证书秘钥
    QString sealDialog;//印章生成目录

    void extractPubKey();
    void extractPriKey();
    void extractCertificate();
    void verifyCertificate();
    void upload();//上传印章信息
    std::string rsa_pri_encrypt(const std::string &clearText);// 私钥加密
    std::string rsa_pub_decrypt(const std::string &cipherText, const std::string &pubKey);//公钥解密
    std::string sha512(const std::string str);//生成用户密码hash值
    std::string Encode(const unsigned char* Data,int DataByte);//base64加密
};


#endif // MAINWINDOW_H
