#include "include/controller/businessmodel.h"
BusinessModel::BusinessModel(QObject *parent)
{
    sealUpdate = new QVector<QVector<QByteArray>> ();
    certification = new QVector<QVector<QByteArray>> ();
    pos = new qint64;
    pdfSize = new qint64;
}

BusinessModel::~BusinessModel()
{

}

void BusinessModel::openFile(QString filePath, QVector<QImage>** imageList, QScrollArea* tempStorage, int numPages[], QListWidget* lWidget, double pictureRate[])
{
    pdfFileName = filePath;
    PdfDocument* pdfDocument = new PdfDocument(filePath, true);
    this->numPages = pdfDocument->openPDF(imageList, tempStorage, lWidget, pictureRate);
    sealUpdate = pdfDocument->getSealInfo();
    certification = pdfDocument->getCertification();
    *pos = pdfDocument->getPos();
    infoAbstract.append(pdfDocument->getInfoAbstract());

    *pdfSize = pdfDocument->getFileSize();
    numPages[0] = this->numPages;
    delete pdfDocument;
}

void BusinessModel::saveFile(QString fp)
{
    PdfDocument* pdfDocument = new PdfDocument();
    QStringList saveFp = fp.split(".");
    saveFp[0].append("(新保存).");
    pdfDocument->savePDF(fp, saveFp[0].append(saveFp[1]));
    delete pdfDocument;
}

void BusinessModel::saveAs(QString filePath, QString saveFp)
{
    PdfDocument* pdfDocument = new PdfDocument();
    pdfDocument->savePDF(filePath, saveFp);
    delete pdfDocument;
}

void BusinessModel::jumpPage(int pageNum, QVector<QImage>** imageList, QScrollArea* tempStorage, QString filePath)
{
    PdfDocument* pdfDocument = new PdfDocument(filePath);
    pdfDocument->jumpPage(pageNum, imageList, tempStorage);
    delete pdfDocument;
}

void BusinessModel::zoom(QVector<QImage>** imageList, int pageNum, double multipleRate, QScrollArea* tempStorage)
{
    PdfDocument* pdfDocument = new PdfDocument();
    pdfDocument->zoomPDF(imageList, tempStorage, pageNum, multipleRate);
    delete pdfDocument;
}

void BusinessModel::makeSeal()
{
    if(power != "普通用户")
    {
        QMessageBox::warning(nullptr,QObject::tr("错误"),QObject::tr("该用户没有权限进行该操作。"),QMessageBox::Ok);
        return;
    }
    MakeSealWindow* makeSeal = new MakeSealWindow(nullptr,email);
    makeSeal->exec();
}

void BusinessModel::seal(QVector<QImage>** imageList, QScrollArea* tempStorage,int x, int y, QString pdfFile, QString imageFile, QString passwd, int pageNum)
{
    sp = new SealPictrue();
    this->imageList = imageList;
    this->tempStorage = tempStorage;
    this->x = x;
    this->y = y;
    this->pdfFile = pdfFile;
    this->passwd = passwd;
    this->pageNum = pageNum;

    if(!sp->openSealPictrue(imageFile))
    {
        qDebug()<<"印章图片打开失败";
        return;
    }
    sealID = sp->getSealUuid();
    Network* net = new Network();
    connect(net,SIGNAL(finish(QJsonObject)),this,SLOT(sealoperate(QJsonObject)));
    net->sendRequest(QUrl("http://holer.cc:50356/vertify/verifySeal?sealId="+sealID));
}

void BusinessModel::sealoperate(QJsonObject json)
{
    if(!json.value("error").toString().isEmpty())
    {
        QMessageBox::warning(nullptr,QObject::tr("错误"),QObject::tr(json.value("error").toString().toUtf8()),QMessageBox::Abort);
        return;
    }
    PdfDocument* pdfDocument = new PdfDocument();
    if(verifyPasswd(passwd, sp->getPasswdHash()))
    {
        QMessageBox::warning(nullptr,QObject::tr("错误"),QObject::tr("您输入的印章密码错误。"),QMessageBox::Abort);
        return;
    }
    pdfDocument->setSealInfo(sealUpdate);
    pdfDocument->setCertification(certification);
    sealedfile = new QMap<QString,QByteArray>();
    pdfDocument->seal(imageList, tempStorage,x, y, sp, pdfFile, pageNum,sealedfile);
    isSubmit = false;
    delete pdfDocument;
    delete sp;
}

bool BusinessModel::verifyPasswd(QString passwd, QByteArray passwdHash)//输入的密码和印章的密码
{
    Calculator* calculator = new Calculator();
    QByteArray hash;
    hash = calculator->sha512(passwd.toStdString());
    hash += '\n';
    if(hash == passwdHash)
    {
        return false;
    }
    return true;
}

void BusinessModel::vertification()
{
    BatchValidation *batch = new BatchValidation(nullptr, certification, &infoAbstract, pdfFileName);//需要传入两个数据，第一个是签章信息，第二个是前十行的信息摘要
    batch->exec();
}

void BusinessModel::freeResource()
{
    try {
        sealUpdate->clear();
        certification->clear();
        infoAbstract.clear();
        allowVerify = true;//是否允许验证
        isSubmit = true;//盖章信息是否提交
        sealID.clear();
    } catch (QException e) {
        QMessageBox message;
        message.setWindowTitle("WRONG！");
        message.setText("To clear sealUpdate or certitfication is failing!");
        message.setDefaultButton(QMessageBox::Ok);
        message.exec();
    }
}

void BusinessModel::unseal(QVector<QImage>** imageList, QScrollArea* tempStorage, int pageNum)
{
    if(power != "普通用户")
    {
        QMessageBox::warning(nullptr,QObject::tr("错误"),QObject::tr("该用户没有权限进行该操作。"),QMessageBox::Abort);
        return;
    }
    Network* net = new Network();
    connect(net,SIGNAL(finish(QJsonObject)),this,SLOT(unsealOperate(QJsonObject)));
    net->sendRequest(QUrl("http://holer.cc:50356/vertify/isUnseal?sealedFileKey="+infoAbstract));
    this->imageList = imageList;
    this->tempStorage = tempStorage;
    this->pageNum = pageNum;
}

void BusinessModel::unsealOperate(QJsonObject json)
{
    if(json.value("message").toString() == "true")
    {
        bool* isOK = new bool;
        *isOK = false;
        UnsealUI* unseal = new UnsealUI();
        unseal->setPdfFileName(pdfFileName);
        unseal->setSealUpdate(sealUpdate);
        unseal->setCertification(certification);
        unseal->setFileSize(*pdfSize);
        unseal->setIsOK(isOK);
        unseal->setPos(*pos);
        unseal->analyseSeal();
        unseal->exec();
        if(isOK)
        {
            PdfDocument* document = new PdfDocument();
            document->unseal(imageList, tempStorage, pageNum, pdfFileName);
        }
    }
    else
    {
        QMessageBox::information(nullptr,"警告","该文件已盖章并已将数据提交到了服务器，不允许你撤章");
    }
}

void BusinessModel::softInfo()
{
    SoftInfoDialog* dialog = new SoftInfoDialog ();
    dialog->exec();
}

void BusinessModel::userManual()
{
    UserManualDialog* dialog = new UserManualDialog ();
    dialog->exec();
}

void BusinessModel::login()
{
    LoginDialog* login = new LoginDialog ();
    connect(login,SIGNAL(loginSuccess(QJsonObject)),this,SLOT(setUserInfo(QJsonObject)));
    login->exec();
}

void BusinessModel::setUserInfo(QJsonObject jsonObject)
{
    username = jsonObject.value("username").toString();
    department = jsonObject.value("department").toString();
    power = jsonObject.value("result").toString();
    email = jsonObject.value("email").toString();
    emit(userOperation(username));
}

void BusinessModel::changePassword()
{
    ChangePasswordDialog changePassword;
    changePassword.exec();
}

void BusinessModel::sendSealedFile()
{
    if(power != "普通用户")
    {
        QMessageBox::warning(nullptr,QObject::tr("错误"),QObject::tr("该用户没有权限进行该操作。"),QMessageBox::Ok);
        return;
    }
    if(sealID.isNull())
    {
        QMessageBox::warning(nullptr,QObject::tr("错误"),QObject::tr("您还没有盖章，不能进行提交"),QMessageBox::Ok);
        return;
    }

    Network* net = new Network(this);
    connect(net,SIGNAL(finish(QJsonObject)),this,SLOT(sendResult(QJsonObject)));
    net->sendRequest(QUrl("http://holer.cc:50356/submit/uploadSealedFile?email="+email+"&sealId="+sealID
                          +"&sealedFileKey="+sealedfile->value("key")
                          +"&sealedFileInfo="+sealedfile->value("info")));
}

void BusinessModel::sendResult(QJsonObject json)
{
    QMessageBox::information(nullptr,"盖章结果",json.value("message").toString());
    isSubmit = true;
}

bool BusinessModel::getSubminInfo(){
    return isSubmit;
}
