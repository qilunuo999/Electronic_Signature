#include "include/interface/batchvalidation.h"
#include"ui_batchvalidation.h"
#include "QtDebug"
#include "QtGui"

using namespace std;

BatchValidation::BatchValidation(QWidget *parent, QVector<QVector<QByteArray>>*info,QByteArray *digest, QString pdfFile):
    QDialog (parent),
    ui(new Ui::BatchValidation)
{

    ui->setupUi(this);
    this->info = info;
    this->digest = digest;
    this->pdfFile = pdfFile;
    qDebug()<<pdfFile;
    int n,k = 0,//n为印章数，k为当前读取到的印章序号
            page = 0,
            unsuc = 0,unverfi = 0;//无效印章
    int publickey;//公钥是否读取
    n = this->info->size();//取出电子印章的个数
    QStringList item;
    for(int i = 0;i<n;i++)//在widget中画出相应的电子印章
    {

        item<<"电子印章"+QString::number(i+1);
    }
    ui->listWidget->addItems(item);
    ui->sealNumber->setText(QString().number(n));
    QVector<QByteArray> verifi;
    std::string aa;
    QByteArray verti,pubkey,ciphertext,Info_digest;//分别为单个印章信息，公钥，加密后的密文，证书,信息摘要
    for(int i = 0;i < n;i++,page++)  //读出电子签章内的印章信息，公钥和加密后的密文
    {
        QByteArray temp = "";//公钥结尾标记
        bool ok = false;//判断是否读完公钥
        verifi = this->info->at(i);
        publickey = 0;
        k = 0;
        for(int j = 0;j<verifi.size();j++)
        {
            verti = verifi.at(j);
            for(int t = 0;t<verti.size();t++)
            {
                if(verti.at(t) == '\n'){
                    k++;
                }
                if(k >=5 && publickey == 0){//读取公钥，当读到\t时跳过本次循环继续往下读取，否则就继续往下读取内容
                    if (verti.at(t) == '@') {
                        publickey = 1;
                        t++;
                        continue;
                    } else {
                        pubkey += verti.at(t);
                    }
                }
                else if(publickey == 1){
                    if (verti.at(t) == '@') {
                        publickey = 2;
                    }else {
                        ciphertext += verti.at(t);
                    }
                }
                else if(k == 4){
                    Info_digest += verti.at(t);
                }
            }
        }
        ciphertext.insert(0,'\n');
        ciphertext.remove(ciphertext.size()-1,1);

        if(page == n-1){
            publicKey.append(pubkey);
            qDebug()<<pubkey;
        }

        aa = Decode((const char *)ciphertext.data(),ciphertext.length(),i);//base64解码
        aa = rsa_pub_decrypt(aa,pubkey.data());   //rsa公钥解密
        pubkey.clear();
        ciphertext.clear();
        if(this->digest == Info_digest){   //无法识别的印章，即信息摘要无法验证错误的印章
            unverfi++;
        }
        Info_digest.clear();
        if(aa.empty()) unsuc++;
    }
    Calculator* calculator = new Calculator();
    QByteArray fileHash = calculator->countFileHash(pdfFile).toLocal8Bit();
    Network* net = new Network();
    connect(net,SIGNAL(finish(QJsonObject)),this,SLOT(getServerInfo(QJsonObject)));
    net->sendRequest(QUrl("http://holer.cc:50356/vertify/verifySealedFile?sealedFileKey="+*this->digest+"&sealedFileInfo="+fileHash));
    ui->validSeal->setText(QString().number(n-unsuc-unverfi));
    ui->invalidSeal->setText(QString().number(unsuc));
    ui->indistingguishableness->setText(QString().number(unverfi));
    delete calculator;
}

BatchValidation::~BatchValidation()
{
    delete ui;
}

void BatchValidation::on_detailsButton_clicked()//详细信息
{

}

void BatchValidation::on_lookCertificateButton_clicked()//查看证书
{
    QVector<QVector<QByteArray>> *test = new QVector<QVector<QByteArray>>;
    QVector<QByteArray> data,mid;
    QByteArray cer,verti,excessive;
    int n,k = 0;
    int pubkey;
    n = this->info->size();
    for(int i = 0;i < n;i++){
        mid = this->info->at(i);
        k = 0;
        pubkey = 0;
        for(int j = 0;j < mid.size(); j++){
            verti = mid.at(j);
            for(int t = 0;t < verti.size(); t++)
            {
                //                if(verti.at(t) == '\n'){
                //                    k++;
                //                }
                if(verti.at(t) == '@'){
                    pubkey++;
                    continue;
                }
                if(pubkey == 2){//k >= 18 && k <= 37
                    excessive += verti.at(t);
                    qDebug()<<verti.at(t);
                }
            }
        }
        qDebug()<<excessive;
        data.append(excessive);
        test->append(data);
        excessive.clear();
        data.clear();
    }
    display_info display(nullptr,test);
    display.exec();
}

void BatchValidation::on_lookTimestampButton_clicked() //查看时间戳证书
{
    QVector<QVector<QByteArray>> *test = new QVector<QVector<QByteArray>>;
    QVector<QByteArray> data,mid;
    QByteArray cer,verti,excessive;
    int n,k = 0;
    n = this->info->size();
    for(int i = 0;i < n;i++){
        mid = this->info->at(i);
        k = 0;
        for(int j = 0;j < mid.size(); j++){
            verti = mid.at(j);
            for(int t = 0;t < verti.size(); t++)
            {
                if(verti.at(t) == '\n'){
                    k++;
                }
                if(k == 0){
                    excessive += verti.at(t);
                }
            }
        }
        data.append(excessive);
        test->append(data);
        excessive.clear();
        data.clear();
    }
    display_info display(nullptr,test);
    display.exec();
}

void BatchValidation::on_closeButton_4_clicked()
{
    this->close();
}

std::string BatchValidation::rsa_pub_decrypt(std::string cipherText,std::string pubKey)//rsa公钥解密,使用时传入密文和公钥信息即可
{
    RSA *rsa;
    int ret;
    char *message;
    std::string strRet;
    BIO *keybio;
    keybio = BIO_new_mem_buf((unsigned char *)pubKey.c_str(), -1);
    rsa = RSA_new();
    rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, nullptr, nullptr);
    size_t flen = (size_t)RSA_size(rsa);
    message = (char *)malloc(flen+1);
    memset(message, 0, flen+1);
    ret = RSA_public_decrypt(cipherText.length(),(const unsigned char *)cipherText.c_str(),(unsigned char *)message,rsa,RSA_PKCS1_PADDING);
    if(ret < 0){
        RSA_free(rsa);
        free(message);
        return nullptr;
    }
    strRet = std::string(message,ret);
    RSA_free(rsa);
    free(message);
    return strRet;
}

std::string BatchValidation::Decode(const char* Data,int DataByte,int& OutByte)
{
    //解码表
    const char DecodeTable[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        62, // '+'
        0, 0, 0,
        63, // '/'
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
        0, 0, 0, 0, 0, 0,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
    };
    //返回值
    string strDecode;
    int nValue;
    int i= 0;
    while (i < DataByte)
    {
        if (*Data != '\r' && *Data!='\n')
        {
            nValue = DecodeTable[*Data++] << 18;
            nValue += DecodeTable[*Data++] << 12;
            strDecode+=(nValue & 0x00FF0000) >> 16;
            //OutByte++;
            if (*Data != '=')
            {
                nValue += DecodeTable[*Data++] << 6;
                strDecode+=(nValue & 0x0000FF00) >> 8;
                //OutByte++;
                if (*Data != '=')
                {
                    nValue += DecodeTable[*Data++];
                    strDecode+=nValue & 0x000000FF;
                    //OutByte++;
                }
            }
            i += 4;
        }
        else// 回车换行,跳过
        {
            Data++;
            i++;
        }
    }
    return strDecode;
}




void BatchValidation::on_listWidget_itemClicked(QListWidgetItem *item)
{
    //this->info->append();
    QVector<QVector<QByteArray>> *test = new QVector<QVector<QByteArray>>;
    QVector<QByteArray> test1,test2;
    test1.append("1 0\t1540386340\t2018-10-24T21:05:40\t2046-01-01 00:00:00\tba3253876aed6bc22d4a6ff53d8406c6ad864195ed144ab5c87621b6c233b548baeae6956df346ec8c17f5ea10f35ee3cbc514797ed7ddd3145464e2a0bab413\tfjasklj;laj;lfdsa\t-----BEGIN RSA PUBLIC KEY-----\nMIIBCAKCAQEAvAGXkF+WvkwXgWejkesqXm26+64W26+a3eVQ3vGR90iC846FO5xC\nfIyKprC72JubcUluHzNzaC2of2c09SMIdKuArhFnAgbvnMB7NmWQNqwF968ZA3jx\nWv5wC999/QCwIgwzMCDpv8kbLIEaE8lKVjyufC+/q+WLP3lPy1lghikAznARMpXQ\nwJTbRifUjpCAzutvII2flvMyt3ZbefLQGFgvs0geyR1ULNBYlXD018l4jk4HNMtO\nS/nylNxum72UNSxQX300MIESiA19/RqlVoxUALmHg/TgfjHwsFMgtp/X0RFmPrwR\nw4O/1xvsaHCw95blSl/mb9HSXNGjTJAomQIBAw==\n-----END RSA PUBLIC KEY-----\tQLG5BbSqlpBOuUGhbl13VPF5EO8Q59ZL1TCtMxAuSLrHSwCK2hBxUJ7nY9YnMo4gXqxzfqU+Zpcx\nnGc6Geq8phQ/MJkWku4a5bBL869zoW507c342wtOAOqZ/cVa5+dL8lE8cB7HWK9+xRcd/mGZBYX6\nNGdgRSI36HAuSPd/77ugCj6sGSnZoMUtity+8rINquugcKTmBVnWjzi54zgyPRSIfb0yEVJJRdY1\n7uGn7lkAjojDqkURTDyOc+3SuXdHedm/kh1LfVsRk+8kSMgI5aSDfBZCHykqB4pUKn2nNdNjIdor\nOZ8uaiEi22YyxYC8TcTmILHNRsBtAPLzPmrcSg==\tzhengshu");
    test2.append("2 7\t1540904022\t2018-10-30T20:53:42\t2042-01-01 00:00:00\tba3253876aed6bc22d4a6ff53d8406c6ad864195ed144ab5c87621b6c233b548baeae6956df346ec8c17f5ea10f35ee3cbc514797ed7ddd3145464e2a0bab413\tqweritrksjfdfdfd\t-----BEGIN RSA PUBLIC KEY-----\nMIIBCAKCAQEAv1DEo2sureAE+TrdYSbGhgylZYeTRr1OUX5nUAWDy6+Bqv4w0DHM\nGRBUHSBz5zhejREpbt54RDaJWVZff3A68fjB6zqa5ttc/TjMqo1pEP82azxmgvYj\nVFpa88Wu01976eI8sw7gJa+ZEfgGeGHle+fHoDREzeLzThTE77ALhisKXOZyY6a9\nRrPLJwcZiAhEu1JhCzBvafLpePSkbwfSZFNZx1LQfENoyABtvXjaKbc2Gs3htdk7\nM4nWn23nzBLX3jCy3KVvANhWEDnf1zi4BFq/KQ4oyTGuFRQu93lDIIQBorkh+E7P\nVDfGIr8jK674c/kzHZM8nwp7tM5DS6AqTwIBAw==\n-----END RSA PUBLIC KEY-----\tmTWoPUNOXxKDmO0qABsTf76WaocZ8fzpxrN+TTFocBFaZ0hZz4yP7CGaoHVPo7jLVN4Ky5NJ7Grh\nvCP7VDgwC/y/6UKnGf7vfGrRisNeTEoQ6NeNoAh6qnNT41uW7fs1JsVTBNZH9WZct15A5EyoYXQA\nNnDw/cEFYb/NCE7yPCiTAjUWMwIeBHH7izfxwqbZ8EpC0LvC+wT50sU90g2P+MSAE7zqcvdx/Ens\nNZCpdjZmf9yJYSUHQYQQHRdoKF4vMfAUeqVGMDHM/TpYhMwwOnl2bnpB1Y+oPzaB6cyY+nH+JyL6\nM0yu0hYVJbtD7tw4k2j0l0BuyEx7sFBvlkyUwg==\tjfdklsjsjl");
    test->append(test1);
    test->append(test2);
    QString list,temporary;//分别为Widget读到的信息和临时变量
    list = item->text();
    int n,j,k = 0,num;
    int publickey = 0;//公钥是否读取
    std::string aa;
    QVector<QByteArray> verifi;
    QByteArray verti,pubkey,ciphertext,test11;
    n = list.size();
    for(int i = 0;i<n;i++)//读取当前选中的印章
    {
        if(i > 3){
            temporary.append(list.at(i));
            j = temporary.toInt();   //j为选中的电子印章编号
        }
    }
    num = j;
    verifi = this->info->at(j-1);
    for(int i = 0;i<verifi.size();i++)
    {
        verti = verifi.at(i);
        for(int t = 0;t<verti.size();t++)
        {
            if(verti.at(t) == '\n'){
                k++;
            }
            if(k >=5 && publickey == 0){//读取公钥，当读到\t时跳过本次循环继续往下读取，否则就继续往下读取内容
                if (verti.at(t) == '@') {
                    publickey = 1;
                    t++;
                    continue;
                } else {
                    pubkey += verti.at(t);
                }
            }
            else if(publickey == 1){
                if (verti.at(t) == '@') {
                    publickey = 2;
                }else {
                    ciphertext += verti.at(t);
                }
            }
        }
    }
    ciphertext.insert(0,'\n');
    ciphertext.remove(ciphertext.size()-1,1);
    aa = Decode((const char *)ciphertext.data(),ciphertext.length(),n);
    aa = rsa_pub_decrypt(aa,pubkey.data());
    QString qstring;
    QString temporary0,temporary1 = nullptr,temporary2 = nullptr,temporary3 = nullptr,temporary4 = nullptr;
    qstring = QString::fromStdString(aa);
    qDebug()<<qstring;
    j=0;
    for(int i =0;i<qstring.size();i++)
    {
        if(qstring[i] == '/'){
            j++;
            continue;
        }
        if(j == 0){
            temporary0 += qstring[i];
        }
        else if(j == 1){
            temporary1 += qstring[i];
        }
        else if(j == 2){
            temporary2 += qstring[i];
        }
        else if(j == 3){
            temporary3 += qstring[i];
        }
        else if(j == 4){
            temporary4 += qstring[i];
        }
    }
    ui->messageLabel->setText("签章编号："+QString::number(num)+"\n签发人/单位:"+temporary0+"\n签章用户:"+temporary1+"\n签章名称:"+temporary2+"\n开始时间:"+temporary3+"\n截止时间:"+temporary4+"\n");
}
void BatchValidation::on_pushButton_clicked()//打开服务器端回传信息显示界面
{
    if(json.isEmpty()){
        QMessageBox::warning(this,"警告","数据未加载完毕，请稍等。");
        return;
    }
    ChaincodeInfoDialog* dialog = new ChaincodeInfoDialog();
    dialog->display(json);
    dialog->exec();
}
void BatchValidation::getServerInfo(QJsonObject jsonObject)
{
    if(!jsonObject.value("error").toString().isEmpty()){
        QMessageBox::warning(this,"警告",jsonObject.value("error").toString());
        return;
    }
    json = jsonObject.value("list").toObject();
}
