/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  : *
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
#include "dock.h"
#include "snapframe.h"

// Qt
#include <QGraphicsLinearLayout>

#include <toolbar.h>
#include <imagebutton.h>
#include <tableview.h>

#include <workspace.h>

// models
#include <default_table_model.h>
#include <default_table_component.h>
#include <item_view.h>

// core
#include <plexyconfig.h>
#include <extensionmanager.h>
#include <imageview.h>

using namespace UIKit;

class DockControllerImpl::PrivateDock {
public:
  PrivateDock() {}
  ~PrivateDock() { qDebug() << Q_FUNC_INFO; }

public:
  Window *m_dock_window;
  Window *m_preview_window;

  UIKit::ToolBar *m_navigation_dock;
  UIKit::ItemView *m_preview_widget;

  QMap<QString, int> m_actions_map;
  QStringList m_controller_name_list;
  bool m_main_panel_is_hidden;

  UIKit::DesktopActivityPtr m_action_activity;
  UIKit::ImageButton *m_add_new_workspace_button_ptr;
  UIKit::ActionList m_supported_action_list;
};

DockControllerImpl::DockControllerImpl(QObject *object)
    : UIKit::ViewController(object), d(new PrivateDock) {
  d->m_actions_map["ToggleDock"] = 1;
  d->m_actions_map["ShowDock"] = 2;
  d->m_actions_map["HideDock"] = 3;
  d->m_actions_map["HideDock"] = 4;
  d->m_actions_map["ShowMenu"] = 5;

  d->m_main_panel_is_hidden = true;
  // navigation
  d->m_navigation_dock = new UIKit::ToolBar();
  d->m_navigation_dock->set_controller(this);
  d->m_navigation_dock->set_orientation(Qt::Vertical);
  d->m_navigation_dock->set_icon_resolution("hdpi");
  d->m_navigation_dock->set_icon_size(QSize(48, 48));

  d->m_navigation_dock->add_action(tr("Up"), "actions/pd_to_top", false);
  d->m_navigation_dock->add_action(tr("Expose"), "actions/pd_view_grid", false);
  d->m_navigation_dock->add_action(tr("Add"), "actions/pd_add", false);
  d->m_navigation_dock->add_action(tr("Menu"), "actions/pd_overflow_tab", false);
  d->m_navigation_dock->add_action(tr("Seamless"), "actions/pd_browser", false);
  d->m_navigation_dock->add_action(tr("Close"), "actions/pd_delete", false);
  d->m_navigation_dock->add_action(tr("Down"), "actions/pd_to_bottom", false);

  d->m_navigation_dock->setGeometry(d->m_navigation_dock->frame_geometry());
  d->m_navigation_dock->on_item_activated([this](const QString &a_action) {
      onNavigationPanelClicked(a_action);
  });
  // menu
  d->m_preview_widget = new UIKit::ItemView();
  d->m_preview_widget->on_item_removed([this](UIKit::ModelViewItem *a_item) {
      if (a_item) {
          delete a_item;
        }
  });
  d->m_preview_widget->on_activated([this](int index) {
    if (this->viewport() && this->viewport()->workspace()) {
      UIKit::WorkSpace *_workspace =
          qobject_cast<UIKit::WorkSpace *>(viewport()->workspace());

      if (_workspace) {
        _workspace->expose(index);
      }
    }
  });
}

DockControllerImpl::~DockControllerImpl() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void DockControllerImpl::init() {
  d->m_supported_action_list << createAction(1, tr("Menu"), "pd_menu_icon.png");
  d->m_supported_action_list << createAction(2, tr("show-dock"),
                                             "pd_menu_icon.png");
  d->m_supported_action_list << createAction(3, tr("hide-dock"),
                                             "pd_menu_icon.png");
  d->m_supported_action_list << createAction(4, tr("show-expose"),
                                             "pd_menu_icon.png");
  d->m_supported_action_list << createAction(5, tr("hide-expose"),
                                             "pd_menu_icon.png");

  if (!viewport()) {
    return;
  }

  UIKit::Space *_space = qobject_cast<UIKit::Space *>(viewport());
  if (_space) {
    d->m_action_activity =
        createActivity("", "icongrid", "", QPoint(), QVariantMap());

    if (d->m_action_activity && d->m_action_activity->window()) {
      d->m_action_activity->window()->set_window_type(Window::kPopupWindow);
      d->m_action_activity->window()->setVisible(false);
    }
  }

  // loads the controllers before dock was created;
  Q_FOREACH(const QString & name, viewport()->current_controller_list()) {
    loadControllerActions(name);
  }

  viewport()->on_viewport_event_notify([this](
      Space::ViewportNotificationType aType, const QVariant &aData,
      const Space *aSpace) {

    if (aType == Space::kControllerAddedNotification) {
      loadControllerActions(aData.toString());
    }
  });

  d->m_dock_window = new UIKit::Window();
  d->m_preview_window = new UIKit::Window();

  d->m_dock_window->set_window_type(Window::kPanelWindow);
  d->m_preview_window->set_window_type(Window::kPopupWindow);
  d->m_preview_window->enable_window_background(false);

  d->m_dock_window->set_window_content(d->m_navigation_dock);
  d->m_preview_window->set_window_content(d->m_preview_widget);

  insert(d->m_dock_window);
  insert(d->m_preview_window);

  d->m_preview_window->hide();
  d->m_navigation_dock->show();
}

void DockControllerImpl::session_data_available(
    const QuetzalKit::SyncObject &a_sesion_root) {}

void DockControllerImpl::submit_session_data(QuetzalKit::SyncObject *a_obj) {}

void DockControllerImpl::set_view_rect(const QRectF &rect) {
  if (!viewport()) {
    return;
  }

  d->m_dock_window->setPos(viewport()->center(d->m_dock_window->geometry(),
                                              QRectF(),
                                              Space::kCenterOnViewportLeft));

  d->m_preview_widget->set_view_geometry(
      QRectF(0.0, 0.0, 256, rect.height() - 24.0));

  d->m_preview_window->setGeometry(QRectF(0.0, 0.0, 256, rect.height()));

  d->m_preview_window->setPos(
      rect.x() + d->m_navigation_dock->frame_geometry().width() + 5,
      rect.y() + 24.0);

  d->m_preview_window->hide();

  if (d->m_action_activity) {
    d->m_action_activity->window()->setPos(rect.x(), rect.y());
  }
}

ActionList DockControllerImpl::actions() const {
  return d->m_supported_action_list;
}

void DockControllerImpl::request_action(const QString &actionName,
                                        const QVariantMap &args) {
  if (actionName.toLower() == "menu") {

    if (d->m_action_activity && d->m_action_activity->window()) {
      d->m_action_activity->window()->setPos(args["menu_pos"].toPoint());
      d->m_action_activity->window()->show();
    }

    return;
  } else if (actionName.toLower() == "show-dock") {
    d->m_navigation_dock->show();
    return;
  } else if (actionName.toLower() == "hide-dock") {
    d->m_navigation_dock->hide();
    return;
  } else if (actionName.toLower() == "show-expose") {
    if (d->m_preview_window->isVisible()) {
      d->m_preview_window->hide();
    } else {
      updatePreview();
      d->m_preview_window->show();
    }
  } else if (actionName.toLower() == "hide-expose") {
    d->m_preview_window->hide();
  }

  if (viewport() && viewport()->controller(args["controller"].toString())) {
    qDebug() << Q_FUNC_INFO << actionName;
    qDebug() << Q_FUNC_INFO << args;

    viewport()->controller(args["controller"].toString())->request_action(
        actionName, args);
  } else {
    qWarning() << Q_FUNC_INFO << "Unknown Action";
  }
}

QString DockControllerImpl::icon() const {
  return QString("pd_desktop_icon.png");
}

void DockControllerImpl::createActionForController(const QString &name,
                                                   const QPointF &pos) {
  if (!viewport()) {
    return;
  }

  viewport()->controller(name)->configure(pos);
}

void DockControllerImpl::createActivityForController(const QString &name) {
  if (!viewport()) {
    return;
  }
}

DesktopActivityPtr DockControllerImpl::createActivity(
    const QString &controllerName, const QString &activity,
    const QString &title, const QPoint &pos, const QVariantMap &dataItem) {
  QPoint _activity_location = pos;

  UIKit::DesktopActivityPtr _intent = viewport()->create_activity(
      activity, title, _activity_location,
      QRectF(0, _activity_location.y(), 484, 320), dataItem);
  _intent->set_controller(UIKit::ViewControllerPtr(this));
  _intent->set_activity_attribute("data", QVariant(dataItem));
  _intent->set_activity_attribute("auto_scale", QVariant(1));

  return _intent;
}

void DockControllerImpl::nextSpace() {
  if (this->viewport() && this->viewport()->workspace()) {
    UIKit::WorkSpace *_workspace =
        qobject_cast<UIKit::WorkSpace *>(viewport()->workspace());

    if (_workspace) {
      toggleDesktopPanel();
      _workspace->expose_next();
    }
  }
}

void DockControllerImpl::toggleSeamless() {
  if (!viewport()) {
    return;
  }

  UIKit::ViewControllerPtr controller =
      viewport()->controller("classicbackdrop");

  if (!controller) {
    qWarning() << Q_FUNC_INFO << "Controller Not Found";
    return;
  }

  controller->request_action("Seamless");
}

void DockControllerImpl::prepare_removal() {
  if (viewport() && viewport()->workspace()) {
    QGraphicsView *_workspace =
        qobject_cast<QGraphicsView *>(viewport()->workspace());

    if (_workspace) {
      d->m_action_activity.clear();
    } else {
      if (d->m_action_activity) {
        d->m_action_activity.clear();
      }
    }
  } else {
    qWarning() << Q_FUNC_INFO << "Error : Missing workspace or viewport";
  }
}

void DockControllerImpl::previousSpace() {
  if (this->viewport() && this->viewport()->workspace()) {
    UIKit::WorkSpace *_workspace =
        qobject_cast<UIKit::WorkSpace *>(viewport()->workspace());

    if (_workspace) {
      toggleDesktopPanel();
      _workspace->expose_previous();
    }
  }
}

void DockControllerImpl::toggleDesktopPanel() {
  if (this->viewport() && this->viewport()->workspace()) {
    UIKit::WorkSpace *_workspace =
        qobject_cast<UIKit::WorkSpace *>(viewport()->workspace());

    if (_workspace) {
      QRectF _work_area = viewport()->geometry();

      if (!d->m_main_panel_is_hidden) {
        _work_area.setX(331.0f);
        _work_area.setWidth(_work_area.width() + 330.0f);
      }

      d->m_main_panel_is_hidden = !d->m_main_panel_is_hidden;

      _workspace->expose_sub_region(_work_area);
    }
  }
}

void DockControllerImpl::loadControllerActions(const QString &name) {
  if (d->m_controller_name_list.contains(name) || !viewport()) {
    return;
  }

  UIKit::ViewControllerPtr controller = viewport()->controller(name);

  if (!controller) {
    return;
  }

  if (controller->actions().count() <= 0) {
    return;
  }

  QVariantMap _data;

  Q_FOREACH(QAction * action, controller->actions()) {
    if (!action) {
      continue;
    }

    QVariantMap _item;

    bool _is_hidden = action->property("hidden").toBool();

    if (_is_hidden) {
      continue;
    }

    _item["label"] = action->text();
    _item["icon"] = action->property("icon_name");
    _item["controller"] = QVariant(name);
    _item["id"] = action->property("id");

    _data[QString("%1.%2").arg(name).arg(action->text())] = _item;
  }

  if (_data.keys().count() <= 0) {
    return;
  }

  d->m_action_activity->update_attribute("data", _data);
}

void DockControllerImpl::onActivityAnimationFinished() {
  if (!sender()) {
    return;
  }

  UIKit::DesktopActivity *activity =
      qobject_cast<UIKit::DesktopActivity *>(sender());

  if (!activity) {
    return;
  }

  UIKit::Widget *_activity_widget =
      qobject_cast<UIKit::Widget *>(activity->window());

  if (_activity_widget) {
    _activity_widget->set_widget_flag(UIKit::Widget::kRenderDropShadow, false);
    _activity_widget->set_widget_flag(UIKit::Widget::kConvertToWindowType,
                                      false);
    _activity_widget->set_widget_flag(UIKit::Widget::kRenderBackground, true);
    _activity_widget->setFlag(QGraphicsItem::ItemIsMovable, false);
    _activity_widget->setPos(QPoint());
  }
}

void DockControllerImpl::onActivityFinished() {
  UIKit::DesktopActivity *_activity =
      qobject_cast<UIKit::DesktopActivity *>(sender());

  if (!_activity) {
    return;
  }

  UIKit::ViewControllerPtr _controller =
      viewport()->controller(_activity->result()["controller"].toString());

  if (!_controller) {
    return;
  }

  _controller->request_action(_activity->result()["action"].toString(),
                              QVariantMap());
}

void DockControllerImpl::removeSpace() {
  if (this->viewport() && this->viewport()->workspace()) {
    UIKit::WorkSpace *_workspace =
        qobject_cast<UIKit::WorkSpace *>(viewport()->workspace());
    if (_workspace) {
      _workspace->remove(qobject_cast<UIKit::Space *>(viewport()));
    }
  }
}

void DockControllerImpl::onNavigationPanelClicked(const QString &action) {
  if (action == tr("Close")) {
      removeSpace();
      return;
    } else if (action == tr("Up")) {
      this->previousSpace();
    } else if (action == tr("Down")) {
      this->nextSpace();
    } else if (action == tr("Seamless")) {
      this->toggleSeamless();
    } else if (action == tr("Expose")) {
      if (d->m_preview_window->isVisible()) {
          d->m_preview_window->hide();
          if (d->m_preview_widget)
            d->m_preview_widget->clear();
        } else {
          updatePreview();
          d->m_preview_window->show();
        }
      return;
    } else if (action == tr("Menu")) {
      if (!viewport() || !viewport()->workspace()) {
          return;
      }

      QPointF _menu_pos =
          viewport()->center(d->m_action_activity->window()->boundingRect(),
                             QRectF(), UIKit::Space::kCenterOnViewportLeft);
      _menu_pos.setX(d->m_navigation_dock->frame_geometry().width() + 5);

      if (d->m_action_activity && d->m_action_activity->window()) {
          d->m_action_activity->window()->setPos(_menu_pos);
          d->m_action_activity->window()->show();
        }

    } else if (action == tr("Add")) {
      onAddSpaceButtonClicked();
    }
}

void DockControllerImpl::onAddSpaceButtonClicked() {
  if (this->viewport() && this->viewport()->workspace()) {
    UIKit::WorkSpace *_workspace =
        qobject_cast<UIKit::WorkSpace *>(viewport()->workspace());

    if (_workspace) {
      _workspace->add_default_space();
    }
  }
}

QAction *DockControllerImpl::createAction(int id, const QString &action_name,
                                          const QString &icon_name) {
  QAction *_add_clock_action = new QAction(this);
  _add_clock_action->setText(action_name);
  _add_clock_action->setProperty("id", QVariant(id));
  _add_clock_action->setProperty("icon_name", icon_name);
  _add_clock_action->setProperty("hidden", 1);

  return _add_clock_action;
}

void DockControllerImpl::updatePreview() {
  d->m_preview_widget->clear();

  if (this->viewport() && this->viewport()->workspace()) {
    UIKit::WorkSpace *_workspace =
        qobject_cast<UIKit::WorkSpace *>(viewport()->workspace());

    float lHeight = 10;
    float lWidth = 0;

    if (_workspace) {
        foreach(UIKit::Space * _space, _workspace->current_spaces()) {
            QPixmap _preview = _workspace->thumbnail(_space);

            UIKit::ImageView *p = new UIKit::ImageView();

            p->setMinimumSize(_preview.size());
            p->set_pixmap(_preview);
            lHeight += _preview.size().height();
            lWidth = _preview.size().width();

            UIKit::ModelViewItem *model_item = new UIKit::ModelViewItem();
            model_item->set_view(p);
            model_item->on_view_removed([=](UIKit::ModelViewItem *a_item) {
                if (a_item && a_item->view()) {
                    UIKit::Widget *view = a_item->view();
                    if (view)
                      delete view;
                  }
            });

            d->m_preview_widget->insert(model_item);
          }
      }

    QPointF lMenuPos =
        viewport()->center(d->m_action_activity->window()->boundingRect(),
                           QRectF(), UIKit::Space::kCenterOnViewportLeft);

    lMenuPos.setX(d->m_navigation_dock->geometry().width() + 5.0);
    d->m_preview_window->setGeometry(
        QRectF(lMenuPos.x(), lMenuPos.y(), lWidth, lHeight + 24));
  }
}

void DockControllerImpl::onPreviewItemClicked(TableViewItem *item) {
  DefaultTableComponent *_item = qobject_cast<DefaultTableComponent *>(item);

  if (_item) {
    bool _ok = false;
    int _space_id = _item->label().toUInt(&_ok);

    if (_ok) {
      if (this->viewport() && this->viewport()->workspace()) {
        UIKit::WorkSpace *_workspace =
            qobject_cast<UIKit::WorkSpace *>(viewport()->workspace());

        if (_workspace) {
          _workspace->expose(_space_id);
        }
      }
    }
  }
}
