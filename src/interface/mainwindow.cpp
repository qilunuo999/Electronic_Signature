#include "include/interface/mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tempStorage = ui->scrollArea;
    lWidget = ui->listWidget;
    imageList = new QVector<QImage>*();
    (*imageList) = new  QVector<QImage>();
    busnessmodel = new BusinessModel();

    connect(busnessmodel,SIGNAL(userOperation(QString)),this,SLOT(displayUserName(QString)));

    this->ui->pushButton->setParent(ui->tabWidget);
    this->ui->pushButton->resize(50,38);
    this->ui->pushButton->move(0,0);
    QMenu *menu = new QMenu(this);
    ui->pushButton->setMenu(menu);

    QWidgetAction *ActionOpen = new QWidgetAction(this);
    ActionOpen->setDefaultWidget(new class MenuItemWidget(QPixmap(":/resource/图片/open.png"),QString::fromStdString("打开"),this));
    menu->addAction(ActionOpen);
    connect(ActionOpen,SIGNAL(triggered()),this,SLOT(on_Open_clicked()));

    QWidgetAction *ActionSave = new QWidgetAction(this);
    ActionSave->setDefaultWidget(new class MenuItemWidget(QPixmap(":/resource/图片/save 保存文档.png"),QString::fromStdString("保存"),this));
    menu->addAction(ActionSave);
    connect(ActionSave,SIGNAL(triggered()),this,SLOT(on_Save_clicked()));

    QWidgetAction *ActionSaveAs = new QWidgetAction(this);
    ActionSaveAs->setDefaultWidget(new class MenuItemWidget(QPixmap(":/resource/图片/另存为.png"),QString::fromStdString("另存为"),this));
    menu->addAction(ActionSaveAs);
    connect(ActionSaveAs,SIGNAL(triggered()),this,SLOT(on_SaveAs_clicked()));

    QWidgetAction *ActionLoginAs = new QWidgetAction(this);
    ActionLoginAs->setDefaultWidget(new class MenuItemWidget(QPixmap(":/resource/图片/user.png"),QString::fromStdString("登录"),this));
    menu->addAction(ActionLoginAs);
    connect(ActionLoginAs,SIGNAL(triggered()),this,SLOT(login()));

    QWidgetAction *ActionChangeAs = new QWidgetAction(this);
    ActionChangeAs->setDefaultWidget(new class MenuItemWidget(QPixmap(":/resource/图片/edit.png"),QString::fromStdString("修改密码"),this));
    menu->addAction(ActionChangeAs);
    connect(ActionChangeAs,SIGNAL(triggered()),this,SLOT(change_password()));

    menu->addSeparator();

    QWidgetAction *ActionClose = new QWidgetAction(this);
    ActionClose->setDefaultWidget(new class MenuItemWidget(QPixmap(":/resource/图片/关闭.png"),QString::fromStdString("关闭"),this));
    menu->addAction(ActionClose);
    connect(ActionClose,SIGNAL(triggered()),this,SLOT(on_Close_clicked()));

    QWidgetAction *ActionExit = new QWidgetAction(this);
    ActionExit->setDefaultWidget(new class MenuItemWidget(QPixmap(":/resource/图片/退出.png"),QString::fromStdString("退出"),this));
    menu->addAction(ActionExit);
    connect(ActionExit, SIGNAL(triggered()), this, SLOT(close()));

    QFile file_(":/Menu.qss");
    file_.open(QFile::ReadOnly);
    menu->setStyleSheet(file_.readAll());
    ui->pushButton->setMenu(menu);
    QFile Ffile(":/FileButton.qss");
    Ffile.open(QFile::ReadOnly);
    ui->pushButton->setStyleSheet(Ffile.readAll());

    QStatusBar *info = ui->statusBar;
    PageNum = new QLabel;
    username = new QLabel;
    QString pagenum = "第n页/共n页";
    PageNum->setMinimumSize(200,20); //设置标签最小尺寸
    PageNum->setFrameShape(QFrame::VLine); //设置标签形状
    PageNum->setFrameShadow(QFrame::Sunken); //设置标签阴影
    username->setMinimumSize(200,20);
    username->setFrameShape(QFrame::VLine);
    username->setFrameShadow(QFrame::Sunken);
    info->addWidget(PageNum);
    info->addWidget(username);
    info->setSizeGripEnabled(false);
    PageNum->setText(pagenum);
    username->setText("昵称：");
    info->setStyleSheet("QStatusBar::item{border:0px}");

    login();
    b_hideme = true;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete *imageList;
    delete  imageList;
}

void MainWindow::closeEvent(QCloseEvent *event){
    if (!busnessmodel->getSubminInfo())
    {
        QMessageBox::StandardButton result = QMessageBox::information(this, tr("提示"), tr("您的盖章信息还未提交，请问您要继续退出吗？"),QMessageBox::Yes|QMessageBox::No);
        switch (result) {
        case QMessageBox::Yes:
            qDebug()<<"Yes";
            break;
        case QMessageBox::No:
            //不退出
            event->ignore();
            break;
        default:
            break;
        }
    }
}

void MainWindow::calculateCoordinates(QPoint point, QVector<int>* actualXY)
{
    //提取scrollArea中的label并获取其高度和宽度
    QList<QLabel*> labelList = tempStorage->findChildren<QLabel*>();
    QLabel* label = labelList.at(0);
    int labelY = label->height();//高是纵坐标
    int labelX = label->width();//宽是横坐标
    //获取鼠标相对于label的坐标
    QPoint point2 = label->mapFromGlobal(point);
    point.setX(point2.x() - labelX/2 + (*imageList)->at(0).width()/2);
    point.setY(labelY - point2.y());
    qDebug()<<point<<point.x()<<point.y();
    //计算当前坐标对应的pdf文档中的实际坐标
    actualXY->replace(1,static_cast<int>(point.y()/ pictureRate[0]));
    actualXY->replace(0,static_cast<int>(point.x()/ pictureRate[0]));
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    QPoint point;
    QVector<int>* actualXY = new QVector<int>();
    actualXY->resize(2);
    if(!ui->Stamp->isChecked() || (*imageList)->count() == 0)
    {
        return;
    }
    if(event->button() == Qt::LeftButton)
    {
        if(fp == nullptr)
        {
            QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
            return;
        }
        point = event->globalPos();
        calculateCoordinates(point, actualXY);
        ElectronicSeal* electronicSeal = new ElectronicSeal(nullptr, busnessmodel);
        electronicSeal->setInfo(fp, imageList, tempStorage, actualXY->at(0), actualXY->at(1), pageNum);
        electronicSeal->exec();
        ui->Stamp->setChecked(false);
        this->setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::on_toolButton_triggered()//隐藏目录按键实现
{
    b_hideme = true;
    connect(ui->Hide,SIGNAL(clicked()),ui->listWidget,SLOT(hide(listWidget)));//alignas (on_btn_hide_clicked())
}

void MainWindow::on_Hide_clicked()//隐藏目录
{
    if(b_hideme == true){
        ui->listWidget->hide();
        b_hideme = false;
    }
    else{
        ui->listWidget->show();
        b_hideme = true;
    }
}

void MainWindow::on_Open_clicked()//打开文件
{
    (*imageList)->clear();
    busnessmodel->freeResource();
    lWidget->clear();
    fp = QFileDialog::getOpenFileName(this, tr("打开文件"), " ",  tr("PdfFile(*.pdf)"));
    if(fp.isEmpty())
    {
        return;
    }
    busnessmodel->openFile(fp, imageList,tempStorage, numPages, lWidget, pictureRate);
    pageNum = 0;
    PageNum->setText("第" + QString::number(pageNum+1).append("页/共"+QString::number(numPages[0]))+"页");
    isSave = false;
}

void MainWindow::on_Save_clicked()//保存文件
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    busnessmodel->saveFile(fp);
    isSave = true;
}

void MainWindow::on_SaveAs_clicked()//文件另存为
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,tr("另存为")," ",tr("PdfFile(*.pdf)"));
    if(fileName == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("文件名不能为空！"),QMessageBox::Abort);
        return;
    }
    busnessmodel->saveAs(fp, fileName);
    isSave = true;
}

void MainWindow::on_Close_clicked()//关闭
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    (*imageList)->clear();
    busnessmodel->freeResource();
    lWidget->clear();
    QWidget* widget = tempStorage->takeWidget();
    if(widget != nullptr)
    {
        delete widget;
    }
}

void MainWindow::on_Suitable_clicked()//适合页面
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    multipleRate = 1;
    busnessmodel->zoom(imageList, pageNum, multipleRate, tempStorage);
}

void MainWindow::on_Full_Screen_clicked()//全屏
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    multipleRate = 1.9;
    busnessmodel->zoom(imageList, pageNum, multipleRate, tempStorage);
}

void MainWindow::on_Amplification_clicked()//放大
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    this->multipleRate += 0.1;
    busnessmodel->zoom(imageList, pageNum, multipleRate, tempStorage);
}

void MainWindow::on_Zoom_out_clicked()//缩小
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    this->multipleRate -= 0.1;
    busnessmodel->zoom(imageList, pageNum, multipleRate, tempStorage);
}

void MainWindow::on_User_manual_clicked()//用户手册
{
    busnessmodel->userManual();
}

void MainWindow::on_Soft_info_clicked()//软件信息
{
    busnessmodel->softInfo();
}

void MainWindow::on_Previous_page_clicked()//上一页
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    if(pageNum > 0)
    {
        pageNum--;
        busnessmodel->jumpPage(pageNum, imageList,tempStorage, this->fp);
        PageNum->setText("第" + QString::number(pageNum+1).append("页/共"+QString::number(numPages[0]))+"页");
    }
}

void MainWindow::on_Next_page_clicked()//下一页
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    if(pageNum+1 < numPages[0])
    {
        pageNum++;
        busnessmodel->jumpPage(pageNum, imageList,tempStorage,  this->fp);
        PageNum->setText("第" + QString::number(pageNum+1).append("页/共"+QString::number(numPages[0]))+"页");
    }
}

void MainWindow::on_First_page_clicked()//第一页
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    pageNum = 0;
    busnessmodel->jumpPage(pageNum, imageList,tempStorage, fp);
    pageNum = 1;
    PageNum->setText("第" + QString::number(pageNum).append("页/共"+QString::number(numPages[0]))+"页");
}

void MainWindow::on_Last_page_clicked()//最后一页
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    pageNum = numPages[0] - 1;
    busnessmodel->jumpPage(pageNum, imageList,tempStorage, fp);
    PageNum->setText("第" + QString::number(pageNum+1).append("页/共"+QString::number(numPages[0]))+"页");
}

void MainWindow::on_Jump_page_clicked()//跳转页面
{
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    PageJump pageJump(imageList, tempStorage, PageNum, numPages, nullptr, this->fp);
    pageJump.exec();
}

void MainWindow::on_Create_seal_clicked()//制作电子印章
{
    if(username->text() == "昵称：")
    {
        QMessageBox::warning(this,QObject::tr("警告")
                             ,QObject::tr("您还没有登录。请先进行登录再进行后续操作")
                             ,QMessageBox::Abort);
        return;
    }
    busnessmodel->makeSeal();
}

void MainWindow::on_Verification_seal_clicked()//验证电子印章
{
    if(username->text() == "昵称：")
    {
        QMessageBox::warning(this,QObject::tr("警告")
                             ,QObject::tr("您还没有登录。请先进行登录再进行后续操作")
                             ,QMessageBox::Abort);
        return;
    }
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    busnessmodel->vertification();
}

void MainWindow::on_Cancel_seal_clicked()//撤销电子印章
{
    if(username->text() == "昵称：")
    {
        QMessageBox::warning(this,QObject::tr("警告")
                             ,QObject::tr("您还没有登录。请先进行登录再进行后续操作")
                             ,QMessageBox::Abort);
        return;
    }
    if(fp == nullptr)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("还未有打开的文件！"),QMessageBox::Abort);
        return;
    }
    busnessmodel->unseal(imageList, tempStorage, pageNum);
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QString pageNum;
    pageNum = item->text();
    this->pageNum = pageNum.toInt() - 1;
    busnessmodel->jumpPage(pageNum.toInt()-1, imageList,tempStorage, this->fp);
    PageNum->setText("第" + pageNum.append("页/共"+QString::number(numPages[0]))+"页");
}

void MainWindow::displayUserName(QString name){
    username->setText("昵称：" + name);
}

void MainWindow::on_Stamp_pressed()
{
    if(username->text() == "昵称：")
    {
        QMessageBox::warning(this,QObject::tr("警告")
                             ,QObject::tr("您还没有登录。请先进行登录再进行后续操作")
                             ,QMessageBox::Abort);
        return;
    }
    if(ui->Stamp->isCheckable() == false)
    {
        ui->Stamp->setCheckable(true);
        this->setCursor(Qt::CrossCursor);
    }
    else
    {
        ui->Stamp->setCheckable(false);
        this->setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::login()
{
    busnessmodel->login();
}

void MainWindow::change_password()
{
    if(username->text() == "昵称：")
    {
        QMessageBox::warning(this,QObject::tr("警告")
                             ,QObject::tr("您还没有登录。请先进行登录再进行后续操作")
                             ,QMessageBox::Abort);
        return;
    }
    busnessmodel->changePassword();
}

void MainWindow::on_submit_clicked()
{
    if(username->text() == "昵称：")
    {
        QMessageBox::warning(this,QObject::tr("警告")
                             ,QObject::tr("您还没有登录。请先进行登录再进行后续操作")
                             ,QMessageBox::Abort);
        return;
    }
    busnessmodel->sendSealedFile();
}
