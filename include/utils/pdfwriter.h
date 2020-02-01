#ifndef PDFWRITER_H
#define PDFWRITER_H

#include <QString>
#include <QVector>
#include <QFile>
#include <QDebug>


class PdfWriter
{
public:
    PdfWriter();
    ~PdfWriter();

    bool writeSeal(QString pdfFileName,QVector<QByteArray>* seal);
    bool writeUnseal(QString pdfFileName,QVector<QVector<QByteArray>>* unsealInfo);
};

#endif // PDFWRITER_H
