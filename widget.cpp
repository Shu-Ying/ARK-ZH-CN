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
QString binDir=QDir::currentPath()+"/bin"; //用于判断bin文件夹是否存在
QTextCodec *codec = QTextCodec::codecForName("GBK");//解决中文乱码

//QString curPath=QDir::currentPath(); //获取当前应用的绝对路径  不用全局变量的原因是会因为子父文件夹导致出BUG

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Diuse)
{
    QString curPath=QDir::currentPath(); //获取当前应用的绝对路径
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString lastPath = setting.value("LastFilePath").toString(); //加载上次保存的路径

    manager= new QNetworkAccessManager(this);
    manager1= new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onlineVersionReplyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("https://diuse.coding.net/p/extension/d/ARK/git/raw/master/online_version.js?download=false")));
    connect(manager1,SIGNAL(finished(QNetworkReply*)),this,SLOT(onlineTipReplyFinished(QNetworkReply*)));
    manager1->get(QNetworkRequest(QUrl("https://diuse.coding.net/p/extension/d/ARK/git/raw/master/tip.js?download=false")));

    QString onlineVersion = setting.value("OnlineVersion").toString(); //加载网络最新版本
    QString version = setting.value("Version").toString();//加载本地版本
    QString tip = setting.value("tip").toString();//加载教程/公告

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

void Widget::on_Go_clicked() //开始替换
{
    QString curPath=QDir::currentPath(); //获取当前应用的绝对路径
    QString path=ui->filePath->text();
    QString filePath = curPath;

    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);

    if(path.isEmpty()){
        QMessageBox::information(this,codec->toUnicode("系统提示"),codec->toUnicode("填写或选取路径啊喂"), QMessageBox::Ok);
    } else {
        filePath.append("/bin/ShooterGame.locres");
        setting.setValue("LastFilePath",path); //保存地址以便后续使用
        QFileInfo fileBin(filePath);
        if(fileBin.exists()==false){
            QMessageBox::information(this,codec->toUnicode("系统提示"),codec->toUnicode("Bin文件夹或汉化补丁不存在，请点击更新补丁"), QMessageBox::Ok);
        } else {
            path.append("/ShooterGame.locres");
            QFile file(filePath);
            file.copy(path);
            QMessageBox::information(this,codec->toUnicode("系统提示"),codec->toUnicode("替换成功"), QMessageBox::Ok);
        }
    }
}

void Widget::on_openPath_clicked() //选择游戏路径
{
    QString curPath=QDir::currentPath(); //获取当前应用的绝对路径
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString lastPath = setting.value("LastFilePath").toString(); //加载上次保存的路径

    QString openFiles = QFileDialog::getExistingDirectory(this, "请选择文件路径...", lastPath);

    if(!openFiles.isEmpty()){
        ui->filePath->setText(openFiles);
        QVariant VarPath(openFiles);
        setting.setValue("LastFilePath", VarPath);//将路径保存至Setting.ini 方便下次调用
    }

    qDebug()<<"hello";
}

void Widget::onlineVersionReplyFinished(QNetworkReply *reply) //网络最新补丁版本号保存至Setting
{
    QString curPath=QDir::currentPath(); //获取当前应用的绝对路径
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString onlineVersion= reply->readAll();
    //ui->textBrowser->setText(onlineVersion);
    reply->deleteLater();
    setting.setValue("OnlineVersion",onlineVersion);//将网络补丁版本号保存至Setting.ini
}

void Widget::versionReplyFinished(QNetworkReply *reply) //本地补丁版本号保存至Setting
{
    QString curPath=QDir::currentPath(); //获取当前应用的绝对路径
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString version= reply->readAll();
    reply->deleteLater();
    setting.setValue("Version",version);//将网络补丁版本号保存至Setting.ini
}

void Widget::onlineTipReplyFinished(QNetworkReply *reply) //使用教程保存至Setting
{
    QString curPath=QDir::currentPath(); //获取当前应用的绝对路径
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString tip= reply->readAll();
    reply->deleteLater();
    setting.setValue("Tip",tip);//将网络补丁版本号保存至Setting.ini
}

void Widget::startReauest(QUrl url)//下载进度条
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

void Widget::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes) //下载进度条
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);

        qDebug()<<"hello";
}

void Widget::httpFinished()  //下载完成
{
    QDir dir;

    ui->progressBar->hide();
    file->flush();
    file->close();
    reply->deleteLater();
    reply=0;
    delete file;
    file=0; //下载完成隐藏进度条

    if (!dir.exists(binDir))
    {
        bool res = dir.mkpath(binDir);
        qDebug() << "新建目录是否成功" << res;
    }

    QString curPath=QDir::currentPath(); //获取当前应用的绝对路径
    QString binPath=curPath;
    binPath.append("/bin/ShooterGame.locres");
    curPath.append("/ShooterGame.locres");
    QFile file(curPath);
    file.copy(binPath); //将下载的文件复制进去
    file.remove(curPath); //将下载的文件删除

    manager= new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(versionReplyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("https://diuse.coding.net/p/extension/d/ARK/git/raw/master/version.js?download=false")));

    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);
    QString version = setting.value("Version").toString();//加载本地版本
    ui->tipVersion->setText(version); //更新UI显示

    QMessageBox::information(this,codec->toUnicode("系统提示"),codec->toUnicode("下载完成"), QMessageBox::Ok);

        qDebug()<<"hello";
}

void Widget::on_online_clicked() //下载或更新 新补丁
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
    QString curPath=QDir::currentPath(); //获取当前应用的绝对路径
    curPath.append("/Setting.ini");
    QSettings setting(curPath, QSettings::IniFormat);

    manager= new QNetworkAccessManager(this);
    manager1= new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onlineVersionReplyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("https://diuse.coding.net/p/extension/d/ARK/git/raw/master/online_version.js?download=false")));
    connect(manager1,SIGNAL(finished(QNetworkReply*)),this,SLOT(onlineTipReplyFinished(QNetworkReply*)));
    manager1->get(QNetworkRequest(QUrl("https://diuse.coding.net/p/extension/d/ARK/git/raw/master/tip.js?download=false")));

    QString lastPath = setting.value("LastFilePath").toString(); //加载上次保存的路径
    QString onlineVersion = setting.value("OnlineVersion").toString(); //加载网络最新版本
    QString version = setting.value("Version").toString();//加载本地版本
    QString tip = setting.value("tip").toString();//加载教程/公告

    ui->filePath->setText(lastPath);
    ui->tipOnlineVersion->setText(onlineVersion);
    ui->tipVersion->setText(version);
    ui->tip->setText(tip);

        qDebug()<<"hello";
}
