#ifndef RSAUTILS_H
#define RSAUTILS_H

#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <QByteArray>
#include <QString>
#include <QFile>
#include <QTextStream>

class RSAUtils
{
public:
    RSAUtils();
    QByteArray load_pri_key(QString keyFilePath);
    QByteArray rsa_pri_encrypt(std::string clearText, QByteArray priKey);
    QByteArray rsa_pub_decrypt(QByteArray cipherText,QByteArray pubKey);
};

#endif // RSAUTILS_H
