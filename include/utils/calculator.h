#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <iostream>
#include <string.h>
#include <openssl/sha.h>
#include <QString>
#include <QFile>
#include <QCryptographicHash>
#include <QVector>
#include <QByteArray>

class Calculator
{
public:
    Calculator();
    ~Calculator();
    QByteArray sha512(const std::string str);
    QString countFileHash(QString fp);
    int offset(int oldoffset,int stringlen);
    int getCharNumber(QVector<QByteArray>* words);
};

#endif // CALCULATOR_H
