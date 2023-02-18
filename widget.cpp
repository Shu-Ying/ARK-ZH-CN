#include "widget.h"
#include "./ui_widget.h"

#include <QDesktopServices>
#include <QUrl>
#include <QEventLoop>
#include <QTextCodec>
#include <QTextStream>
#include <QDir>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QtNetwork>
#include <qtextcodec.h>
#include <QFileInfo>

#include <QDebug>

QString fileName;
QString binDir=QDir::currentPath()+"/bin"; //�����ж�bin�ļ����Ƿ����
QTextCodec *codec = QTextCodec::codecForName("GBK");//�����������

//QString curPath=QDir::currentPath(); //��ȡ��ǰӦ�õľ���·��  ����ȫ�ֱ�����ԭ���ǻ���Ϊ�Ӹ��ļ��е��³�BUG

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Diuse)
{
    QString curPath=QDir::currentPath(); //��ȡ��ǰӦ�õľ���·��
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString lastPath = setting.value("LastFilePath").toString(); //�����ϴα����·��

    manager= new QNetworkAccessManager(this);
    manager1= new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onlineVersionReplyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("https://diuse.coding.net/p/extension/d/ARK/git/raw/master/online_version.js?download=false")));
    connect(manager1,SIGNAL(finished(QNetworkReply*)),this,SLOT(onlineTipReplyFinished(QNetworkReply*)));
    manager1->get(QNetworkRequest(QUrl("https://diuse.coding.net/p/extension/d/ARK/git/raw/master/tip.js?download=false")));

    QString onlineVersion = setting.value("OnlineVersion").toString(); //�����������°汾
    QString version = setting.value("Version").toString();//���ر��ذ汾
    QString tip = setting.value("tip").toString();//���ؽ̳�/����

    ui->setupUi(this);

    ui->progressBar->hide();
    ui->filePath->setText(lastPath);
    ui->tipOnlineVersion->setText(onlineVersion);
    ui->tipVersion->setText(version);
    ui->tip->setText(tip);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_Go_clicked() //��ʼ�滻
{
    QString curPath=QDir::currentPath(); //��ȡ��ǰӦ�õľ���·��
    QString path=ui->filePath->text();
    QString filePath = curPath;

    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);

    if(path.isEmpty()){
        QMessageBox::information(this,codec->toUnicode("ϵͳ��ʾ"),codec->toUnicode("��д��ѡȡ·����ι"), QMessageBox::Ok);
    } else {
        filePath.append("/bin/ShooterGame.locres");
        setting.setValue("LastFilePath",path); //�����ַ�Ա����ʹ��
        QFileInfo fileBin(filePath);
        if(fileBin.exists()==false){
            QMessageBox::information(this,codec->toUnicode("ϵͳ��ʾ"),codec->toUnicode("Bin�ļ��л򺺻����������ڣ��������²���"), QMessageBox::Ok);
        } else {
            path.append("/ShooterGame.locres");
            QFile file(filePath);
            file.copy(path);
            QMessageBox::information(this,codec->toUnicode("ϵͳ��ʾ"),codec->toUnicode("�滻�ɹ�"), QMessageBox::Ok);
        }
    }
}

void Widget::on_openPath_clicked() //ѡ����Ϸ·��
{
    QString curPath=QDir::currentPath(); //��ȡ��ǰӦ�õľ���·��
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString lastPath = setting.value("LastFilePath").toString(); //�����ϴα����·��

    QString openFiles = QFileDialog::getExistingDirectory(this, "��ѡ���ļ�·��...", lastPath);

    if(!openFiles.isEmpty()){
        ui->filePath->setText(openFiles);
        QVariant VarPath(openFiles);
        setting.setValue("LastFilePath", VarPath);//��·��������Setting.ini �����´ε���
    }

    qDebug()<<"hello";
}

void Widget::onlineVersionReplyFinished(QNetworkReply *reply) //�������²����汾�ű�����Setting
{
    QString curPath=QDir::currentPath(); //��ȡ��ǰӦ�õľ���·��
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString onlineVersion= reply->readAll();
    //ui->textBrowser->setText(onlineVersion);
    reply->deleteLater();
    setting.setValue("OnlineVersion",onlineVersion);//�����粹���汾�ű�����Setting.ini
}

void Widget::versionReplyFinished(QNetworkReply *reply) //���ز����汾�ű�����Setting
{
    QString curPath=QDir::currentPath(); //��ȡ��ǰӦ�õľ���·��
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString version= reply->readAll();
    reply->deleteLater();
    setting.setValue("Version",version);//�����粹���汾�ű�����Setting.ini
}

void Widget::onlineTipReplyFinished(QNetworkReply *reply) //ʹ�ý̳̱�����Setting
{
    QString curPath=QDir::currentPath(); //��ȡ��ǰӦ�õľ���·��
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString tip= reply->readAll();
    reply->deleteLater();
    setting.setValue("Tip",tip);//�����粹���汾�ű�����Setting.ini
}

void Widget::startReauest(QUrl url)//���ؽ�����
{
    reply = manager->get(QNetworkRequest(url));
    connect(reply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateDataReadProgress(qint64,qint64)));
    connect(reply,SIGNAL(finished()),this,SLOT(httpFinished()));
}

void Widget::httpReadyRead()
{
    if(file)file->write(reply->readAll());
}

void Widget::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes) //���ؽ�����
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);

        qDebug()<<"hello";
}

void Widget::httpFinished()  //�������
{
    QDir dir;

    ui->progressBar->hide();
    file->flush();
    file->close();
    reply->deleteLater();
    reply=0;
    delete file;
    file=0; //����������ؽ�����

    if (!dir.exists(binDir))
    {
        bool res = dir.mkpath(binDir);
        qDebug() << "�½�Ŀ¼�Ƿ�ɹ�" << res;
    }

    QString curPath=QDir::currentPath(); //��ȡ��ǰӦ�õľ���·��
    QString binPath=curPath;
    binPath.append("/bin/ShooterGame.locres");
    curPath.append("/ShooterGame.locres");
    QFile file(curPath);
    file.copy(binPath); //�����ص��ļ����ƽ�ȥ
    file.remove(curPath); //�����ص��ļ�ɾ��

    manager= new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(versionReplyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("https://diuse.coding.net/p/extension/d/ARK/git/raw/master/version.js?download=false")));

    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString version = setting.value("Version").toString();//���ر��ذ汾
    ui->tipVersion->setText(version); //����UI��ʾ

    QMessageBox::information(this,codec->toUnicode("ϵͳ��ʾ"),codec->toUnicode("�������"), QMessageBox::Ok);

        qDebug()<<"hello";
}

void Widget::on_online_clicked() //���ػ���� �²���
{
    //url= ui->lineEdit->text();
    url="https://diuse.coding.net/p/extension/d/ARK/git/raw/master/ShooterGame.locres?download=true";
    QFileInfo info(url.path());
    QString fileName(info.fileName());
    file=new QFile(fileName);

    if(!file->open(QIODevice::WriteOnly))
    {
        qDebug()<<"File Open Error";
        delete file;
        file=0;
        return;
    }

    startReauest(url);
    ui->progressBar->setValue(0);
    ui->progressBar->show();

        qDebug()<<"hello";
}

void Widget::on_TieBa_clicked()
{
    QDesktopServices::openUrl(QUrl("https://tieba.baidu.com/home/main?id=tb.1.11259986.gwSQ6GUby_-PIi7uydn6NQ?t=1525076081&fr=pb"));
}

void Widget::on_renew_clicked()
{
    QString curPath=QDir::currentPath(); //��ȡ��ǰӦ�õľ���·��
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);

    manager= new QNetworkAccessManager(this);
    manager1= new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onlineVersionReplyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("https://diuse.coding.net/p/extension/d/ARK/git/raw/master/online_version.js?download=false")));
    connect(manager1,SIGNAL(finished(QNetworkReply*)),this,SLOT(onlineTipReplyFinished(QNetworkReply*)));
    manager1->get(QNetworkRequest(QUrl("https://diuse.coding.net/p/extension/d/ARK/git/raw/master/tip.js?download=false")));

    QString lastPath = setting.value("LastFilePath").toString(); //�����ϴα����·��
    QString onlineVersion = setting.value("OnlineVersion").toString(); //�����������°汾
    QString version = setting.value("Version").toString();//���ر��ذ汾
    QString tip = setting.value("tip").toString();//���ؽ̳�/����

    ui->filePath->setText(lastPath);
    ui->tipOnlineVersion->setText(onlineVersion);
    ui->tipVersion->setText(version);
    ui->tip->setText(tip);

        qDebug()<<"hello";
}
