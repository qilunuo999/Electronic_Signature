#include "include/utils/constructor.h"

Constructor::Constructor()
{
    alteredPageObj = new QVector<QByteArray>();
    image = new QVector<QByteArray>();
    contentsObj = new QVector<QByteArray>();
    contents = new QVector<QByteArray>();
    crosstable = new QVector<QByteArray>();
    trailer = new QVector<QByteArray>();
    sealInfo = new QVector<QByteArray>();
    allInfo = new QVector<QByteArray>();
}

Constructor::~Constructor()
{
    delete alteredPageObj;
    delete image;
    delete contents;
    delete contentsObj;
    delete crosstable;
    delete trailer;
    delete sealInfo;
    delete allInfo;
}

bool Constructor::loadInfo(SealPictrue* seal, QVector<QByteArray>* pageObj, QByteArray infoAbstract, int pageNumber, int offset, int sizeField, int infoField, int rootField, QVector<QVector<QByteArray>>* sealArray)
{
    this->width = seal->getWidth();
    this->height = seal->getHeight();
    this->imageStream = seal->getImageStream();
    this->timeStamp = seal->getTimestamp();
    this->startTime = seal->getStartTime();
    this->endTime = seal->getEndTime();
    this->pubKey = seal->getPubKey();
    this->passwdHash = seal->getPasswdHash();
    this->basicInfo = seal->getBasicInfo();
    this->certification = seal->getCertification();
    this->pageObj = pageObj;
    this->infoAbstract = infoAbstract;
    this->pageNumber = pageNumber;
    this->offset = offset;
    this->sizeField = sizeField;
    this->infoField = infoField;
    this->rootField = rootField;
    this->sealArray = sealArray;
    return true;
}

bool Constructor::setCoordinate(int x, int y)
{
   this->x = x;
    this->y = y;
    return true;
}

bool Constructor::startConstructor()
{
    if(sealArray->isEmpty())
    {
        this->rewritePageObj();
        this->constructContents();
        this->constructImage();
        this->constructCrossTable();
        this->constructTrailer();
        this->constructSealInfo();
        this->setSealNumber();
        this->constructAllInfo();
    }
    else
    {
        if(isExistedPage())//判断是否是已盖章的页面
        {
            int subscript = getSubscript();
            rewriteSealedPageObj(subscript);
            this->constructImage();
            this->constructSealInfo();
            this->constructCrossTable();
            this->constructTrailer();
            this->setSealNumber();
            this->constructAllInfo();
        }
        else
        {
            this->rewritePageObj();
            this->constructContents();
            this->constructImage();
            this->constructSealInfo();
            this->constructCrossTable();
            this->constructTrailer();
            this->setSealNumber();
            this->constructAllInfo();
        }
    }
    return true;
}

bool Constructor::isExistedPage()
{
    QList<QByteArray> temp;
    for(int i = 0; i < this->sealArray->size(); i++)
    {
        temp = this->sealArray->at(i).at(0).split(' ');
        if(pageNumber == temp.at(2).toInt())
        {
            return true;
        }
    }
    return false;
}

int Constructor::getSubscript()
{
    int i,
            temp = 0;//用于记录盖章页面最后一次增量更新的位置
    QList<QByteArray> temp2;
    for(i = 0; i < sealArray->size(); i++)
    {
        temp2 = sealArray->at(i).at(0).split(' ');
        if(pageNumber == temp2.at(2).toInt())
        {
            temp = i;
        }
    }
    return temp;
}

void Constructor::rewriteSealedPageObj(int subscript)
{
    int temp = 1;
    QVector<QByteArray>* ctemp = new QVector<QByteArray>();
    for(int i = 1; i < sealArray->at(subscript).size(); i++)//从1开始是为了不添加原印章标识
    {
        if(sealArray->at(subscript).at(i).left(6) == "endobj")
        {
            alteredPageObj->append(sealArray->at(subscript).at(i));
            ctemp->append(sealArray->at(subscript).at(i + 1));
            ctemp->append(sealArray->at(subscript).at(i + 2));
            ctemp->append(sealArray->at(subscript).at(i + 3));
            break;
        }
        if(sealArray->at(subscript).at(i).left(5) == "/Seal")
        {
            temp++;
        }
        if(sealArray->at(subscript).at(i).left(5) == "/Seal" && sealArray->at(subscript).at( i+1).left(5) != "/Seal")
        {
            alteredPageObj->append(sealArray->at(subscript).at(i));
            alteredPageObj->append("/Seal"+QByteArray::number(temp) + " " + QByteArray::number(sizeField + 1) + " 0 R\n");
            continue;
        }
        alteredPageObj->append(sealArray->at(subscript).at(i));
    }
    rewriteContentsObj(ctemp);
}
void Constructor::rewriteContentsObj(QVector<QByteArray>* ctemp)
{
    QList<QByteArray> temp;
    (*ctemp)[1].insert(1, QByteArray::number(sizeField) + " 0 R ");
    contentsObj = ctemp;
    contents->append(QByteArray::number(sizeField) + " 0 obj\n");
    for(int i = 0; i < this->alteredPageObj->size(); i++)
    {
        if(this->alteredPageObj->at(i).left(5) == "/Seal" && alteredPageObj->at(i+1).left(5) != "/Seal")
        {
            temp = this->alteredPageObj->at(i).split(' ');
            QByteArray* temp2 = new QByteArray("q 75 0 0 75 " + QByteArray::number(x-35) + " " + QByteArray::number(y+20) + " cm "+ temp.at(0) + " Do Q\n");//间接对象的数据流
            contents->append("<</Length " + QByteArray::number(temp2->size()) + ">>\n");
            contents->append("stream\n");
            contents->append(*temp2);
            contents->append("endstream\n");
            contents->append("endobj\n");
            break;
        }
    }
    sizeField--;//为重复利用图片构造方法
}

QVector<QByteArray>* Constructor::getCertification()
{
    return sealInfo;
}

QVector<QByteArray>* Constructor::getInfo()
{
    return this->allInfo;
}

void Constructor::rewritePageObj()
{
    int i, j;
    bool flag1 = true
            ,flag2 = true;
    QList<QByteArray> temp;
    if(pageObj->at(1).left(3) == "<</" || pageObj->at(0).right(2) == ">>")
    {
        QList<QByteArray> temp2;
        if(pageObj->at(1).left(3) == "<</")
        {
            this->alteredPageObj->append(pageObj->at(0));
            temp = pageObj->at(1).split('/');
            this->alteredPageObj->append(" " + temp.at(0) + "\n");
        }
        else
        {
            temp = pageObj->at(0).split('/');
        }
        for(i = 1; i < temp.size(); i++)
        {
            if(temp.at(i).left(8) == "Contents")
            {
              if(temp.at(i).at(9) == '[' || temp.at(i).at(8) == '[')
              {
                  temp2 = temp.at(i).split('[');
                  this->alteredPageObj->append("/" + temp2.at(0) + " " + QByteArray::number(this->sizeField) + " 0 R\n");
                  temp2 = temp2.at(1).split(']');
                  this->oldContents.append(temp2.at(0));
                  flag1 = false;
                  continue;
              }
              else
              {
                  temp2 = temp.at(i).split(' ');
                  this->alteredPageObj->append("/" + temp2.at(0) + " " + QByteArray::number(this->sizeField) + " 0 R\n");
                  this->oldContents.append(temp2.at(1) + " 0 R");
                  flag1 = false;
                  continue;
              }
            }
            if(temp.at(i).left(7) == "XObject")
            {
                this->alteredPageObj->append("/" + temp.at(i) + "\n");
                for(j = i+1; j < temp.size(); j++)
                {
                    if((temp.at(j).left(2) == "Im" || temp.at(j).left(2) == "IM") && (temp.at(j).right(2) == ">>" || temp.at(j).right(2) == "> "))
                    {
                        temp2 = temp.at(j).split(' ');
                        this->alteredPageObj->append("/" + temp2.at(0) + " " + temp2.at(1) + " 0 R" + "\n");
                        if(temp2.at(temp2.size()-1) == "R>>>>" || temp2.at(temp2.size()-1) == ">>>>" ||(temp2.at(temp2.size()-2) == ">>" && temp2.at(temp2.size()-1) == ">>")){
                            this->alteredPageObj->append("/Seal1 "+ QByteArray::number(this->sizeField + 2) + " 0 R\n>>\n>>\n");
                        }else {
                            this->alteredPageObj->append("/Seal1 "+ QByteArray::number(this->sizeField + 2) + " 0 R\n>>\n");
                        }
                        j++;
                        break;
                    }
                    this->alteredPageObj->append("/"+temp.at(j)+"\n");
                }
                i = j;
                this->alteredPageObj->append("/" + temp.at(i) + "\n");
                flag2 = false;
                continue;
            }
            this->alteredPageObj->append("/" + temp.at(i) + "\n");
        }
    }
    else
    {
        //如果页面信息不是全在一行的话
        for(int i = 0; i < pageObj->size(); i++)
        {
            if(pageObj->at(i).left(11) == "/Type /Page")
            {
                temp = pageObj->at(i).split('/');
                this->alteredPageObj->append("/" + temp.at(1) + "\n");
                this->alteredPageObj->append("/" + temp.at(2) + "\n");
                continue;
            }
            if(pageObj->at(i).left(9) == "/Contents")
            {
                if(pageObj->at(i).at(9) == '[' || pageObj->at(i).at(10) == '[')
                {
                    temp = pageObj->at(i).split('[');
                    this->alteredPageObj->append(temp.at(0) + " " + QByteArray::number(this->sizeField) + " 0 R\n");
                    temp = temp.at(1).split(']');
                    this->oldContents.append(temp.at(0).right(temp.at(0).size()-2));
                    flag1 = false;
                    continue;
                }
                else
                {
                    temp = pageObj->at(i).split(' ');
                    this->alteredPageObj->append(temp.at(0) + " " + QByteArray::number(this->sizeField) + " 0 R\n");
                    this->oldContents.append(temp.at(1) + " 0 R");
                    flag1 = false;
                    continue;
                }
            }
            if(pageObj->at(i).left(5) == "/XObj")
            {
                this->alteredPageObj->append(pageObj->at(i));
                temp = pageObj->at(i+1).split(' ');
                this->alteredPageObj->append(temp.at(0) + " " + temp.at(1) + " 0 R\n");
                this->alteredPageObj->append("/Seal1 " + QByteArray::number(this->sizeField + 2) + " 0 R\n>>\n");
                i++;
                flag2 = false;
                continue;
            }
            this->alteredPageObj->append(pageObj->at(i));
        }
    }
    if(alteredPageObj->at(alteredPageObj->size() - 1) != "endobj\n")
    {
        alteredPageObj->append("endobj\n");
    }

    if(flag1)
    {
        QByteArray contents("/Contents " + QByteArray::number(this->sizeField) + " 0 R\n");
        alteredPageObj->insert(searchTyepSubscript(), contents);
    }
    if(flag2)
    {
        QByteArray xobject("/XObject<<\n");
        QByteArray imageObj("/Seal1 "+ QByteArray::number(this->sizeField + 2) + " 0 R\n>>\n");
        int t = searchTyepSubscript();
        alteredPageObj->insert(t + 1, xobject);
        alteredPageObj->insert(t + 2, imageObj);
    }
}

int Constructor::searchTyepSubscript()
{
    int i;
    for(i = 0; i < alteredPageObj->size(); i++)
    {
        if(alteredPageObj->at(i).left(10) == "/Resources")
        {
            return i;
        }
    }
    return 0;
}

void Constructor::constructContents()
{
    QList<QByteArray> temp;
    contentsObj->append(QByteArray::number(sizeField) + " 0 obj\n");
    contentsObj->append("[" + QByteArray::number(sizeField+1) + " 0 R " + this->oldContents + "]\n");
    contentsObj->append("endobj\n");
    contents->append(QByteArray::number(sizeField+1) + " 0 obj\n");
    for(int i = 0; i < alteredPageObj->size(); i++)
    {
        if(alteredPageObj->at(i).left(5) == "/Seal" && alteredPageObj->at(i+1).left(5) != "/Seal")
        {
            temp = alteredPageObj->at(i).split(' ');
            QByteArray* temp2 = new QByteArray("q 75 0 0 75 " + QByteArray::number(x-35) + " " + QByteArray::number(y+20) + " cm "+ temp.at(0) + " Do Q\n");//间接对象的数据流
            contents->append("<</Length " + QByteArray::number(temp2->size()) + ">>\n");
            contents->append("stream\n");
            contents->append(*temp2);
            contents->append("endstream\n");
            contents->append("endobj\n");
            break;
        }
    }
}

void Constructor::constructImage()
{
    image->append(QByteArray::number(this->sizeField + 2) + " 0 obj\n");
    image->append("<</BitsPerComponent 8 /ColorSpace /DeviceRGB /Filter /DCTDecode /Height " + QByteArray::number(this->height) + " /Width " + QByteArray::number(this->width) + " /Length "+ QByteArray::number(this->imageStream.size()) + " /Type /XObject /Subtype /Image >>\n");
    image->append("stream\n");
    image->append(imageStream);
    image->append("endstream\n");
    image->append("endobj\n");
}

void Constructor::constructCrossTable()
{
    Calculator* calculator = new Calculator();
    int totalLength,//各间接对象的总字节数
           pageObjLength = 0,//重写的页面对象的字节数
            constentsObjLength = 0,//内容对象的字节数
            constentsLength = 0,//内容间接对象的字节数
           imageLength = 0;//图片间接对象的字节数

    pageObjLength = calculator->getCharNumber(this->alteredPageObj);
    for(int i = 0; i < contentsObj->size(); i++)
    {
        constentsObjLength += contentsObj->at(i).size();
    }
    for(int i = 0; i < contents->size(); i++)
    {
        constentsLength += contents->at(i).size();
    }

    for(int i = 0; i < image->size(); i++)
    {
        imageLength += image->at(i).size();
    }
    totalLength = pageObjLength + constentsLength + constentsObjLength + imageLength;

    QList<QByteArray> temp;
    QByteArray temp2, temp3;
    temp = alteredPageObj->at(0).split(' ');
    crosstable->append("xref\n");
    crosstable->append("0 1 \n");
    crosstable->append("0000000000 65535 f\n");
    crosstable->append(temp.at(0) + " 1\n");
    constructIndex(totalLength + offset);//构造索引
    crosstable->append(QByteArray::number(this->sizeField) + " 3 \n");
    constructIndex(pageObjLength + totalLength + offset);
    constructIndex(constentsObjLength + pageObjLength + totalLength + offset);
    constructIndex(constentsLength + constentsObjLength + pageObjLength + totalLength + offset);
    newOffset = imageLength + constentsLength + constentsObjLength + pageObjLength + totalLength + offset + sealBeginning.size() + sealInfo->size() + 13;
}

void Constructor::constructTrailer()
{
    trailer->append("trailer\n");
    trailer->append("<<\n");
    trailer->append("/Size " + QByteArray::number(sizeField + 3) + "\n");
    trailer->append("/Info " + QByteArray::number(infoField) + " 0 R\n");
    trailer->append( "/Root " + QByteArray::number(rootField) + " 0 R\n");
    trailer->append(">>\n" );
    trailer->append("startxref\n");
    trailer->append(QByteArray::number(this->newOffset) + "\n");
    trailer->append("%%EOF\n");
}

void Constructor::constructSealInfo()
{
    if(sealArray == nullptr)
    {
        this->sealBeginning = "%seal " + QByteArray::number(1) + " " + QByteArray::number(pageNumber) + " y \n";
        this->sealInfo->append(this->timeStamp);
        this->sealInfo->append(this->startTime);
        this->sealInfo->append(this->endTime);
        this->sealInfo->append(this->passwdHash);
        this->sealInfo->append(this->infoAbstract + '\n');
        this->sealInfo->append(this->pubKey);
        this->sealInfo->append(this->basicInfo);
        this->sealInfo->append(this->certification);
    }
    else
    {
        this->sealBeginning = "\n%seal " + QByteArray::number(sealArray->size() + 1) + " " + QByteArray::number(pageNumber) + " y \n";
        this->sealInfo->append(this->timeStamp);
        this->sealInfo->append(this->startTime);
        this->sealInfo->append(this->endTime);
        this->sealInfo->append(this->passwdHash);
        this->sealInfo->append(this->infoAbstract + "\n");
        this->sealInfo->append(this->pubKey);
        this->sealInfo->append(this->basicInfo);
        this->sealInfo->append(this->certification);
    }
}

void Constructor::setSealNumber()
{
    int number = 0;
    for (int i = 0; i < sealArray->size(); i++)
    {
        QList<QByteArray>temp = sealArray->at(i).at(0).split(' ');
        if(temp.at(1) != "n" && temp.at(3) == "y")
        {
            number++;
        }
    }
    this->sealNumber.append(QByteArray::number(number + 1));
}

void Constructor::constructAllInfo()
{
    allInfo->append(sealBeginning);
    allInfo->append(*alteredPageObj);
    allInfo->append(*contentsObj);
    allInfo->append(imageObj);
    allInfo->append(*contents);
    allInfo->append(*image);
    allInfo->append(*crosstable);
    allInfo->append(*trailer);
    allInfo->append(*sealInfo);
    allInfo->append("%END    " + sealNumber);
}

void Constructor::constructIndex(int fileOffset)
{
    QByteArray temp = QByteArray::number(fileOffset);
    QByteArray temp2;
    for(int i = 0; i < 10 - temp.size(); i++)
    {
        temp2.append("0");
    }
    this->crosstable->append(temp2 + temp + " 00000 n\n");
}
