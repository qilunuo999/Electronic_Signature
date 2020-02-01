#include "include/utils/pdfwriter.h"

PdfWriter::PdfWriter()
{

}

PdfWriter::~PdfWriter()
{

}

bool PdfWriter::writeSeal(QString pdfFileName,QVector<QByteArray>* seal)
{
    QFile pdfFile(pdfFileName);

    if(!pdfFile.open(QIODevice::WriteOnly|QIODevice::Append))
    {
         qDebug()   << "Cannot open file for Writing 19";
         return false;
    }

    for(int i = 0; i < seal->size(); i++)
    {
        pdfFile.write(seal->at(i));
    }

    pdfFile.close();
    return true;
}

bool PdfWriter::writeUnseal(QString pdfFileName,QVector<QVector<QByteArray>>* unsealInfo)
{
    QFile pdfFile(pdfFileName);

    if(!pdfFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"open File witch is used to read is failure!";
        return false;
    }
    QByteArray temp;
    QVector<QByteArray> temp2;
    while(!pdfFile.atEnd())
    {
        temp = pdfFile.readLine();
        if(temp.left(5) == "%seal")
        {
            break;
        }
        temp2.append(temp);
    }
    pdfFile.close();

    if(!pdfFile.open(QIODevice::WriteOnly))
    {
         qDebug()   << "Cannot open file for Writing 19";
         return false;
    }

    for(int i = 0; i < temp2.size(); i++)
    {
        pdfFile.write(temp2.at(i));
    }
    for(int i = 0; i < unsealInfo->size(); i++)
    {
        for(int j = 0; j < unsealInfo->at(i).size(); j++)
        {
            pdfFile.write(unsealInfo->at(i).at(j));
        }
    }

    pdfFile.close();
    return true;
}
