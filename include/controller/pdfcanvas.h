#ifndef PDFCANVAS_H
#define PDFCANVAS_H

#include<QWidget>
#include<QMap>
#include <QMapIterator>
#include <QLabel>
#include <QRectF>
#include <QScrollArea>
#include"include/utils/pdfutils.h"

//pdf渲染类
class PdfCanvas
{
public:
    PdfCanvas();
    ~PdfCanvas();
    //绘制图片
    void drawCanvas(QVector<QImage>** imageList, QScrollArea* tempStorage,int pageNumber);
    void zoomCanvas(QVector<QImage>** imageList, QScrollArea* tempStorage,int pageNumber, double multipleRate);
    void drawSeal(Poppler::Page* page, QRectF coordinate);
    void setHightlight(Poppler::Page* page, QRectF coordinate);//设置高亮注释
    void removeAnnotation(Poppler::Page* page, Poppler::Annotation *ann);//可同时用于撤章和清除注释

private:
    int numPages;
};

#endif // PDFCANVAS_H
