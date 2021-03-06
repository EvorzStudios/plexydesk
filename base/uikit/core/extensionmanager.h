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
#ifndef EXTENSION_MANAGER_H
#define EXTENSION_MANAGER_H

#include <plexy.h>

#include <QHash>
#include <QSharedPointer>

#include <datasource.h>
#include <view_controller.h>
#include <widgetstyleinterface.h>

#include <desktopactivity.h>
#include <plexydesk_ui_exports.h>

namespace UIKit {
/**
    * @brief PlexyDesk Plugin Manager Class
    *
    * Supports loading and handling various plexydesk
    * extensions supported by the sytem
    */
class DECL_UI_KIT_EXPORT ExtensionManager : public QObject {
  Q_OBJECT
public:
  /**
      * @brief
      *
      * @param parent
      */
  ExtensionManager(QObject *a_parent_ptr = 0);
  /**
      * @brief
      *
      */
  virtual ~ExtensionManager();
  /**
      * @brief
      *
      * @param desktopPrefix
      * @param libPrefix
      * @return ExtensionManager
      */
  static ExtensionManager *instance(const QString &a_desktopPrefix,
                                    const QString &a_prefix);

  static ExtensionManager *init(const QString &a_desktopPrefix,
                                const QString &a_prefix);

  /**
      * @brief
      *
      * @return ExtensionManager
      */
  static ExtensionManager *instance();

  static void destroy_instance();
  /**
      * @brief
      *
      * @param types
      * @return QStringList
      */
  QStringList extension_list(const QString &a_types);
  /**
      * @brief
      *
      * @param name
      * @return DataSourcePtr
      */
  DataSourcePtr data_engine(const QString &a_name);
  /**
      * @brief
      *
      * @param name
      * @return ControllerPtr
      */
  ViewControllerPtr controller(const QString &a_name);
  /**
      * @brief
      *
      * @param name
      * @return DesktopActivityPtr
      */
  UIKit::DesktopActivityPtr activity(const QString &a_name);
  /**
      * @brief
      *
      * @param name
      * @return StylePtr
      */
  StylePtr style(const QString &a_name);
  /**
    * @brief
    *
    * @param key
    * @return QString
    */
  QString desktop_controller_extension_info(const QString &a_key) const;

private:
  /**
      * @brief
      *
      */
  void scan_for_plugins();
  /**
      * @brief
      *
      * @param path
      */
  void load_desktop(const QString &a_path);
  /**
      * @brief
      *
      * @param _interface
      * @param plugin
      */
  void load(const QString &a_interface, const QString &a_plugin_name);
  /**
      * @brief
      *
      * @param path
      */
  void set_plugin_prefix(const QString &a_path);
  /**
      * @brief
      *
      * @param path
      */
  void set_plugin_info_prefix(const QString &a_path);
  /**
      * @brief
      *
      * @return QString
      */
  QString plugin_info_prefix() const;
  /**
      * @brief
      *
      * @return QString
      */
  QString plugin_prefix() const;

  class PrivateExtManager;
  PrivateExtManager *const d; /**< TODO */

#ifdef Q_OS_WIN
  static ExtensionManager *mInstance;
#else
  static DECL_UI_KIT_EXPORT ExtensionManager *mInstance; /**< TODO */
#endif
};
} // namespace PlexDesk
#endif
