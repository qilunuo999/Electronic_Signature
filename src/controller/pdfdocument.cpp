#include "include/controller/pdfdocument.h"

PdfDocument::PdfDocument()
{

}

PdfDocument::PdfDocument(QString fp, bool isOpen)
{
    this->fp = fp;
    if(isOpen)
    {
        this->sealUpdate = new QVector<QVector<QByteArray>> ();
        this->certification = new QVector<QVector<QByteArray>> ();
        pos = new qint64;
    }
}

PdfDocument::~PdfDocument()
{
}

int PdfDocument::openPDF(QVector<QImage>** imageList, QScrollArea* tempStorage,  QListWidget* lWidget, double pictureRate[])
{
    this->imageList = imageList;
    pdfUtils = new PdfUtils(nullptr, lWidget);

    if(pdfUtils == nullptr)
    {
        return 0;
    }
    pdfUtils->countInfoAbstract(fp);
    pdfUtils->loadSealInfo(this->fp);
    *pos = pdfUtils->getCursor();
    sealUpdate = pdfUtils->getSealInfo();
    certification = pdfUtils->getCertification();
    infoAbstract = pdfUtils->getInfoAbstract();
    pdfUtils->getPdfDocument(this->fp);
    numPages = pdfUtils->getNumPages();
    (*this->imageList)->resize(numPages);
    pageSize = pdfUtils->getPageSize();
    pdfUtils->getPdfImage(0, this->imageList);
    pictureRate[0] = countSizeRate();
    connect(pdfUtils,&QThread::finished,pdfUtils,&QObject::deleteLater);
    pdfUtils->start();
    PdfCanvas* pdfCanvas = new PdfCanvas();
    pdfCanvas->drawCanvas(imageList,tempStorage,0);
    delete pdfCanvas;
    return numPages;
}
void PdfDocument::setCertification(QVector<QVector<QByteArray>>* certification)
{
    this->certification = certification;
}
void PdfDocument::setSealInfo(QVector<QVector<QByteArray>>* seal)
{
    sealUpdate = seal;
}

qint64 PdfDocument::getPos()
{
    return *pos;
}

QByteArray PdfDocument::getInfoAbstract()
{
    return infoAbstract;
}

QVector<QVector<QByteArray>>* PdfDocument::getSealInfo()
{
    return sealUpdate;
}

 QVector<QVector<QByteArray>>* PdfDocument::getCertification()
 {
     return certification;
 }

 qint64 PdfDocument::getFileSize()
 {
     QFile pdfFile(fp);
     if(!pdfFile.open(QIODevice::ReadOnly))
     {
          qDebug()   << "open file is failure!";
          return 0;
     }
     return pdfFile.size();
 }

void PdfDocument::savePDF(QString fp, QString saveFp)
{
    PdfUtils* pdfUtils = new PdfUtils();
    pdfUtils->openPdf(fp);
    pdfUtils->saveOutput(saveFp);
    delete  pdfUtils;
}

void PdfDocument::jumpPage(int pageNum, QVector<QImage>** imageList, QScrollArea* tempStorage)
{
    if((*imageList)->at(pageNum).isNull())
    {
        PdfUtils* utils = new PdfUtils();
        utils->getPdfDocument(this->fp);
        utils->getPdfImage(pageNum, imageList);
        delete utils;
    }
    PdfCanvas* pdfCanvas = new PdfCanvas();
    pdfCanvas->drawCanvas(imageList, tempStorage, pageNum);
    delete pdfCanvas;
}

 void PdfDocument::zoomPDF(QVector<QImage>** imageList, QScrollArea* tempStorage, int pageNum, double multipleRate)
 {
    PdfCanvas* pdfCanvas = new PdfCanvas();
    pdfCanvas->zoomCanvas(imageList, tempStorage, pageNum, multipleRate);
    delete  pdfCanvas;
 }

 double PdfDocument::countSizeRate()
 {
     return (static_cast<double>((*this->imageList)->at(0).height()) / this->pageSize.height());
 }

void PdfDocument::seal(QVector<QImage>** imageList, QScrollArea* tempStorage, int x, int y,
                       SealPictrue* seal, QString pdfFile, int page,
                       QMap<QString,QByteArray>* temp)
 {
    PdfUtils* pdfUtils = new PdfUtils(page);
    pdfUtils->setSealInfo(sealUpdate);
    pdfUtils->openSealFile(pdfFile);
    temp->insert("key",pdfUtils->getInfoAbstract());
    Constructor* constructor = new Constructor();
    constructor->loadInfo(seal, pdfUtils->getPageObj(), pdfUtils->getInfoAbstract()
                          ,page, pdfUtils->getOffset(), pdfUtils->getSize(),
                          pdfUtils->getInfo(), pdfUtils->getRoot(), sealUpdate);
    qDebug()<<sealUpdate->size();
    constructor->setCoordinate(x, y);
    constructor->startConstructor();
    PdfWriter* pdfWriter = new PdfWriter();
    pdfWriter->writeSeal(pdfFile, constructor->getInfo());
    (*certification).append(*constructor->getCertification());
    (*sealUpdate).append(*constructor->getInfo());
    Calculator* calculator = new Calculator();
    temp->insert("info",calculator->countFileHash(pdfFile).toLocal8Bit());
    pdfUtils->openPdf(pdfFile);
    pdfUtils->getPdfImage(page, imageList);
    PdfCanvas* pdfCanvas = new PdfCanvas();
    pdfCanvas->drawCanvas(imageList,tempStorage,page);
    delete pdfCanvas;
    pdfUtils->freeResource();
    delete pdfUtils;
    delete constructor;
 }

void PdfDocument::unseal(QVector<QImage>** imageList, QScrollArea* tempStorage, int pageNum, QString pdfFileName)
{
    pdfUtils = new PdfUtils();
    pdfUtils->openPdf(pdfFileName);
    pdfUtils->getPdfImage(pageNum, imageList);
    PdfCanvas* pdfCanvas = new PdfCanvas();
    pdfCanvas->drawCanvas(imageList,tempStorage,pageNum);
    delete pdfCanvas;
    delete pdfUtils;
}
