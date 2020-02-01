#ifndef UNSEALUI_H
#define UNSEALUI_H

#include <QDialog>
#include <QListWidget>
#include <QBrush>
#include <QMessageBox>
#include "include/utils/unseal.h"
#include "unsealpasswdui.h"
#include "include/utils/pdfwriter.h"

namespace Ui {
class UnsealUI;
}

class UnsealUI : public QDialog
{
    Q_OBJECT

public:
    explicit UnsealUI(QWidget *parent = nullptr);
    ~UnsealUI();
    void setPdfFileName(QString pdfFileName);
    void setSealUpdate(QVector<QVector<QByteArray>>* sealUpdate);
    void setCertification(QVector<QVector<QByteArray>>* certification);
    void setIsOK(bool* isOK);
    void setFileSize(qint64 size);
    void setPos(qint64 pos);
    void analyseSeal();
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    qint64 size;
    qint64 pos;
    QString pdfFileName;
    Ui::UnsealUI *ui;
    QVector<QVector<QByteArray>>* sealUpdate;//pdf中已存在印章操作更新
    QVector<QVector<QByteArray>>* certification;//pdf中的证书信息
    bool ok;//验证密码是否正确
    bool* isOK;
    QVector<QByteArray>* picture;
    QVector<QByteArray>* page;
    QVector<QByteArray>* sealId;
    QVector<QByteArray>* passwd;
    void extractPicture();//提取印章中的图片
    void extractSealID();//获取印章编号
    void extractSealPage();//获取盖章页面的页码
    void extractSealPasswd();//提取印章的密码
    void clearSeal(int id);//撤章
    void loadSealList();
};

#endif // UNSEALUI_H
