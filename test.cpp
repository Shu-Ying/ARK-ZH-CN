#include "test.h"
#include <QDebug>

Test::Test(QObject *parent)
    : QObject{parent}
{
    qDebug()<<"hell";
}
