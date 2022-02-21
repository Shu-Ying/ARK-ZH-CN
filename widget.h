#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

class QFile;
class QNetworkReply;
class QNetworkAccessManager;

QT_BEGIN_NAMESPACE
namespace Ui { class Diuse; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void startReauest(QUrl url);
    QNetworkReply *reply;
    QUrl url;
    QFile *file;

private slots:
    void on_Go_clicked();
    void on_openPath_clicked();
    void onlineVersionReplyFinished(QNetworkReply *);
    void versionReplyFinished(QNetworkReply *);
    void onlineTipReplyFinished(QNetworkReply *);
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64,qint64);
    void on_online_clicked();
    void on_TieBa_clicked();

    void on_renew_clicked();

private:
    Ui::Diuse *ui;
    QNetworkAccessManager *manager;
    QNetworkAccessManager *manager1;
};
#endif // WIDGET_H
