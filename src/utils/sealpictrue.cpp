#include "include/utils/sealpictrue.h"

SealPictrue::SealPictrue()
{

}

SealPictrue::~SealPictrue()
{

}

bool SealPictrue::openSealPictrue(QString sealName)
{
   QFile imageFile(sealName);
   QByteArray temp = "";

   if(!imageFile.open(QIODevice::ReadOnly))
   {
       qDebug()   << "Cannot open file for Writing";
       return false;
   }
    int i = 1;
    while(temp != "__Delimiter__\n")
    {
        imageStream += temp;
        temp = imageFile.readLine();
        qDebug()<<i++;
    }
    temp = imageFile.readLine();
    timeStamp = temp;
    startTime = imageFile.readLine();
    endTime = imageFile.readLine();
    temp = imageFile.readLine();
    while((temp = imageFile.readLine()) != "__Delimiter__\n")
    {
        pubKey += temp;
    }
    pubKey.insert(pubKey.size()-1,'@');
    passwdHash = imageFile.readLine();

    temp = imageFile.readLine();
    temp = imageFile.readLine();
    while(temp != "__Delimiter__\n")
    {
        basicInfo += temp;
        temp = imageFile.readLine();
    }

    temp = imageFile.readLine();
    while (temp != "__Delimiter__\n")
    {
        certification.append(temp);
        temp = imageFile.readLine();
    }
    certification.insert(0,'@');
    sealUuid = imageFile.readLine();
    imageFile.close();
    QPixmap* pixmap = new QPixmap(sealName);
    this->width = pixmap->width();
    this->height = pixmap->height();
    return true;
}

QByteArray SealPictrue::getImageStream()
{
    return imageStream;
}

QByteArray SealPictrue::getTimestamp()
{
    return timeStamp;
}

QByteArray SealPictrue::getStartTime()
{
    return startTime;
}

QByteArray SealPictrue::getEndTime()
{
    return endTime;
}

QByteArray SealPictrue::getPubKey()
{
    return pubKey;
}

QByteArray SealPictrue::getPasswdHash()
{
    return passwdHash;
}

QByteArray SealPictrue::getBasicInfo()
{
    return basicInfo;
}

QByteArray  SealPictrue::getCertification()
{
    return certification;
}

QByteArray SealPictrue::getSealUuid()
{
    return sealUuid;
}

int SealPictrue::getWidth()
{
    return this->width;
}

int SealPictrue::getHeight()
{
    return this->height;
}
