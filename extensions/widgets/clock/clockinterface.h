/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#ifndef PLEXY_CLOCK_I
#define PLEXY_CLOCK_I

#include <QtCore>
#include <plexy.h>
#include "clock.h"
#include "clockwidget.h"
#include <view_controller.h>
#include <controllerplugininterface.h>

class ClockInterface : public QObject, public UIKit::ControllerPluginInterface {
  Q_OBJECT
  Q_INTERFACES(UIKit::ControllerPluginInterface)
  Q_PLUGIN_METADATA(IID "org.qt-project.clock")

public:
  QSharedPointer<UIKit::ViewController> controller();

private:
  Clock *mClock;
};
#endif
