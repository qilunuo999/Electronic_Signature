#include "include/utils/unseal.h"


Unseal::Unseal()
{
    info = new QVector<QByteArray>();
    unsealInfo = new QVector<QByteArray>();
}

void Unseal::setSealId(int id)
{
    sealId = id;
}

void Unseal::setSealInfo(QVector<QVector<QByteArray>>* seal)
{
    this->seal = seal;
}

void Unseal::setCertification(QVector<QVector<QByteArray>>* certification)
{
    this->certification = certification;
}

void Unseal::setFileSize(qint64 size)
{
    this->size = size;
}

bool Unseal::constructUnsealInfo()
{
    int i = 1, temp = 0;
    int count = 0;//记录读取到的endobj次数
    int subscript = findSubscript();
    countSeal(subscript);
    saveLastUpdate(subscript);
    unsealInfo->append("\n%seal n " + this->page + " y \n");//撤章增量更新的开头

    while(count != 1)
    {
        unsealInfo->append(info->at(i++));
        if(unsealInfo->at(unsealInfo->size() - 1) == "endobj\n")
        {
            count++;
        }
    }
    unsealInfo->append(info->at(i++));
    QList<QByteArray> temp2 = info->at(i++).split(' ');
    temp = getSealNo(subscript);//获取印章在盖章页面的序号
    qDebug()<<temp;
    if(temp == 0)
    {
        qDebug()<<"search is failure!\n";
        return false;
    }
    else
    {
        //移除内容对象中的印章内容对象
        temp2.removeAt(temp2.size()-1-temp*3);
        temp2.removeAt(temp2.size()-1-temp*3);
        temp2.removeAt(temp2.size()-1-temp*3);
        qDebug()<<"unseal,temp2"<<temp2;
        if(temp2.size()-1-temp*3 < 0)//表示删除当前页面最新盖的印章
        {
            QByteArray temp3("");
            for(int j = 0; j < temp2.size(); j++)
            {
                temp3 += temp2.at(j);
                if(j != temp2.size()-1)
                {
                    temp3 += " ";
                }
            }
            qDebug()<<"unseal,temp3"<<temp3;
            unsealInfo->append("["+temp3);
        }
        else
        {
            QByteArray temp3("");
            for(int j = 0; j < temp2.size(); j++)
            {
                temp3 += temp2.at(j);
                if(j != temp2.size()-1)
                {
                    temp3 += " ";
                }
            }
            unsealInfo->append(temp3);
        }
        unsealInfo->append(info->at(i));
    }
    constructCrossTable();
    constructTrailer();
    return true;
}

QVector<QVector<QByteArray>> *  Unseal::getUnsealInfo()
{
    seal->append(*unsealInfo);
    return seal;
}

int Unseal::findSubscript()
{
    int subscript = 0
            ,page = 0
            ,i;
    QList<QByteArray> temp;
    for(i = 0; i < seal->size(); i++)
    {
        temp = seal->at(i).at(0).split(' ');//印章信息开头格式为%Seal 印章编号 对应页码 是否可用标记
        if(temp.at(1) == QByteArray::number(sealId) && temp.at(3) == "y")
        {
            page = temp.at(2).toInt();
        }
    }
    for(i = 0; i < seal->size(); i++)
    {
        temp = seal->at(i).at(0).split(' ');
        if(temp.at(2) == QByteArray::number(page))
        {
            subscript = i;
        }
    }
    return subscript;
}

void Unseal::saveLastUpdate(int subscript)
{
    for(int i = 0; i < seal->at(subscript).size(); i++)
    {
        info->append(seal->at(subscript).at(i));
    }
}

int Unseal::getSealNo(int subscript)
{
    int count = 0, page = 0,i;
    QList<QByteArray> temp;
    temp =  seal->at(subscript).at(0).split(' ');
    page = temp.at(2).toInt();
    for(i = 0; i < seal->size(); i++)
    {
        temp = seal->at(i).at(0).split(' ');
        if(sealId == temp.at(1).toInt())
        {
            count++;
            return count;
        }
        if(temp.at(1) != "n" && page == temp.at(2).toInt() && temp.at(3) == "y")
        {
            count++;
        }
    }
    return count;
}

void Unseal::alterSealSign()
{
    int number = 0;//有效印章数
    QList<QByteArray> temp;
    for(int i = 0; i < seal->size(); i++)
    {
        temp = seal->at(i).at(0).split(' ');
        if(temp.at(1) != "n" && temp.at(3) == "y")
        {
            number ++;
        }
        if(temp.at(1).toInt() == sealId)
        {
            (*seal)[i][0].replace(seal->at(i).at(0).size() - 3, 1,"n");
            certification->remove(number-1);
        }
    }
}

void Unseal::countSeal(int subscript)
{
    int count = 0, page = 0,i;
    QList<QByteArray> temp;
    temp =  seal->at(subscript).at(0).split(' ');
    this->page = temp.at(2);
    page = temp.at(2).toInt();
    for(i = 0; i < seal->size(); i++)
    {
        temp = seal->at(i).at(0).split(' ');
        if(page == temp.at(2).toInt() && temp.at(3) == "y")
        {
            count++;
        }
    }
    sealNum = count;
}

void Unseal::constructCrossTable()
{
    qint64 vectorSize = 0;
    qint64 tempOffset = 0;
    for(int i = 0; i < unsealInfo->size(); i++)
    {
        vectorSize += unsealInfo->at(i).size();
    }
    QByteArray pageObj, contentObj;
    QList<QByteArray> temp;
    temp = unsealInfo->at(1).split(' ');
    pageObj = temp.at(0);
    temp = unsealInfo->at(unsealInfo->size() - 3).split(' ');
    contentObj = temp.at(0);
    unsealInfo->append("xref\n");
    unsealInfo->append("0 1 \n");
    unsealInfo->append("0000000000 65535 f\n");
    unsealInfo->append(pageObj + " 1\n");
    tempOffset += vectorSize + size;
    constructIndex(tempOffset);
    unsealInfo->append(contentObj + " 1\n");
    tempOffset += vectorSize - (unsealInfo->at(unsealInfo->size() - 1).size() + unsealInfo->at(unsealInfo->size() - 2).size());
    constructIndex(tempOffset);
    newOffset += tempOffset + unsealInfo->at(unsealInfo->size() - 1).size() + unsealInfo->at(unsealInfo->size() - 2).size();
}

void Unseal::constructTrailer()
{
    int i = 0;
    while(seal->at(seal->size() - 1).at(i) != "startxref\n")
    {
        if(seal->at(seal->size() - 1).at(i) == "trailer\n")
        {
            while(seal->at(seal->size() - 1).at(i) != "startxref\n")
            {
                unsealInfo->append(seal->at(seal->size() - 1).at(i));
                i++;
            }
            break;
        }
        i++;
    }
    unsealInfo->append("startxref\n");
    unsealInfo->append(QByteArray::number(this->newOffset) + "\n");
    unsealInfo->append("%%EOF\n");
    unsealInfo->append("%END    " + QByteArray::number(certification->size()));
}

void Unseal::constructIndex(qint64 fileOffset)
{
    QByteArray temp = QByteArray::number(fileOffset);
    QByteArray temp2;
    for(int i = 0; i < 10 - temp.size(); i++)
    {
        temp2.append("0");
    }
    unsealInfo->append(temp2 + temp + " 00000 n\n");
}
