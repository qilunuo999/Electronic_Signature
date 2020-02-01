#include"include/utils/pdfutils.h"

#define MAX 9999999

PdfUtils::PdfUtils()
{

}

PdfUtils::PdfUtils(int pageNum)
{
    this->pageNum = pageNum;
    this->crossTable = new QVector<QByteArray>();
    this->eof = new QVector<QByteArray>();
    this->pagesArray = new QVector<QByteArray>();
    this->kidsArray = new QVector<QByteArray>();
    this->pageObj = new QVector<QByteArray>();
}

PdfUtils::PdfUtils(QObject* par, QListWidget* lWidget):QThread (par)
{
    this->widget = lWidget;
    this->seal = new QVector<QVector<QByteArray>> ();
    this->certification = new QVector<QVector<QByteArray>> ();
}
PdfUtils::~PdfUtils()
{
    delete doc;
    requestInterruption();
    quit();
    wait();
}

void PdfUtils::getPdfDocument(QString filepath)
{
    numPages = 0;
    doc =  Poppler::Document::load(filepath);
    if(!doc || doc->isLocked())
    {
        delete doc;
        QMessageBox::warning(nullptr,"警告","PDF文档开启失败,请检查文档是否存在或是否已打开",QMessageBox::Cancel);
        return;
    }
    // 更加清晰
    doc->setRenderHint(Poppler::Document::Antialiasing);
    doc->setRenderHint(Poppler::Document::TextAntialiasing);
    numPages = doc->numPages();
    Poppler::Page* pdfPage = doc->page(0);
    pageSize = pdfPage->pageSize();
    // getPdfImage(0, imageList);
    delete pdfPage;
}

void PdfUtils::getPdfImage(int pageNumber,QVector<QImage>** imageList)
{
    QImage image;
    Poppler::Page* pdfPage;
    doc->setRenderHint(Poppler::Document::Antialiasing);
    doc->setRenderHint(Poppler::Document::TextAntialiasing);
    pdfPage = doc->page(pageNumber);
    if(pdfPage == nullptr)
    {
        delete pdfPage;
        QMessageBox::warning(nullptr,"警告","该PDF文档无内容",QMessageBox::Cancel);
        return ;
    }
    image = pdfPage->renderToImage(128,128);
    if(image.isNull())
    {
        delete pdfPage;
        QMessageBox::warning(nullptr,"警告","获取PDF文档图像失败",QMessageBox::Cancel);
        return ;
    }
    (*imageList)->replace(pageNumber, image);
    delete pdfPage;
}

QImage PdfUtils::getPdfThumbnail(int pageNumber)
{
    QImage image;
    Poppler::Page* pdfPage;
    pdfPage = doc->page(pageNumber);
    if(pdfPage == nullptr)
    {
        delete pdfPage;
        QMessageBox::warning(nullptr,"警告","该PDF文档无内容",QMessageBox::Cancel);
        return image;
    }
    image = pdfPage->renderToImage(10,10);
    if(image.isNull())
    {
        delete pdfPage;
        QMessageBox::warning(nullptr,"警告","获取PDF文档图像失败",QMessageBox::Cancel);
        return image;
    }
    delete pdfPage;
    return image;
}

int PdfUtils::getNumPages()
{
    return numPages;
}

QSize PdfUtils::getPageSize()
{
    return pageSize;
}

void PdfUtils::run()
{
    for(int i = 0; i < getNumPages() && !isInterruptionRequested(); i++)
    {
        if(widget == nullptr)
        {
            return;
        }
        QPixmap* pixmap=new QPixmap();
        pixmap->convertFromImage(getPdfThumbnail(i));
        QPixmap pix = pixmap->scaled(QSize(150,200));
        QListWidgetItem *item = new QListWidgetItem;
        item->setIcon(QIcon(pix));
        item->setText(QString::number(i+1,10));
        widget->addItem(item);
    }

}

void PdfUtils::openPdf(QString filePath)
{
    this->doc = Poppler::Document::load(filePath);
    this->pdfConverter = this->doc->pdfConverter();
    this->pdfConverter->setPDFOptions(Poppler::PDFConverter::WithChanges);
}

void PdfUtils::saveOutput(QString filePath)
{
    this->pdfConverter->setOutputFileName(filePath);
    this->pdfConverter->convert();
}


bool PdfUtils::openSealFile(QString fileName)
{
    QFile pdfFile(fileName);
    if(!pdfFile.open(QIODevice::ReadOnly))
    {
        qDebug()   << "Cannot open file for Writing 141";
        return 0;
    }
    pdfFile.seek(pdfFile.size()-4);//文件末尾有标记盖章次数的数字，占1个字节。
    qDebug()<<pdfFile.size()<<endl;
    QByteArray time(pdfFile.readLine());
    if(time.at(time.size()-1) == '\n')
    {
        time.remove(time.size()-1,1);
    }
    pdfFile.close();
    bool ok;
    switch (time.toInt(&ok,10))
    {
    case 0:getPdfInfo1(fileName);return true;
    default:if(ok){getPdfInfo2(fileName);return true;}break;
    }
    return false;
}

void PdfUtils::setSealInfo(QVector<QVector<QByteArray>>* seal)
{
    this->seal = seal;
}

qint64 PdfUtils::getCursor()
{
    return cursor;
}
QVector<QVector<QByteArray>>* PdfUtils::getSealInfo()
{
    return this->seal;
}
QVector<QVector<QByteArray>>* PdfUtils::getCertification()
{
    return this->certification;
}

QVector<QByteArray>* PdfUtils::getPageObj()
{
    return this->pageObj;
}

int PdfUtils::getOffset()
{
    return this->offset;
}

int PdfUtils::getSize()
{
    return this->sizeField;
}

int PdfUtils::getInfo()
{
    return this->infoField;
}

int PdfUtils::getRoot()
{
    return this->rootField;
}

QByteArray PdfUtils::getInfoAbstract()
{
    return this->infoAbstract;
}

void PdfUtils::getPdfInfo1(QString fp)
{
    QFile pdfFile(fp);
    QByteArray temp;//临时存储字符串

    countInfoAbstract(fp);
    if(!pdfFile.open(QIODevice::ReadOnly))
    {
        qDebug()   << "Cannot open file by way 1!\n";
        return;
    }

    while(!pdfFile.atEnd())
    {
        temp= pdfFile.readLine();
        if(temp.left(4) == "xref" || temp.right(5) == "xref\n" || temp.right(6) == "xref\r\n")//交叉引用表的开始，有些pdf文档结构不是很规范，会有一部分将xref写在前一行的末尾
        {

            temp = pdfFile.readLine();
            while(temp.left(7) != "trailer")//文件尾
            {
                crossTable->append(temp);
                temp = pdfFile.readLine();
            }
        }
        if((temp.left(8) == "trailer\n" || temp.left(9) == "trailer\r\n"))
        {
            qint64 tempPos = pdfFile.pos();
            while(pdfFile.readLine().left(4) != "xref" && !pdfFile.atEnd());//如果不是最后一个的文件尾就重新寻找
            if(pdfFile.pos() != pdfFile.size())
            {
                pdfFile.seek(tempPos);
                continue;
            }
            pdfFile.seek(tempPos);

            eof->append(temp);
            while(!pdfFile.atEnd())
            {
                temp = pdfFile.readLine();
                eof->append(temp);
            }
        }
        if((temp.left(9) == "trailer\r<" || temp.left(8) == "trailer<"))
        {
            qint64 tempPos = pdfFile.pos();
            while(pdfFile.readLine().left(4) != "xref" && !pdfFile.atEnd());
            if(pdfFile.pos() != pdfFile.size())
            {
                pdfFile.seek(tempPos);
                continue;
            }
            pdfFile.seek(tempPos);

            eof->append(temp);
        }
    }
    if(!loadInfo())
    {
        qDebug()<<"信息装载";
        return;
    }
    pdfFile.close();
    loadPagesArray(fp);
    loadKidArray(fp);
    loadSealPageObj(fp);
}

void PdfUtils::getPdfInfo2(QString fp)
{
    QFile pdfFile(fp);
    QByteArray temp, temp2, temp3;//临时存储字符串

    countInfoAbstract(fp);
    if(!pdfFile.open(QIODevice::ReadOnly))
    {
        qDebug()   << "Cannot open file for Writing 250";
        return;
    }

    while(!pdfFile.atEnd())
    {
        temp= pdfFile.readLine();
        if(temp.left(4) == "xref"|| temp.right(5) == "xref\n" || temp.right(6) == "xref\r\n")//交叉引用表的开始，有些pdf文档结构不是很规范，会有一部分将xref写在前一行的末尾
        {
            temp2 = pdfFile.readLine();
            while(temp2.left(7) != "trailer")//文件尾
            {
                crossTable->append(temp2);
                temp2 = pdfFile.readLine();
            }
            break;
        }
    }

    //获取文件尾
    for(int i = 0; i < this->seal->at(this->seal->size() - 1).size(); i++){
        if(this->seal->at(this->seal->size() - 1).at(i).left(7) == "trailer")
        {
            while (this->seal->at(this->seal->size() - 1).at(i).left(5) != "%%EOF")
            {
                this->eof->append(this->seal->at(this->seal->size() - 1).at(i));
                i++;
            }
            this->eof->append(this->seal->at(this->seal->size() - 1).at(i));
            break;
        }
    }

    if(!loadInfo())
    {
        qDebug()<<"信息装载";
        return;
    }
    pdfFile.close();
    loadPagesArray(fp);
    loadKidArray(fp);
    loadSealPageObj(fp);
}

bool PdfUtils::loadInfo()
{
    QList<QByteArray> temp;
    int i, j;
    offset = 0;
    for(i = 0; i < eof->size(); i++)
    {
        if(eof->at(i).left(3) == "<</" || eof->at(i).left(8) == "trailer<" || eof->at(i).left(9) == "trailer\r<")
        {
            temp = eof->at(i).split('/');
            for(j = 0; j < temp.size(); j++)
            {
                if(temp.at(j).left(4) == "Size")
                {
                    sizeField = temp.at(j).split(' ').at(1).toInt();
                }
                if(temp.at(j).left(4) == "Root")
                {
                    rootField = temp.at(j).split(' ').at(1).toInt();
                }
                if(temp.at(j).left(4) == "Info")
                {
                    infoField = temp.at(j).split(' ').at(1).toInt();
                }
            }
            if(eof->size() <= 1)
            {
                temp = eof->at(0).split('\r');
                QByteArray temp2;
                for(int k = 0; k < temp.at(2).size(); k++)
                {
                    if(temp.at(2).at(k) >= 48 && temp.at(2).at(k) <= 57)
                    {
                        temp2.append(temp.at(2).at(k));
                    }
                }
                offset = temp2.toInt();
            }
            else
            {
                if(eof->at(i+1).left(5) == "start")
                {
                    QByteArray temp2;
                    int temp3 = i + 2;
                    for(int k = 0; k < eof->at(temp3).size(); k++)
                    {
                        if(eof->at(temp3).at(k) >= 48 && eof->at(temp3).at(k) <= 57)
                        {
                            temp2.append(eof->at(temp3).at(k));
                        }
                    }
                    offset = temp2.toInt();
                }
            }
            break;
        }
        else
        {
            if(eof->at(i).left(5) == "/Size")
            {
                sizeField = eof->at(i).split(' ').at(1).split('\n').at(0).toInt();
            }
            if(eof->at(i).left(5) == "/Root")
            {
                rootField = eof->at(i).split(' ').at(1).toInt();
            }
            if(eof->at(i).left(5) == "/Info")
            {
                infoField = eof->at(i).split(' ').at(1).toInt();
            }
            if(eof->at(i).left(5) == "start")
            {
                QByteArray temp2;
                j = i + 1;
                for(int k = 0; k < eof->at(j).size(); k++)
                {
                    if(eof->at(j).at(k) >= 48 && eof->at(j).at(k) <= 57)
                    {
                        temp2.append(eof->at(j).at(k));
                    }
                }
                offset = temp2.toInt();
            }
        }
    }
    return true;
}

bool PdfUtils::loadPagesArray(QString fp)
{
    QByteArray temp;
    qint64 seek = crossTable->at(rootField + 1).split(' ').at(0).toInt();
    QFile pdfFile(fp);
    if(!pdfFile.open(QIODevice::ReadOnly))
    {
        qDebug()   << "Cannot open file for Writing 283";
        return false;
    }

    pdfFile.seek(seek);
    temp = pdfFile.readLine();

    if(temp.right(7) == "endobj\n" || temp.right(8) == "endobj\r\n")
    {
        QList<QByteArray> temp2 = temp.split('/');
        for (int i = 0; i < temp2.size(); i++)
        {
            if(temp2.at(i).left(4) == "Page")
            {
                QList<QByteArray> temp3;
                temp3 = temp2.at(i).split(' ');
                for(int j = 1; j < temp3.size(); j += 3)
                {
                    pagesArray->append(temp3.at(j));
                }
            }
        }
    }
    else
    {
        if(temp.right(3) != ">>\n" && temp.right(4) != ">>\r\n"){
            temp = pdfFile.readLine();
        }
        if(temp.right(3) == ">>\n" || temp.right(4) == ">>\r\n")
        {
            QList<QByteArray> temp2 = temp.split('/');
            for (int i = 0; i < temp2.size(); i++)
            {
                if(temp2.at(i).left(4) == "Page" && temp2.at(i).left(13) != "Page>>\rendobj")
                {
                    QList<QByteArray> temp3;
                    temp3 = temp2.at(i).split(' ');
                    for(int j = 1; j < temp3.size(); j += 3)
                    {
                        if(temp3.at(j) != ""){
                            pagesArray->append(temp3.at(j));
                        }
                    }
                }
            }
        }
        else
        {
            while(temp.left(4) != "Page")
            {
                temp = pdfFile.readLine();
            }
            QList<QByteArray> temp2;
            temp2 = temp.split(' ');
            for(int j = 1; j < temp2.size(); j += 3)
            {
                if(temp2.at(j) != ""){
                    pagesArray->append(temp2.at(j));
                }
            }
        }
    }
    pdfFile.close();
    return true;
}

bool PdfUtils::loadKidArray(QString fp)
{
    QFile pdfFile(fp);
    qint64 seek;
    QByteArray temp, kids;
    bool isCatalog = false;

    if(!pdfFile.open(QIODevice::ReadOnly))
    {
        qDebug()   << "Cannot open file for Writing 391";
        return false;
    }

    QList<QByteArray> stack;
    for(int i = 0; i < pagesArray->size(); i++)
    {
        stack.push_front(pagesArray->at(pagesArray->size()-1-i));
    }

    while(!stack.isEmpty())
    {
        kids = stack.first();
        stack.pop_front();
        if(kids != ""){
            if(kids.at(0) == '['){
                kids.remove(0,1);
            }
        }
        seek = crossTable->at(kids.toInt() + 1).split(' ').at(0).toInt();
        pdfFile.seek(seek);
        temp = pdfFile.readLine();

        //判断是页面对象还是目录对象
        if(temp.right(7) == "endobj\n" || temp.right(8) == "endobj\r\n" || temp.right(6) == "xref\r\n")
        {
            QList<QByteArray> temp2 = temp.split('/');
            for (int i = 0; i < temp2.size(); i++)
            {
                if(temp2.at(i).left(4) == "Kids")
                {
                    QList<QByteArray> temp3;
                    temp3 = temp2.at(i).split(' ');
                    for(int j = 1; j*3 < temp3.size(); j++)
                    {
                        stack.push_front(temp3.at(temp3.size()-3*j-1));
                    }
                    isCatalog = true;
                    break;
                }
            }
        }
        else
        {
            if(temp.right(3) != ">>\n" && temp.right(4) != ">>\r\n"){
                temp = pdfFile.readLine();
            }
            if(temp.right(3) == ">>\n" || temp.right(4) == ">>\r\n")
            {
                QList<QByteArray> temp2 = temp.split('/');
                for (int i = 0; i < temp2.size(); i++)
                {
                    if(temp2.at(i).left(4) == "Kids")
                    {
                        QList<QByteArray> temp3;
                        temp3 = temp2.at(i).split(' ');
                        for(int j = 1; j*3 < temp3.size(); j++)
                        {
                            stack.push_front(temp3.at(temp3.size()-3*j-1));
                        }
                        isCatalog = true;
                        break;
                    }
                }
            }
            else
            {
                while(temp.left(4) != "Kids")
                {
                    temp = pdfFile.readLine();
                }
                QList<QByteArray> temp2;
                temp2 = temp.split(' ');
                for(int j = 1; j*3 < temp2.size(); j++)
                {
                    stack.push_front(temp2.at(temp2.size()-3*j-1));
                    isCatalog = true;
                }
            }
        }
        if(!isCatalog)
        {
            kidsArray->append(kids);
            isCatalog = false;
        }
        isCatalog = false;
    }

    pdfFile.close();
    return true;
}

bool PdfUtils::loadSealPageObj(QString fp)
{
    QFile pdfFile(fp);
    if(!pdfFile.open(QIODevice::ReadOnly))
    {
        qDebug()   << "Cannot open file for Writing 590";
        return false;
    }
    qint64 seek = crossTable->at(kidsArray->at(pageNum).toInt() + 1).split(' ').at(0).toInt();
    pdfFile.seek(seek);
    QByteArray temp;
    temp = pdfFile.readLine();
    if(temp == kidsArray->at(pageNum)+" 0 obj\n" || temp == kidsArray->at(pageNum)+" 0 obj\r\n")
    {
        while(temp != "endobj\n" && temp != "endobj\r\n")
        {
            pageObj->append(temp);
            temp = pdfFile.readLine();
        }
        pageObj->append(temp);
    }
    else
    {
        QList<QByteArray> temp2 = temp.split('\r');
        for(int i = 0; i < temp2.size(); i++)
        {
            if(temp2.at(i).left(6) != "endobj")
            {
                pageObj->append(temp2.at(i));
            }
            else if(temp2.at(i).left(6) == "endobj")
            {
                pageObj->append(temp2.at(i));
                break;
            }
        }
    }
    pdfFile.close();
    return true;
}

void PdfUtils::loadSealInfo(QString pdfFileName)
{
    QFile pdfFile(pdfFileName);
    bool isY = false;
    int flag = 0;//标记，用于获取第一次印章增量更新前的光标位置
    QByteArray temp;//临时存储字符串

    if(!pdfFile.open(QIODevice::ReadOnly))
    {
        qDebug()   << "Cannot open file for Writing 250";
        return;
    }
    QVector<QByteArray>* seal;//用于临时存储印章
    QVector<QByteArray>* certification;//用于临时储存印章的证书信息
    while(!pdfFile.atEnd())
    {
        temp = pdfFile.readLine();
        if(temp.left(5) == "%seal")
        {
            if(temp.at(temp.size() - 3) == 'y')
            {
                isY = true;
            }
            if(flag == 0)
            {
                cursor = pdfFile.pos() - temp.size();
                flag = 1;
            }
            seal = new QVector<QByteArray>();
            while(temp.left(4) != "%END")
            {
                seal->append(temp);
                temp = pdfFile.readLine();
                if(temp.left(5) == "%%EOF")
                {
                    certification = new QVector<QByteArray>();
                    while(temp.left(4) != "%END")
                    {
                        seal->append(temp);
                        temp = pdfFile.readLine();
                        if(isY)
                        {
                            certification->append(temp);
                        }
                    }
                    if(isY && certification->size() > 1)
                    {
                        this->certification->append(*certification);
                    }
                    seal->append(temp);
                    isY = false;
                }
            }
            this->seal->append(*seal);
        }
    }
    pdfFile.close();
}

void PdfUtils::countInfoAbstract(QString fileName)
{
    QFile pdfFile(fileName);
    QByteArray temp;//临时存储字符串

    if(!pdfFile.open(QIODevice::ReadOnly))
    {
        qDebug()   << "Cannot open file for Writing 250";
        return;
    }
    for(int i = 0; i < 10; i++)
    {
        temp += pdfFile.readLine();
    }
    Calculator* calculator = new Calculator();
    this->infoAbstract = calculator->sha512(temp.toStdString());
    pdfFile.close();
}

bool PdfUtils::freeResource()
{
    if(!this->crossTable->isEmpty() && !this->eof->isEmpty() && !this->pagesArray->isEmpty() && !this->kidsArray->isEmpty() && !this->pageObj->isEmpty())
    {
        delete this->crossTable;
        delete this->eof;
        delete this->pagesArray;
        delete this->kidsArray;
        delete this->pageObj;
        return true;
    }
    requestInterruption();
    quit();
    wait();
    return false;
}
