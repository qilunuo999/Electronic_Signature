#ifndef UNSEAL_H
#define UNSEAL_H

#include<QByteArray>
#include<QVector>
#include <QDebug>

class Unseal
{
public:
    Unseal();
    void setSealId(int id);
    void setSealInfo(QVector<QVector<QByteArray>>* seal);
    void setCertification(QVector<QVector<QByteArray>>* certification);
    void setFileSize(qint64 size);
    bool constructUnsealInfo();
    //修改印章标记
    void alterSealSign();
    QVector<QVector<QByteArray>> *  getUnsealInfo();

private:
    int sealId;
    int sealNum;
    qint64 size;
    qint64 newOffset;
    QByteArray page;
    QVector<QVector<QByteArray>>* seal;
    QVector<QVector<QByteArray>>* certification;
    QVector<QByteArray>* info;//用于存储要撤的章所在页面的最后一次增量更新
    QVector<QByteArray>* unsealInfo;
    //找到页面最后一次增量更新的地方
    int findSubscript();
    //存储最后一次增量更新
    void saveLastUpdate(int subscript);
    //获取印章在盖章页面的序号
    int getSealNo(int subscript);
    //计算在某一页面的印章个数
    void countSeal(int subscript);
    //构造交叉引用表
    void constructCrossTable();
    //构造文件尾
    void constructTrailer();
     //交叉引用表索引构造
    void constructIndex(qint64 fileOffset);
};

#endif // UNSEAL_H
