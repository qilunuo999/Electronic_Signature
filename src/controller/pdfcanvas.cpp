#include"include/controller/pdfcanvas.h"

PdfCanvas::PdfCanvas()
{

}
PdfCanvas::~PdfCanvas()
{

}

void PdfCanvas::drawCanvas( QVector<QImage>** imageList, QScrollArea* tempStorage,int pageNumber)
 {
    QLabel* label;
    QPixmap* pixmap=new QPixmap();
    pixmap->convertFromImage((*imageList)->at(pageNumber));
    *pixmap = pixmap->scaled(1050,1400, Qt::KeepAspectRatio);
    label = new QLabel();
    label->setPixmap(*pixmap);
    label->setAlignment(Qt::AlignCenter);
    tempStorage->setWidget(label);
 }

void PdfCanvas::zoomCanvas(QVector<QImage>** imageList, QScrollArea* tempStorage,int pageNumber, double multipleRate)
{
    QWidget* widget = tempStorage->takeWidget();
    if(widget != nullptr)
    {
        delete widget;
    }
    QLabel* label;
    QPixmap* pixmap=new QPixmap();
    QPixmap pix;
    pixmap->convertFromImage((*imageList)->at(pageNumber));
    pix = pixmap->scaled(QSize(1050,1400)*multipleRate,Qt::KeepAspectRatio);
    label = new QLabel();
    label->adjustSize();
    label->setPixmap(pix);
    label->setAlignment(Qt::AlignCenter);
    tempStorage->setWidget(label);
}

 void PdfCanvas::setHightlight(Poppler::Page* page, QRectF coordinate)
 {
    Poppler::HighlightAnnotation *ann = new Poppler::HighlightAnnotation();
    ann->setBoundary(coordinate);
    ann->setHighlightType(Poppler::HighlightAnnotation::Highlight);
    page->addAnnotation(ann);
    delete ann;
 }

 void PdfCanvas::removeAnnotation(Poppler::Page* page, Poppler::Annotation *ann)
 {
    page->removeAnnotation(ann);
 }
