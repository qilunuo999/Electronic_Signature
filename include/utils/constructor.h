#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include "sealpictrue.h"
#include "calculator.h"

class Constructor
{
public:
    Constructor();
    ~Constructor();

    //信息装载
    bool loadInfo(SealPictrue* seal, QVector<QByteArray>* pageObj, QByteArray infoAbstract, int pageNumber, int offset, int sizeField, int infoField, int rootField, QVector<QVector<QByteArray>>* sealArray);
    //设置盖章位置
    bool setCoordinate(int x, int y);
    //开始构造信息
    bool startConstructor();
    //获取构造完成后的信息
    QVector<QByteArray>* getInfo();
    //获取证书信息
    QVector<QByteArray>* getCertification();

private:
    //印章在pdf中的坐标
    int x;
    int y;
    int width;//印章宽度
    int height;//印章高度
    QByteArray imageStream;//图片流
    QByteArray timeStamp;//时间戳
    QByteArray startTime;//起效时间
    QByteArray endTime;//有效期截止时间
    QByteArray pubKey;//公钥
    QByteArray passwdHash;//密码哈希
    QByteArray basicInfo;//基础信息
    QByteArray certification;//印章证书
    QVector<QByteArray>* pageObj;//要盖章的页面的间接对象
    QByteArray infoAbstract;//文件信息摘要
    int pageNumber;//盖章页面编号
    int offset;//原文件偏移量
    int newOffset;//新构造完成后的文件偏移量
    int sizeField;//原pdf文件的对象总数
    int infoField;//原pdf文件的信息对象标号
    int rootField;//原pdf文件的root对象的编号
    QVector<QVector<QByteArray>>* sealArray;//已有印章信息
    QVector<QByteArray>* alteredPageObj;//修改后的页面对象
    QByteArray sealBeginning;//印章开头
    QVector<QByteArray>* contentsObj;//新的内容对象
    QByteArray imageObj;//新的图片对象
    QByteArray oldContents;//旧内容对象
    QByteArray oldImage;//旧图片对象
    QVector<QByteArray>* contents;//要添加的新内容间接对象
    QVector<QByteArray>* image;//要添加的图片间接对象
    QVector<QByteArray>* crosstable;//交叉引用表
    QVector<QByteArray>* trailer;//文件尾
    int sealNum;//原印章数
    QByteArray sealNumber;//盖章后总印章数
    QVector<QByteArray>* sealInfo;//根据印章图片的信息重新组合
    QVector<QByteArray>* allInfo;//构建完整的增量更新信息
    //重写页面对象
    void rewritePageObj();
    //构造内容对象
    void constructContents();
    //构造图片对象
    void constructImage();
    void constructImage2();
    //构造交叉引用表
    void constructCrossTable();
    //构造文件尾
    void constructTrailer();
    //构造印章信息
    void constructSealInfo();
    //设置印章个数
    void setSealNumber();
    //构造完整的增量更新
    void constructAllInfo();
    //交叉引用表索引构造
    void constructIndex(int fileOffset);
    //判断是否为已盖章页面
    bool isExistedPage();
    //查找盖章页面最后一次增量更新在印章Vector中的位置
    int getSubscript();
    //重写盖章页面
    void rewriteSealedPageObj(int subscript);
   //重写盖章页面的间接内容对象
    void rewriteContentsObj(QVector<QByteArray>* ctemp);
    //查找Type下标
    int searchTyepSubscript();
};

#endif // CONSTRUCTOR_H0
