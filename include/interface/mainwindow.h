#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QFile"
#include "QMenu"
#include "QWidgetAction"
#include "QLabel"
#include "QHBoxLayout"
#include "menuitemwidget.h"
#include "QToolButton"
#include "QStatusBar"
#include <QString>
#include <QFileDialog>
#include <QStandardPaths>
#include <QScrollArea>
#include <QListWidget>
#include <QMouseEvent>
#include <QCloseEvent>

#include "pagejump.h"
#include "search.h"
#include"include/controller/businessmodel.h"
#include "electronicseal.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void MenuItemWidget(const QPixmap& icon, const QString& text, QWidget *parent = nullptr);
    bool b_hideme;
    ~MainWindow();
    //一下三项用于临时存储转换得到的pdf图片,pdf文档信息以及用于显示文档的widget
    QVector<QImage>** imageList;
    QScrollArea* tempStorage;
    //放大缩小的倍率
    double multipleRate = 1.0;
    //实际pdf大小与图片大小的比例
    double pictureRate[1] = {0};
    //用于判断当前文档是否被保存
    bool isSave = true;
    //用于临时存储pdf文件地址
    QString fp;
    int numPages[1];
    int pageNum;
    //缩略图
    QListWidget* lWidget;
    //pdf坐标
    QRectF* rect;
    void MenuItemWidget(const QPixmap &icon, const QString &text);

protected:
    //鼠标点击事件
    void mousePressEvent(QMouseEvent* event);
    //关闭事件
    void closeEvent(QCloseEvent *event);

private slots:
    void on_toolButton_triggered();

    void on_Open_clicked();

    void on_Save_clicked();

    void on_SaveAs_clicked();

    void on_Close_clicked();

    void on_Hide_clicked();

    void on_Suitable_clicked();

    void on_Full_Screen_clicked();

    void on_Amplification_clicked();

    void on_Zoom_out_clicked();

    void on_User_manual_clicked();

    void on_Soft_info_clicked();

    void on_Previous_page_clicked();

    void on_Next_page_clicked();

    void on_First_page_clicked();

    void on_Last_page_clicked();

    void on_Jump_page_clicked();

    void on_Create_seal_clicked();

    void on_Verification_seal_clicked();

    void on_Cancel_seal_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_Stamp_pressed();

    void login();

    void change_password();

    void displayUserName(QString name);

    void on_submit_clicked();

private:
    Ui::MainWindow *ui;
    QLabel *PageNum;
    QLabel *username;
    QLabel *Proportion;
    //控制中心
    BusinessModel *busnessmodel;
    void calculateCoordinates(QPoint point, QVector<int>* actualXY);//计算实际坐标

};

#endif // MAINWINDOW_H
