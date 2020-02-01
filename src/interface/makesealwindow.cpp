#include "include/interface/makesealwindow.h"
#include "ui_makesealdialog.h"

using namespace std;

MakeSealWindow::MakeSealWindow(QWidget *parent,QString usr) :
    QDialog(parent),
    ui(new Ui::MakeSealWindow)
{
    ui->setupUi(this);
    user = usr;
}

MakeSealWindow::~MakeSealWindow()
{
    delete ui;
}

void MakeSealWindow::on_pushButton_clicked()
{
    QString company = ui->company->text();//所属单位
    QString name = ui->name->text();//印章名称
    sealName = name.toUtf8();
    QString pwd = ui->pwd->text();//印章密码
    QString endtime = ui->endtime->text();//印章失效时间
    QString stardate = QDateTime::currentDateTime().toString(Qt::ISODate);
    QDateTime tempTime1 = QDateTime::fromString(stardate, "yyyy-MM-ddThh:mm:ss");
    QDateTime tempTime2 = QDateTime::fromString(endtime, "yyyy-MM-dd hh:mm:ss");
    uint sTime = tempTime1.toTime_t();
    uint eTime = tempTime2.toTime_t();
    uint ndaysec = 24*60*60;
    QString effectiveTime = QString::number((eTime - sTime)/ndaysec, 10);

    if(company.isEmpty())
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("签发人/单位不能为空！"),QMessageBox::Abort);
        return;
    }
    if(name.isEmpty())
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("签章用户不能为空！"),QMessageBox::Abort);
        return;
    }
    if(pwd.isEmpty())
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("密码不能为空！"),QMessageBox::Abort);
        return;
    }
    if(tempTime1 > tempTime2)
    {
        QMessageBox::warning(this,QObject::tr("警告"),QObject::tr("截止日期不能在当前日期之前！\n例如：今天是2018年8月18日，截止日期不能设置为2018年8月17日以及更早的时间"),QMessageBox::Abort);
        return;
    }

    std::string str_company = company.toStdString();//公司名
    std::string str_user = user.toStdString();//印章制作人
    std::string str_name = name.toStdString();
    std::string str_pwd = pwd.toStdString();
    std::string str_endtime = endtime.toStdString();
    std::string str_stardate = stardate.toStdString();
    std::string encryptText;//RSA私钥加密后的字符串
    std::string key[2];//密钥对

    QDateTime time = QDateTime::currentDateTime();   //获取当前时间
    unsigned int startime_stmp = time.toTime_t();   //将当前时间转为时间戳
    QString startime_stmp2 = QString::number(startime_stmp);//转换成字符

    //基本信息组合，进行RSA加密
    QString information = company + "/" + user + "/" + name + "/" +stardate + "/" + endtime ;
    std::string str_information = information.toStdString();
    //生成印章编号
    QUuid uuid = QUuid::createUuid();
    sealId = uuid.toByteArray();
    sealId.remove(0,1);
    sealId.remove(sealId.size()-1,1);
    /*
     *构建印章生成路径
     */
    file_fullpath2 = file_fullpath;
    file_fullpath2 = sealDialog+"/"+name+"."+file_suffix;
    //填入印章数据
    QFile::copy(file_fullpath, file_fullpath2);
    ofstream outFile;
    outFile.open(file_fullpath2.toStdString(),ios::app);
    outFile << endl<< "__Delimiter__" << endl;
    outFile << startime_stmp << endl;   //签发时间戳
    outFile << str_stardate << endl;     //签发时间
    outFile << str_endtime << endl;      //印章有效期
    outFile << "__Delimiter__" << endl;
    outFile << keyInfo.toStdString();        //公钥
    outFile << "__Delimiter__" << endl;
    outFile << sha512(str_pwd) << endl; //用户密码hash
    outFile << "__Delimiter__" << endl;
    outFile << rsa_pri_encrypt(str_information.c_str()) << endl;//使用私钥对基本信息RSA加密（签名）
    outFile << "__Delimiter__" << endl;
    //写入证书
    outFile << cerInfo.toStdString();
    outFile << "__Delimiter__" << endl;
    outFile << sealId.toStdString();
    outFile.close();

    upload();
}

/*
 * 上传印章信息
 */
void MakeSealWindow::upload()
{
    if(!isError){
        Network* net = new Network();
        connect(net,SIGNAL(finish(QJsonObject)),this,SLOT(getResultInfo(QJsonObject)));
        net->sendRequest(QUrl("http://holer.cc:50356/submit/uploadSeal?sealId="+sealId+"&sealName="
                              +sealName+"&sealInfo="+sealinfo+"&certificateName="+certificateName
                              +"&sealType="+ui->sealType->currentText()+"&email="+user));
    }else
    {
        QMessageBox::information(this,"警告","由于服务器端验证不通过，因此不允许制作该印章。");
    }
}

/*
 * 接收印章制作结果
 */
void MakeSealWindow::getResultInfo(QJsonObject json)
{
    if(json.value("error").toString().isEmpty())
    {
        ui->textBrowser->setText("印章制作成功。");
    }else
    {
        ui->textBrowser->setText(json.value("error").toString());
    }
}

/*
 * 获取印章图片路径、文件名和后缀
 */
void MakeSealWindow::on_pushButton_2_clicked()
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("打开图片"));
    //设置默认文件路径
    fileDialog->setNameFilter(tr("Images(*.jpg *.jpeg)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);

    //获取所选择的文件的路径
    file_fullpath = QFileDialog::getOpenFileName(this,tr(""),nullptr,tr(""));
    ui->pictrue->setText(file_fullpath);
    info = QFileInfo(file_fullpath);

    file_name = info.fileName();
    file_suffix = info.suffix();
}

/*
 * 获取证书文件路径、内容及其公钥
 */
void MakeSealWindow::on_getCertificateButton_clicked()
{
    certificate = QFileDialog::getOpenFileName(this,tr(""),nullptr,tr(""));
    ui->certificate->setText(certificate);
    QFileInfo fileInfo = QFileInfo(certificate);
    certificateName = fileInfo.fileName();
    extractCertificate();
    verifyCertificate();
}

void MakeSealWindow::on_pushButton_3_clicked()
{
    sealDialog = QFileDialog::getExistingDirectory(nullptr, "印章生成位置", ".");
    ui->sealDialog->setText(sealDialog);
}

/*
 *获取印章私钥路径
 */
void MakeSealWindow::on_keyButton_clicked()
{
    certificateKey =  QFileDialog::getOpenFileName(this,tr(""),nullptr,tr(""));
    ui->certificateKey->setText(certificateKey);
    extractPriKey();
    extractPubKey();
}

/*
 * 重载openssl证书校验反馈函数
 */
int verify_callback(int ok, X509_STORE_CTX *ctx)
{
    return ok;
}

/*
 *获取证书数据
 */
void MakeSealWindow::extractCertificate(){
    QFile* cer = new QFile(certificate);
    qDebug()<<certificate;
    if(!cer->open(QIODevice::ReadOnly)){
        qDebug()<<"读取证书文件失败";
        return;
    }
    cerInfo.clear();
    while(!cer->atEnd()){
        cerInfo.append(cer->readLine());//逐行读取文件
    }
    cer->close();
    qDebug()<<cerInfo;
    //通过openssl解读证书
    OpenSSL_add_all_algorithms();
    BIO *bio_mem = new BIO;
    bio_mem = BIO_new(BIO_s_mem());
    //根据读到的数据生成X509对象
    BIO_puts(bio_mem, cerInfo.data());
    X509 *x509 = new X509;
    x509 = PEM_read_bio_X509(bio_mem, nullptr, nullptr, nullptr);
    X509_STORE* ca_store = X509_STORE_new();
    X509_STORE_CTX* ctx = X509_STORE_CTX_new();

    //设置证书验证结果的回调
    X509_STORE_set_verify_cb(ca_store,verify_callback);
    int ret = X509_STORE_add_cert(ca_store, x509);
    if (1 != ret)
    {
        qDebug()<<"[ssl_verify] X509_STORE_add_cert ret failed";
        return;
    }

    STACK_OF(X509) *ca_stack = nullptr;
    ret = X509_STORE_CTX_init(ctx, ca_store, x509, ca_stack);
    if (ret != 1)
    {
        qDebug()<<"[ssl_verify] X509_STORE_CTX_init ret failed";
        return;;
    }

    ret = X509_verify_cert(ctx);//获取验证结果
    if(ret == 0)
    {
        sealinfo.append("制作印章所使用证书未能通过验证");
        ui->textBrowser->setText("证书未能通过openssl验证，请确认您使用的证书是否合法有效,如果你坚持使用，这一信息将被记录在印章信息中。");
    }
    else
    {
        sealinfo.append("客户端openssl审核通过。");
        ui->textBrowser->setText("证书成功通过客户端openssl验证。");
    }
    //获取证书序列号
    ASN1_INTEGER* serialNumber = X509_get_serialNumber(x509);
    BIGNUM *bignum = nullptr;
    char *serial = nullptr;
    bignum = ASN1_INTEGER_to_BN(serialNumber, nullptr);//将证书序列号转换成openssl的大数对象
    if(bignum != nullptr)
    {
        serial = BN_bn2hex(bignum);//将大数转成字符串
        if(serial != nullptr)
        {
            BN_free(bignum);
        }
    }
    cerSerialNumber.append(serial);
}

/*
 * 校验证书内容
 */
void MakeSealWindow::verifyCertificate(){
    Network* net = new Network(this);
    connect(net,SIGNAL(finish(QJsonObject)),this,SLOT(getVerifyInfoByNetwork(QJsonObject)));
    net->sendRequest(QUrl("http://holer.cc:50356/vertify/verifyCertificate?fileName="+certificateName+"&certificate="+cerInfo));
}

/*
 * 收到服务器传回的信息后保存和显示
 */

void MakeSealWindow::getVerifyInfoByNetwork(QJsonObject json)
{
    if(!json.value("error").toString().isEmpty())
    {
        QString temp = ui->textBrowser->toPlainText();
        temp.append("服务器端验证不通过不能制作，请先到网站提交证书，认证通过后再制作印章");
        ui->textBrowser->setText(temp);
        isError = true;
        return;
    }
    sealinfo.append(","+json.value("message").toString());
    QString temp = ui->textBrowser->toPlainText();
    temp.append("\n"+json.value("message").toString());
    ui->textBrowser->setText(temp);
    isError = false;
}

/*
 * 获取私钥数据
 */
void MakeSealWindow::extractPriKey()
{
    QFile* prikey = new QFile(certificateKey);
    if(!prikey->open(QIODevice::ReadOnly)){
        qDebug()<<"读取证书文件失败";
        return;
    }
    while(!prikey->atEnd()){
        prikeyInfo.append(prikey->readLine());
    }
    prikey->close();
}

/*
 * 从私钥中提取公钥
 */
void MakeSealWindow::extractPubKey(){
    QProcess* process = new QProcess();
    QStringList list;
    list.append("rsa");
    list.append("-in");
    list.append(certificateKey);
    list.append("-out");
    list.append("tempPubKey.key");
    list.append("-RSAPublicKey_out");
    process->start("openssl",list);

    process->waitForFinished();
    ui->textBrowser->setText(process->readAllStandardError());
    process->close();
    QFile* key = new QFile("tempPubKey.key");
    if(!key->open(QIODevice::ReadOnly)){
        qDebug()<<"读取公钥文件失败";
        return;
    }
    while(!key->atEnd())
    {
        keyInfo.append(key->readLine());
    }
    key->close();
    qDebug()<<keyInfo;
    QFile::remove("tempPubKey.key");
}

// 私钥加密
std::string MakeSealWindow::rsa_pri_encrypt(const std::string &clearText)
{
    std::string strRet;
    RSA *rsa = nullptr;
    BIO *keybio = BIO_new_mem_buf(prikeyInfo.data(), -1);

    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, nullptr, nullptr);

    size_t len = static_cast<size_t>(RSA_size(rsa));
    char *encryptedText = static_cast<char *>(malloc(len + 1));
    memset(encryptedText, 0, len + 1);

    // 加密函数
    unsigned long ret = static_cast<unsigned long>(RSA_private_encrypt(static_cast<int>(clearText.length()), reinterpret_cast<const unsigned char *>(clearText.c_str()), reinterpret_cast<unsigned char*>(encryptedText), rsa, RSA_PKCS1_PADDING));
    if (ret > 0)
        strRet = std::string(encryptedText, ret);
    strRet = Encode(reinterpret_cast<const unsigned char *>(strRet.c_str()),static_cast<int>(strRet.length()));

    // 释放内存
    free(encryptedText);
    BIO_free_all(keybio);
    RSA_free(rsa);

    return strRet;
}

// 公钥解密
std::string rsa_pub_decrypt(const std::string &cipherText, const std::string &pubKey)
{
    std::string strRet;
    RSA *rsa = RSA_new();
    BIO *keybio;
    keybio = BIO_new_mem_buf(pubKey.c_str(), -1);

    rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, nullptr, nullptr);

    size_t len = static_cast<size_t>(RSA_size(rsa));
    char *decryptedText = static_cast<char*>(malloc(len + 1));
    memset(decryptedText, 0, len + 1);

    // 解密函数
    unsigned long ret = static_cast<unsigned long>(RSA_public_decrypt(static_cast<int>(cipherText.length()), reinterpret_cast<const unsigned char *>(cipherText.c_str()), reinterpret_cast<unsigned char*>(decryptedText), rsa, RSA_PKCS1_PADDING));
    if (ret > 0)
        strRet = std::string(decryptedText, ret);

    // 释放内存
    free(decryptedText);
    BIO_free_all(keybio);
    RSA_free(rsa);

    return strRet;
}



//===============================================================================
std::string MakeSealWindow::sha512(const std::string str)//生成用户密码hash值
{
    char buf[5];
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    SHA512_Update(&sha512, str.c_str(), str.size());
    SHA512_Final(hash, &sha512);
    std::string newString = "";
    for(int i = 0; i < SHA512_DIGEST_LENGTH; i++)
    {
        sprintf(buf,"%02x",hash[i]);
        newString = newString + buf;
    }

    return newString;
}

//base64加密
std::string MakeSealWindow::Encode(const unsigned char* Data,int DataByte)
{
    //编码表
    const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    //返回值
    string strEncode;
    unsigned char Tmp[4]={0};
    int LineLength=0;
    for(int i=0;i<static_cast<int>(DataByte / 3);i++)
    {
        Tmp[1] = *Data++;
        Tmp[2] = *Data++;
        Tmp[3] = *Data++;
        strEncode+= EncodeTable[Tmp[1] >> 2];
        strEncode+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
        strEncode+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
        strEncode+= EncodeTable[Tmp[3] & 0x3F];
        if(static_cast<void>(LineLength+=4),LineLength==76) {strEncode+="\r\n";LineLength=0;}
    }
    //对剩余数据进行编码
    int Mod=DataByte % 3;
    if(Mod==1)
    {
        Tmp[1] = *Data++;
        strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
        strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4)];
        strEncode+= "==";
    }
    else if(Mod==2)
    {
        Tmp[1] = *Data++;
        Tmp[2] = *Data++;
        strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
        strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
        strEncode+= EncodeTable[((Tmp[2] & 0x0F) << 2)];
        strEncode+= "=";
    }

    return strEncode;
}
