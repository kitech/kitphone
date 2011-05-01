// callhistorymodel.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-05-01 13:27:36 +0800
// Version: $Id$
// 

#include "callhistorymodel.h"

CallHistoryModel::CallHistoryModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    this->mroot = NULL;
}

CallHistoryModel::~CallHistoryModel()
{
}

