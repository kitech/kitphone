// metauri.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-07-02 11:02:47 +0800
// Version: $Id: metauri.cpp 153 2010-07-04 05:37:05Z drswinghead $
// 

#include "metauri.h"

MetaUri::MetaUri()
{
    this->valid = false;
}
MetaUri::~MetaUri()
{
}

void MetaUri::dump()
{
    qDebug()<<"==== MetaUri package >>>";
    qDebug()<<"url: "<<this->url;
    qDebug()<<"nameMd5:"<<this->nameMd5;
    qDebug()<<"fileSize:"<<this->fileSize;
    qDebug()<<"owner:"<<this->owner;
    qDebug()<<"mtime:"<<this->mtime;
    qDebug()<<"<<<< MetaUri package ===";

}

QString MetaUri::toString()
{
    QString str;

    str = "MetaUri";
    str += "|" + this->url.toAscii().toHex();
    str += "|" + this->nameMd5;
    str += "|" + this->contentMd5;
    str += "|" + QString("%1").arg(this->fileSize);
    str += "|" + owner.toAscii();
    str += "|" + QString("%1").arg(this->valid ? "T" : "F");
    str += "|" + QString("%1").arg(this->mtime);

    return str;
    return QString();
}

MetaUri MetaUri::fromString(QString str)
{
    QByteArray ba = str.toAscii();
    QList<QByteArray> elts = ba.split('|');
    if (elts.count() != 9 || elts.at(0) != "MetaUri") {
        qDebug()<<"Invalid MetaUri package.";
        return MetaUri();
    }

    MetaUri mu;
    mu.url = QString(QByteArray::fromHex(elts.at(1)));
    mu.nameMd5 = elts.at(2); // QString(QByteArray::fromHex(elts.at(2)));
    mu.contentMd5 = elts.at(3); // QString(QByteArray::fromHex(elts.at(3)));
    mu.fileSize = elts.at(4).toLongLong(); // QString(QByteArray::fromHex(elts.at(4))).toLongLong();
    mu.owner = elts.at(5); // QString(QByteArray::fromHex(elts.at(5)));
    mu.valid = elts.at(6) == "T" ? true : false; // QString(QByteArray::fromHex(elts.at(6))) == "true" ? true : false;
    mu.mtime = elts.at(7).toInt(); // QString(QByteArray::fromHex(elts.at(7))).toInt();

    return MetaUri();
}

bool MetaUri::match(MetaUri *mu)
{
    if (mu->nameMd5 == this->nameMd5
        // && mu->contentMd5 == this->contentMd5
        ) {
        return true;
    }
    return false;
}

bool SkypePackage::isValid()
{
    return (this->seq >= 0 && this->type > SPT_MIN && this->type < SPT_MAX);
}

QString SkypePackage::toString()
{
    QString str;

    str = "SkypePackage";
    str += "," + QString("%1").arg(this->seq);
    str += "," + QString("%1").arg(this->type);
    str += "," + this->data;

    return str;
}
// static 
SkypePackage SkypePackage::fromString(QString str)
{
    SkypePackage sp;

    QList<QString> elts = str.split(",");
    
    // qDebug()<<elts.count()<<elts
    //         <<(elts.count() == 4)
    //         <<(elts.at(0) == QString("SkypePackage"));
    // if (elts.at(0) != QString("SkypePackage")) {
    //     QString str = elts.at(0);
    //     int len = str.length();
    //     for (int i = 0 ; i < len ; ++i) {
    //         qDebug()<<str.at(i)<<QString("SkypePackage").at(i)
    //                 <<(str.at(i) == QString("SkypePackage").at(i));
    //     }
    // }
    if (elts.count() != 4 || elts.at(0) != QString("SkypePackage")) {
        qDebug()<<__FILE__<<__LINE__<<"Invalid SkypePackage";
        return sp;
    }

    sp.seq = elts.at(1).toInt();
    sp.type = elts.at(2).toInt();
    sp.data = elts.at(3);

    return sp;
}

