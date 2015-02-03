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
#include "datepicker.h"
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>
#include <tableview.h>
#include <default_table_model.h>
#include <themepackloader.h>

#include "calendarwidget.h"
#include "datecellfactory.h"

class DatePickerActivity::PrivateDatePicker {
public:
  PrivateDatePicker() {}
  ~PrivateDatePicker() {}

  UI::UIWidget *mFrame;
  DateCellFactory *mFactory;
  UI::TableView *mTable;
  QRectF mBoundingRect;
  QString mSelection;
  UI::Theme *mLoader;

  QVariantMap m_result_data;

  CalendarWidget *mCalendarWidget;
};

DatePickerActivity::DatePickerActivity(QGraphicsObject *object)
    : UI::DesktopActivity(object), d(new PrivateDatePicker) {}

DatePickerActivity::~DatePickerActivity() { delete d; }

void DatePickerActivity::createWindow(const QRectF &window_geometry,
                                      const QString &window_title,
                                      const QPointF &window_pos) {
  d->mFrame = new UI::UIWidget();

  updateContentGeometry(d->mFrame);
  setGeometry(window_geometry);

  d->mFrame->setWindowFlag(UI::UIWidget::kRenderBackground, false);
  d->mFrame->setWindowFlag(UI::UIWidget::kTopLevelWindow);
  d->mFrame->setWindowFlag(UI::UIWidget::kConvertToWindowType);
  d->mFrame->setWindowFlag(UI::UIWidget::kRenderWindowTitle, false);
  d->mFrame->setWindowFlag(UI::UIWidget::kRenderDropShadow);

  d->mCalendarWidget = new CalendarWidget(d->mFrame);

  d->mCalendarWidget->setPos(0.0, 0.0);

  d->mFrame->setWindowTitle(window_title);

  exec(window_pos);

  showActivity();

  d->mLoader = new UI::Theme("default", this);

  connect(d->mLoader, SIGNAL(imageSearchDone(QImage)), this,
          SLOT(onImageReady(QImage)));

  d->mFrame->updateWindowButton(d->mCalendarWidget->zValue());

  connect(d->mFrame, SIGNAL(closed(UI::UIWidget *)), this,
          SLOT(onWidgetClosed(UI::UIWidget *)));
  connect(d->mCalendarWidget, SIGNAL(done()), this, SLOT(onCalendarReady()));
}

QVariantMap DatePickerActivity::result() const { return d->m_result_data; }

UI::UIWidget *DatePickerActivity::window() const { return d->mFrame; }

void DatePickerActivity::onWidgetClosed(UI::UIWidget *widget) {
  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discardActivity();
}

void DatePickerActivity::onHideAnimationFinished() { Q_EMIT finished(); }

void DatePickerActivity::onImageReady(const QImage &img) {
  d->mCalendarWidget->setBackgroundImage(img);
}

void DatePickerActivity::onCalendarReady() {
  if (!d->mCalendarWidget) {
    d->m_result_data["date"] = QVariant(QDate::currentDate().toString());
    d->m_result_data["hour"] = QVariant(0);
    d->m_result_data["minute"] = QVariant(0);
  } else {
    d->m_result_data["date"] = QVariant(d->mCalendarWidget->currentDate());
    d->m_result_data["hour"] = QVariant(d->mCalendarWidget->currentHour());
    d->m_result_data["minute"] = QVariant(d->mCalendarWidget->currentMinute());
  }

  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discardActivity();
}
