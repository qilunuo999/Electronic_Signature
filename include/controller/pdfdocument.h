#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QScrollArea>
#include <QListWidget>
#include <QVBoxLayout>
#include <QStandardItem>

#include "include/utils/pdfutils.h"
#include "pdfcanvas.h"
#include "include/utils/sealpictrue.h"
#include"include/utils/constructor.h"
#include "include/utils/calculator.h"

//pdf文档处理类
class PdfDocument:public QObject
{
public:
    PdfDocument();
    PdfDocument(QString fp, bool isOpen = false);
    ~PdfDocument();
    int openPDF(QVector<QImage>** imageList, QScrollArea* tempStorage, QListWidget* lWidget,
                double pictureRate[]);//返回pdf文档的总页数
    void savePDF(QString fp, QString saveFp);
    //页面跳转
    void jumpPage(int pageNum, QVector<QImage>** imageList, QScrollArea* tempStorage);
    //缩放pdf图片
    void zoomPDF(QVector<QImage>** imageList, QScrollArea* tempStorage, int pageNum,
                 double multipleRate);
    //盖章
    void seal(QVector<QImage>** imageList, QScrollArea* tempStorage,
              int x, int y, SealPictrue* seal, QString pdfFile,
              int page,QMap<QString,QByteArray>*);
    //查找功能
    QMap<int, QList<QRectF> > search(const QString &text);
    //打印
    void printPDF(QString fp);
    //验证
    void vertification();
    //计算pdf图片实际大小和渲染后的大小的比值
    double countSizeRate();
    //加载证书信息
    void setCertification(QVector<QVector<QByteArray>>* certification);
    //加载印章信息
    void setSealInfo(QVector<QVector<QByteArray>>* seal);
    //获取第一次印章增量更新前的pos
    qint64 getPos();
    //获取已存在印章的信息
    QVector<QVector<QByteArray>>* getSealInfo();
    //获取印章证书信息
     QVector<QVector<QByteArray>>* getCertification();
     //获取文件信息摘要
     QByteArray getInfoAbstract();
     //获取文件大小
     qint64 getFileSize();
     //撤章
     void unseal(QVector<QImage>** imageList, QScrollArea* tempStorage, int pageNum,
                 QString pdfFileName);
private:
    QString fp;
    int numPages;
    qint64* pos;
    QSize pageSize;
    PdfUtils* pdfUtils;
    QVector<QImage>** imageList;
    QByteArray infoAbstract;//文件信息摘要
    QVector<QVector<QByteArray>>* sealUpdate;//pdf中已存在印章操作增量更新
    QVector<QVector<QByteArray>>* certification;//印章的证书信息
};

#endif // PDFDOCUMENT_H
