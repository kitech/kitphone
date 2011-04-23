// skypeobject.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-10-24 17:04:27 +0800
// Version: $Id$
// 

#include "skypeobject.h"


int SkypeMeta::CallStatusToInt(QString status)
{
  int istatus = 0;

 if (status == "UNPLACED") {
   istatus = CS_UNPLACED;
 } else if (status == "ROUTING") {
   istatus = CS_ROUTING;
 } else if (status == "REMOTEHOLD") {
   istatus = CS_REMOTEHOLD;
 } else if (status == "LOCALHOLD") {
   istatus = CS_LOCALHOLD;
 }
  
  return istatus;
}
