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

#include <config.h>

#include <QCoreApplication>
#include <QDir>
#include <QHash>
#include <QtDebug>
#include "plexyconfig.h"
#include <widget.h>
#include <QDesktopWidget>

#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace UIKit {

class Config::Private {
public:
  Private() {}
  ~Private() {}
};

Config *Config::config = 0;
QNetworkAccessManager *Config::m_networkaccessmanager = 0;

Config *Config::instance() {
  if (config == 0) {
    config =
        new Config(QLatin1String("plexydesk"), QLatin1String("plexydesktop"));
    return config;
  } else {
    return config;
  }
}

Config::Config(const QString &a_organization, const QString &a_application,
               QObject *parent)
    : QObject(parent), d(new Private) {}

Config::~Config() { delete d; }

QString Config::prefix() {
#ifndef Q_OS_LINUX
  QDir binaryPath(QCoreApplication::applicationDirPath());
  if (binaryPath.cdUp()) {
    return QDir::toNativeSeparators(binaryPath.canonicalPath());
  }
#endif

#ifdef Q_OS_LINUX
  QString basePath(qgetenv("PLEXYDESK_DIR"));
  if (basePath.isEmpty() || basePath.isNull()) {
    return PLEXYPREFIX;
  }

  return basePath;
#endif

#ifdef Q_OS_MAC
  CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFStringRef macPath =
      CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
  const char *pathPtr =
      CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
  CFRelease(appUrlRef);
  CFRelease(macPath);
  return QLatin1String(pathPtr) + QString("/Contents/");
#endif

  return QString();
}

QString Config::cache_dir(const QString &a_folder) {
  QString rv = QDir::toNativeSeparators(QDir::homePath() + "/" +
                                        ".plexydesk/cache/" + a_folder);
  QDir(QDir::homePath()).mkpath(rv);
  return rv;
}

QNetworkAccessManager *Config::network_access_manager() {
  if (m_networkaccessmanager == 0) {
    m_networkaccessmanager = new QNetworkAccessManager(instance());
    // networkaccessmanager->setCookieJar(getCookieJar());
    return m_networkaccessmanager;
  } else {
    return m_networkaccessmanager;
  }
}
}
