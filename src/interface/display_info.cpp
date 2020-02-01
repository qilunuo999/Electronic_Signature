#include "include/interface/display_info.h"
#include "ui_display_info.h"
#include "QDebug"

display_info::display_info(QWidget *parent,QVector<QVector<QByteArray>>*info) :
    QDialog(parent),
    ui(new Ui::display_info)
{
    ui->setupUi(this);
    this->info = info;
    QByteArray data;
    QString list,temporary;
    QStringList item;
    int n;
    n = this->info->size();
    for(int i = 0;i<n;i++)//在widget中画出相应的电子印章
    {
        if(this->info->at(0).at(0).size() == 10){
            item<<"时间"+QString::number(i+1);
        }
        else{
            item<<"证书"+QString::number(i+1);
        }
    }
    ui->listWidget->addItems(item);
}

display_info::~display_info()
{
    delete ui;
}

void display_info::on_pushButton_2_clicked()
{
    this->close();
}

void display_info::on_listWidget_itemClicked(QListWidgetItem *item)
{
    int j = 0,n,flag = 1;
    QString list,temporary;
    list = item->text();
    n = list.size();
    for(int i = 0;i<n;i++)//读取当前选中的印章
    {
        if(i > 1){
            temporary.append(list.at(i));
            j = temporary.toInt();   //j为选中的电子印章编号
        }
    }
    for(int i = 0;i<this->info->at(j-1).at(0).size();i++)
    {
        if(this->info->at(j-1).at(0).size() == 10){
            ui->label->setText(this->info->at(j-1).at(0));
        }
        else{
            flag = 0;
        }
    }
    if(flag == 0){
        Cer_verifi(this->info->at(j-1).at(0).data());
    }
}

void display_info::Cer_verifi(const char *bytes)      //验证x509证书使用时传入证书内容即可
{                                                       //(base64编码)
    X509_NAME *issuer = new X509_NAME;       //证书的办法者信息
    X509_NAME *subject = new X509_NAME;      //证书使用者的信息
    ASN1_TIME *timeBefore,*timeAfter; //分别是证书的开始日期和截止日期
    BIO *bio_mem = new BIO;
    bio_mem = BIO_new(BIO_s_mem());
    BIO_puts(bio_mem, bytes);
    X509 *x509 = new X509;
    x509 = PEM_read_bio_X509(bio_mem, nullptr, nullptr, nullptr);
    BIO *bio_out = new BIO;
    bio_out = BIO_new_fp(stdout, BIO_NOCLOSE);

    long l = X509_get_version(x509);    //证书的版本号:l+1
    ASN1_INTEGER *bs;                   //证书的序列号
    bs = X509_get_serialNumber(x509);
    QString bbs = "";
    for(int i=0; i<bs->length; i++) {
       bbs.append(QString::number(bs->data[i],16));
    }
    char sig[128] = {0};
    ASN1_OBJECT* salg  = nullptr;
    salg = x509->sig_alg->algorithm;
    OBJ_obj2txt(sig,128,salg,1);

    int nNameLen = 512;  //颁发者
    issuer=X509_get_issuer_name(x509);
    char issuercountry[512] = {0},issuerProvince[512] = {0},issuerlocality[512] = {0};//国家：NID_countryName       常用名：NID_commonName
    char issuerorganiza[512] = {0}, issuerOrgUnit[512] = {0},issuercommon[512] = {0};
    subject=X509_get_subject_name(x509);
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_countryName,issuercountry,512);//国家
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_stateOrProvinceName,issuerProvince,512);//省份
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_localityName,issuerlocality,512);//当前地址
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_organizationName,issuerorganiza,512);//组织
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_organizationalUnitName,issuerOrgUnit,512);//单位
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_commonName,issuercommon,512);//常用名/办法者
//证书有效时间
    QString ttBefore;
    QString ttAfter;
    int* err = nullptr;
//    long long nLLBefore = 0;
    timeBefore=X509_get_notBefore(x509);
    QDateTime dateTime = QDateTime::fromTime_t(static_cast<uint>(ASN1_TIMEGet(timeBefore, err)));
    ttBefore = dateTime.toString(Qt::SystemLocaleLongDate);

    timeAfter=X509_get_notAfter(x509);
    dateTime = QDateTime::fromTime_t(static_cast<uint>(ASN1_TIMEGet(timeAfter, err)));
    ttAfter = dateTime.toString(Qt::SystemLocaleLongDate);

    char subjectcountry[512] = {0},subjectProvince[512] = {0},subjectlocality[512] = {0};//国家：NID_countryName       常用名：NID_commonName
    char subjectorganiza[512] = {0}, subjectOrgUnit[512] = {0},subjectcommon[512] = {0};
    subject=X509_get_subject_name(x509);
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_countryName,subjectcountry,512);//国家
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_stateOrProvinceName,subjectProvince,512);//省份
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_localityName,subjectlocality,512);//当前地址
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_organizationName,subjectorganiza,512);//组织
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_organizationalUnitName,subjectOrgUnit,512);//单位
    nNameLen = X509_NAME_get_text_by_NID(subject,NID_commonName,subjectcommon,512);//常用名/办法者


    char buf[20],ch[128] = {0};   //sha1指纹
    const EVP_MD *digest = EVP_sha1();
    unsigned len;
    int rc = X509_digest(x509,digest,(unsigned char *)buf,&len);
    QByteArray sha1,test;
    sha1.append(buf);

    EVP_PKEY *pk = nullptr;     //证书所用加密算法
    pk = X509_get_pubkey(x509);
    QString pulType;
    if (EVP_PKEY_RSA == pk->type) {
        pulType = "RSA";
    } else if (EVP_PKEY_EC == pk->type) {
        pulType = "EC";
    } else if (EVP_PKEY_DSA == pk->type) {
        pulType = "DSA";
    } else if (EVP_PKEY_DH == pk->type) {
        pulType = "DH";
    } else {
        pulType = "UNKNOWN";
    }

    ui->label->setText("证书版本号：V"+QString::number(l+1)+"\n证书序列号："+bbs+"\n签名算法："+sig+"\n颁发者:\n国家:"+subjectcountry+"\n省份："+subjectProvince+"\n当前地址："+subjectlocality+"\n组织："+subjectorganiza+"\n单位:"+subjectOrgUnit+"\n常用名:"+subjectcommon+"\n使用者：\n国家:"+subjectcountry+"\n省份："+subjectProvince+"\n当前地址："+subjectlocality+"\n组织："+subjectorganiza+"\n单位:"+subjectOrgUnit+"\n常用名:"+subjectcommon+"\n在此之后有效："+ttBefore+"\n在此时候无效："+ttAfter+"\nSHA1指纹："+sha1.toHex());
    BIO_free(bio_out);
    BIO_free(bio_mem);
    X509_free(x509);
}

time_t display_info::ASN1_TIMEGet ( ASN1_TIME * a, int *err)
{
    char days[2][12] =
    {
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    };

    int dummy;
    unsigned const char *s;
    int generalized;
    struct tm t;
    int i, year, isleap, offset;
    time_t retval;
    if (err == nullptr) err = &dummy;
    if (a->type == V_ASN1_GENERALIZEDTIME)
    {
        generalized = 1;
    }
    else if (a->type == V_ASN1_UTCTIME)
    {
        generalized = 0;
    }
    else
    {
        *err = 1;
        return 0;
    }
    s = a->data;
    if (s == nullptr || s[a->length] != '\0')
    {
        *err = 1;
        return 0;
    }

    *err = 0;
    if (generalized)
    {
        t.tm_year = mypint(&s, 4, 0, 9999, err) - 1900;
    }
    else
    {
        t.tm_year = mypint(&s, 2, 0, 99, err);
        if (t.tm_year < 50) t.tm_year += 100;
    }

    t.tm_mon = mypint(&s, 2, 1, 12, err) - 1;
    t.tm_mday = mypint(&s, 2, 1, 31, err);
    t.tm_hour = mypint(&s, 2, 0, 23, err);
    t.tm_min = mypint(&s, 2, 0, 59, err);
    if (*s >= '0' && *s <= '9')
    {
        t.tm_sec = mypint(&s, 2, 0, 59, err);
    }
    else
    {
        t.tm_sec = 0;
    }
    if (*err) return 0;
    if (generalized)
    {
        while (*s == '.' || *s == ',' || (*s >= '0' && *s <= '9')) ++s;
        if (*s == 0)
        {
            t.tm_isdst = -1;
            retval = mktime(&t);
            if (retval == (time_t)-1)
            {
                *err = 2;
                retval = 0;
            }
            return retval;
        }

    }
    if (*s == 'Z')
    {
        offset = 0;
        ++s;
    }
    else if (*s == '-' || *s == '+')
    {
        i = (*s++ == '-');
        offset = mypint(&s, 2, 0, 12, err);
        offset *= 60;
        offset += mypint(&s, 2, 0, 59, err);
        if (*err) return 0;
        if (i) offset = -offset;
    }
    else
    {
        *err = 1;
        return 0;
    }
    if (*s)
    {
        *err = 1;
        return 0;
    }

    retval = t.tm_sec;
    retval += (t.tm_min - offset) * 60;
    retval += t.tm_hour * 3600;
    retval += (t.tm_mday - 1) * 86400;
    year = t.tm_year + 1900;
    if ( sizeof (time_t) == 4)
    {
        if (year < 1900 || year > 2040) *err = 2;
    }

    isleap = ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
    for (i = t.tm_mon - 1; i >= 0; --i) retval += days[isleap][i] * 86400;
    retval += (year - 1970) * 31536000;
    if (year < 1970)
    {
        retval -= ((1970 - year + 2) / 4) * 86400;
        if ( sizeof (time_t) > 4)
        {
            for (i = 1900; i >= year; i -= 100)
            {
                if (i % 400 == 0) continue ;
                retval += 86400;
            }
        }
        if (retval >= 0) *err = 2;
    }
    else
    {
        retval += ((year - 1970 + 1) / 4) * 86400;
        if ( sizeof (time_t) > 4)
        {
            for (i = 2100; i < year; i += 100)
            {
                if (i % 400 == 0) continue ;
                retval -= 86400;
            }

        }
        if (retval < 0) *err = 2;
    }
    if (*err) retval = 0;
    return retval;
}
int display_info::mypint(unsigned const char ** s,int n,int min,int max,int * e)
{
    int retval = 0;
    while (n)
    {
        if (**s < '0' || **s > '9')
        {
            *e = 1; return 0;
        }
        retval *= 10;
        retval += **s - '0';
        --n;
        ++(*s);
    }
    if (retval < min || retval > max) *e = 1;
    return retval;
}
