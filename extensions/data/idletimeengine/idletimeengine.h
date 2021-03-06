/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@kde.org>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#ifndef IDLETIMEENGINE_DATA_H
#define IDLETIMEENGINE_DATA_H

#include <QtCore>
#include <plexy.h>
#include <abstractplugininterface.h>
#include <datasource.h>
#include <QtNetwork>

class IdleTimeEngineData : public PlexyDesk::DataSource {
  Q_OBJECT

public:
  IdleTimeEngineData(QObject *object = 0);
  virtual ~IdleTimeEngineData();
  void init();
  QVariantMap readAll();

  void timerEvent(QTimerEvent *event);

public
Q_SLOTS:
  void setArguments(QVariant sourceUpdated);
  void onAwakeEvent();

private:
  class PrivateIdleTimeEngine;
  PrivateIdleTimeEngine *const d;
};

#endif
