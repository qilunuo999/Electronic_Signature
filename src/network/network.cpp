#include "include/network/network.h"

Network::Network(QObject *parent)
{

}

Network::~Network()
{
    delete accessManager;
}

void Network::sendRequest(QUrl url)
{
    accessManager = new QNetworkAccessManager (this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishReply(QNetworkReply*)));
    request.setUrl(url);
    accessManager->get(request);
    qDebug()<<accessManager->objectName();
}

void Network::finishReply(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
         {
             QByteArray data = reply->readAll();
             QJsonObject jsonObject = analyzeReply(data);
             emit(finish(jsonObject));
         }
         else
         {
             qDebug() << "finishedSlot errors here";
             qDebug( "found error .... code: %d\n", static_cast<int>(reply->error()));
             qDebug() << reply->errorString();
         }
         reply->deleteLater();
}

QJsonObject Network::analyzeReply(QByteArray data)
{
    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(data, &json_error));
    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!";
        return jsonDoc.object();
    }
    return jsonDoc.object();
}
