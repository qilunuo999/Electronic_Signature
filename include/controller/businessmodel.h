#ifndef BUSINESSMODEL_H
#define BUSINESSMODEL_H

#include <QObject>
#include <QString>
#include <QDomDocument>
#include <QStandardItemModel>
#include <QListWidget>
#include <QScrollArea>
#include <QException>

#include "poppler-qt5.h"

#include "pdfdocument.h"
#include "include/interface/makesealwindow.h"
#include "include/interface/batchvalidation.h"
#include "include/interface/unsealui.h"
#include "include/interface/softinfodialog.h"
#include "include/interface/usermanualdialog.h"
#include "include/interface/logindialog.h"
#include "include/interface/changepassworddialog.h"

//业务处理中心
class BusinessModel:public QObject
{
    Q_OBJECT
public:
    explicit BusinessModel(QObject *parent = nullptr);
    ~BusinessModel();
    void openFile(QString filePath, QVector<QImage>** imageList, QScrollArea* tempStorage, int numPages[], QListWidget* lWidget, double pictureRate[]);
    void saveFile(QString filePath);
    void saveAs(QString filePath, QString saveFp);
    void jumpPage(int pageNum, QVector<QImage>** imageList, QScrollArea* tempStorage, QString filePath);
    void search(QString path);
    void zoom(QVector<QImage>** imageList, int pageNum, double multipleRate, QScrollArea* tempStorage);
    void viewProperties(QString filePath);
    void printing(QString path);
    void seal(QVector<QImage>** imageList, QScrollArea* tempStorage,int x, int y, QString pdfFile, QString imageFile, QString passwd, int pageNum);
    void vertification();
    void unseal(QVector<QImage>** imageList, QScrollArea* tempStorage, int pageNum);
    void makeSeal();
    void userManual();
    void softInfo();
    void viewCertificate();
    void viewTimeStamp();
    void login();//用户登录
    void changePassword();//修改用户密码
    void sendSealedFile();//发送已盖章文件信息
    bool getSubminInfo();
    void freeResource();//释放已申请资源

signals:
    void userOperation(QString);

private slots:
    void setUserInfo(QJsonObject);

    void sendResult(QJsonObject);//盖章文件信息发送结果

    void sealoperate(QJsonObject);//根据验证结果盖章

    void unsealOperate(QJsonObject);

private:
    int numPages;
    qint64* pdfSize;
    qint64* pos;
    QString pdfFileName;
    //临时保存盖章页面信息
    QVector<QImage>** imageList;
    QScrollArea* tempStorage;
    int x;
    int y;
    QString pdfFile;
    QString passwd;
    int pageNum;

    bool allowVerify = true;//是否允许验证
    bool isSubmit = true;//盖章信息是否提交
    SealPictrue* sp;//当前使用的印章
    QString username;//用户昵称
    QString department;//用户所属部门
    QString email;//用户邮箱
    QString power;//用户权限
    QByteArray infoAbstract;//文件信息摘要
    QVector<QVector<QByteArray>>* sealUpdate;//pdf中已存在印章操作更新
    QVector<QVector<QByteArray>>* certification;//印章的证书信息
    QByteArray sealID;
    QMap<QString,QByteArray>* sealedfile;//盖章后的文件信息
    bool verifyPasswd(QString passwd, QByteArray passwdHash);//验证密码

};

#endif // BUSINESSMODEL_H
