#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QUrl>

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = nullptr);
    ~Network();
    void sendRequest(QUrl url);//初始化网络模块


signals:
    void finish(QJsonObject);//处理数据结束信号

public slots:
    void finishReply(QNetworkReply*);//接收完毕槽函数

private:
    QNetworkAccessManager *accessManager;
    QNetworkRequest request;
    QJsonObject analyzeReply(QByteArray data);

};

#endif // NETWORK_H
