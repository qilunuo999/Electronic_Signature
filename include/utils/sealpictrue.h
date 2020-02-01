#ifndef SEALPICTRUE_H
#define SEALPICTRUE_H

#include <QByteArray>
#include <QFile>
#include <QDebug>
#include <QPixmap>

class SealPictrue
{
public:
    SealPictrue();
    ~SealPictrue();

    bool openSealPictrue(QString sealName);
    QByteArray getImageStream();
    QByteArray getTimestamp();
    QByteArray getStartTime();
    QByteArray getEndTime();
    QByteArray getPubKey();
    QByteArray getPasswdHash();
    QByteArray getBasicInfo();
    QByteArray getCertification();
    QByteArray getSealUuid();
    int getWidth();
    int getHeight();
private:
    QByteArray imageStream;//图片流
    QByteArray timeStamp;//时间戳
    QByteArray startTime;//起效时间
    QByteArray endTime;//有效期截止时间
    QByteArray pubKey;//公钥
    QByteArray passwdHash;//密码哈希
    QByteArray basicInfo;//基础信息
    QByteArray certification;//印章证书
    QByteArray sealUuid;//印章ID
    int width;
    int height;
};

#endif // SEALPICTRUE_H
