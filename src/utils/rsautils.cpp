#include "include/utils/rsautils.h"

RSAUtils::RSAUtils()
{

}
//base64加密
std::string Encode(const unsigned char* Data,int DataByte)
{
    //编码表
    const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    //返回值
    std::string strEncode;
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

//加载私钥文件
QByteArray RSAUtils::load_pri_key(QString keyFilePath)
{
    QFile file(keyFilePath);
    file.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream key(&file);
    QByteArray temp;
    temp.append(key.readAll());
    return temp;
}

// 私钥加密
QByteArray RSAUtils::rsa_pri_encrypt(std::string clearText, QByteArray priKey)
{
    std::string strRet;
    RSA *rsa = nullptr;
    BIO *keybio = BIO_new_mem_buf(priKey.data(), -1);

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

    return QByteArray::fromStdString(strRet);
}

// 公钥解密
QByteArray RSAUtils::rsa_pub_decrypt(QByteArray cipherText,QByteArray pubKey)
{
    QByteArray strRet;
    RSA *rsa = RSA_new();
    BIO *keybio;
    keybio = BIO_new_mem_buf(pubKey.data(), -1);

    rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, nullptr, nullptr);

    size_t len = static_cast<size_t>(RSA_size(rsa));
    char *decryptedText = static_cast<char*>(malloc(len + 1));
    memset(decryptedText, 0, len + 1);
    cipherText = QByteArray::fromBase64(cipherText);
    // 解密函数
    int ret = RSA_public_decrypt(cipherText.length(), reinterpret_cast<const unsigned char*>(cipherText.data()), reinterpret_cast<unsigned char*>(decryptedText), rsa, RSA_PKCS1_PADDING);
    if (ret > 0)
        strRet = QByteArray(decryptedText, ret);

    // 释放内存
    free(decryptedText);
    BIO_free_all(keybio);
    RSA_free(rsa);

    return strRet;
}


