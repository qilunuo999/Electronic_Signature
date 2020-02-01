#ifndef PDFUTILS_H
#define PDFUTILS_H

#include <QSize>
#include <QObject>
#include <QImage>
#include <QtDebug>
#include <QMessageBox>
#include <QThread>
#include <QListWidget>
#include <QFile>
#include "poppler-qt5.h"
#include "calculator.h"
#include "pdfwriter.h"

//pdf基础工具类
class PdfUtils:public QThread
{
public:
    PdfUtils();//用于判断是否是打开新文档，还是其他操作
    PdfUtils(int pageNum);
    PdfUtils(QObject* par, QListWidget* lWidget);
    ~PdfUtils();
     //将pdf的文档内容转换成图片
     void getPdfImage(int pageNumber, QVector<QImage>** imageList);
    //获取pdf文档及其页面大小的信息和页面总数
     void getPdfDocument(QString filepath);
     //获取PDF文档图片的缩略图
     QImage getPdfThumbnail(int pageNumber);
     //打开pdf文档，用于另存为
     void openPdf(QString filePath);
     //另存为
     void saveOutput(QString filepath);
    //获取pdf总页码
    int getNumPages();
    //获取pdf页面大小
    QSize getPageSize();
    //获取印章增量更新前的光标位置
    qint64 getCursor();
    //装载以保存的印章信息
    void setSealInfo(QVector<QVector<QByteArray>>* seal);
    //获取已存在印章的信息
    QVector<QVector<QByteArray>>* getSealInfo();
    //获取印章证书信息
     QVector<QVector<QByteArray>>* getCertification();
    //打开要盖章的文件
    bool openSealFile(QString fileName);
    //获取PDf间接页面对象
    QVector<QByteArray>* getPageObj();
    //获取文件信息摘要
    QByteArray getInfoAbstract();
    //获取文件偏移量
    int getOffset();
    //获取文件间接对象总数
    int getSize();
    //获取文件信息间接对象号
    int getInfo();
    //获取文件的Root字段
    int getRoot();
    //获取印章信息和证书信息
    void loadSealInfo(QString pdfFileName);
    //计算信息摘要
    void countInfoAbstract(QString fileName);
    //释放资源
    bool freeResource();

protected:
    virtual void run();//该线程用于加载缩略图

private:
    int numPages;//总页数
    QSize pageSize;//页面实际大小
    QListWidget* widget;//加载缩略图目录
    Poppler::Document* doc;
    Poppler::PDFConverter* pdfConverter;
    qint64 cursor;//印章增量更新开始前的光标位置
    QVector<QVector<QByteArray>>* seal;//pdf中已存在印章操作增量更新
    QVector<QVector<QByteArray>>* certification;//印章的证书信息
    QByteArray infoAbstract;//文件信息摘要
    int offset;//原文件偏移量
    QVector<QByteArray>* eof;//文件尾
    QVector<QByteArray>* pagesArray;//PDF页面字段对象(原始文件目录)
    QVector<QByteArray>* kidsArray;//目录
    int rootField;//Root字段
    int sizeField;//Size字段
    int infoField;//Info字段
    QVector<QByteArray>* crossTable;//PDF交叉引用表
    QVector<QByteArray>* pageObj;
    int pageNum;//盖章页面的页数
    /*三种获取盖章pdf信息的方法
     * 1.还没盖过章的
     * 2.盖过一次章的*/
    void getPdfInfo1(QString fp);
    void getPdfInfo2(QString fp);
    //加载所需信息
    bool loadInfo();
    //加载文件页面的目录
    bool loadPagesArray(QString fp);
    //加载各页面的kid(即各页面间接对象的编号)
    bool loadKidArray(QString fp);
    //加载盖章页面的间接对象
    bool loadSealPageObj(QString fp);
};


#endif // PDFUTILS_H
