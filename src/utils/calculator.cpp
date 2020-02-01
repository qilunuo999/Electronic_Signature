#include "include/utils/calculator.h"

Calculator::Calculator()
{

}

Calculator::~Calculator()
{

}

QByteArray Calculator::sha512(const std::string str)
{
    char buf[5];
     unsigned char digest[SHA512_DIGEST_LENGTH];
     SHA512_CTX ctx;
     SHA512_Init(&ctx);
     SHA512_Update(&ctx, str.c_str(), str.size());
     SHA512_Final(digest, &ctx);

     QByteArray mdString = "";
     for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
     {
         sprintf(buf,"%02x",digest[i]);
         mdString += buf;
     }
     return mdString;
}

QString Calculator::countFileHash(QString fp)
{
    QFile* file = new QFile(fp);
    file->open(QIODevice::ReadOnly);
    QCryptographicHash hash(QCryptographicHash::Sha512);
    if(!file->atEnd())
    {
        hash.addData(file->readAll());
    }
    QString sha512;
    sha512.append(hash.result().toHex());
    file->close();
    return sha512;
}

int Calculator::offset(int oldOffset,int stringlen)  //计算偏移量，需要传入旧的>偏移量和字符串长度
{
        return (oldOffset+stringlen);
}

int Calculator::getCharNumber(QVector<QByteArray>* words)
{
    int charNumber = 0;
    for(int i = 0; i < words->size(); i++)
    {
        charNumber += words->at(i).size();
    }
    return charNumber;
}
