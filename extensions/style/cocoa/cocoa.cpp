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
#include "cocoa.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>
#include <stylefeatures.h>

//for the clock
int angle_between_hands(double h, double m)
{
    // validate the input
    if (h <0 || m < 0 || h >12 || m > 60)
        printf("Wrong input");

    if (h == 12) h = 0;
    if (m == 60) m = 0;

    // Calculate the angles moved by hour and minute hands
    // with reference to 12:00
    int hour_angle = 0.5 * (h*60 + m);
    int minute_angle = 6*m;

    // Find the difference between two angles
    int angle = abs(hour_angle - minute_angle);

    // Return the smaller angle of two possible angles
    angle = std::min(360 - angle, angle);

    return angle;
}

class CocoaStyle::PrivateCocoa {
public:
  PrivateCocoa() {}
  ~PrivateCocoa() {}

  QHash<QString, int> m_type_map;
  QVariantMap m_attribute_map;
  QVariantMap m_color_map;
};

void CocoaStyle::load_default_widget_style_properties()
{
  d->m_type_map["button"] = 1;
  d->m_type_map["vertical_list_item"] = 2;
  d->m_type_map["window_button"] = 3;
  d->m_type_map["seperator"] = 4;
  d->m_type_map["text_edit"] = 5;
  d->m_type_map["line_edit"] = 6;
  d->m_type_map["image_view"] = 7;
  d->m_type_map["image_frame"] = 8;
  d->m_type_map["label"] = 9;
  d->m_type_map["clock"] = 10;
  d->m_type_map["cell"] = 11;
  d->m_type_map["dial"] = 12;
  d->m_type_map["knob"] = 13;
  d->m_type_map["tool_button"] = 14;
  d->m_type_map["tool_bar"] = 15;
  d->m_type_map["dock_background"] = 16;
  d->m_type_map["desktop_background"] = 17;
  d->m_type_map["desktop_side_panel"] = 18;
  d->m_type_map["linear_progress_bar"] = 19;
  d->m_type_map["circular_progress_bar"] = 20;
  d->m_type_map["window_frame"] = 21;
  d->m_type_map["window_title"] = 22;
  d->m_type_map["window_resize_handle"] = 23;

  // style attributes. this could be read from a xml file or a stylesheet.
  QVariantMap _frame_attributes;
  QVariantMap _widget_attributes;
  QVariantMap _size_attributes;
  QVariantMap _button_attributes;
  QVariantMap _vlist_item_attributes;
  QVariantMap lLabelAttributesMap;

  _frame_attributes["window_title_height"] = 64.0;
  _frame_attributes["window_minimized_height"] = 128.0;
  _frame_attributes["window_minimized_width"] = 96.0;
  _frame_attributes["window_blur_radius"] = 24.0;
  _frame_attributes["window_close_button_width"] = 16.0;
  _frame_attributes["window_close_button_height"] = 16.0;
  _frame_attributes["window_shadow_color"] = QString("#323232");
  _frame_attributes["padding"] = 5.0;

  _widget_attributes["line_edit_width"] = 256.0;
  _widget_attributes["line_edit_height"] = 32.0;

  _size_attributes["small"] = 0.5;
  _size_attributes["original"] = 1.0;
  _size_attributes["large"] = 2.0;
  _size_attributes["huge"] = 4.0;

  _button_attributes["width"] = 128.0;
  _button_attributes["height"] = 32.0;

  d->m_attribute_map["frame"] = _frame_attributes;
  d->m_attribute_map["widget"] = _widget_attributes;
  d->m_attribute_map["size"] = _size_attributes;

}

CocoaStyle::CocoaStyle() : d(new PrivateCocoa) {
  load_default_widget_style_properties();

  d->m_color_map["primary_forground"] = "#646464";
  d->m_color_map["primary_background"] = "#ffffff";
  d->m_color_map["primary_highlight"] = "#646464";

  d->m_color_map["base_forground"] = "#E6E6E6";
  d->m_color_map["base_background"] = "#ffffff";
  d->m_color_map["base_highlight"] = "#111111";

  d->m_color_map["soft_forground"] = "#191919";
  d->m_color_map["soft_background"] = "#ffffff";
  d->m_color_map["soft_highlight"] = "#111111";

  d->m_color_map["accent_primary_forground"] = "#F0F0F0";
  d->m_color_map["accent_primary_background"] = "#0092CC";
  d->m_color_map["accent_primary_highlight"] = "#FF3333";

  d->m_color_map["accent_base_forground"] = "#E6E6E6";
  d->m_color_map["accent_base_background"] = "#087099";
  d->m_color_map["accent_base_highlight"] = "#CC3333";

  d->m_color_map["accent_soft_forground"] = "#779933";
  d->m_color_map["accent_soft_background"] = "#5C7829";
  d->m_color_map["accent_soft_highlight"] = "#B7B327";
}

CocoaStyle::~CocoaStyle() { delete d; }

QVariantMap CocoaStyle::attribute_map(const QString &type) const {
  return d->m_attribute_map[type].toMap();
}

QVariantMap CocoaStyle::color_scheme_map() const {
  return d->m_color_map;
}

void CocoaStyle::draw(const QString &type, const StyleFeatures &options,
                      QPainter *painter, const Widget *aWidget) {
  switch (d->m_type_map[type]) {
    case 1:
      drawPushButton(options, painter);
      break;
    case 2:
      drawVListItem(options, painter);
      break;
    case 3:
      drawWindowButton(options, painter);
      break;
    case 6:
      drawLineEdit(options, painter);
      break;
    case 9:
      drawLabel(options, painter, aWidget);
      break;
    case 10:
      drawClock(options, painter);
      break;
    case 19:
      drawProgressBar(options, painter);
      break;
    case 21:
      drawFrame(options, painter);
      break;
    default:
      qWarning() << Q_FUNC_INFO << "Unknown Element:" << type;
  }
}

void CocoaStyle::drawPushButton(const StyleFeatures &features,
                                QPainter *painter) {
  QRectF rect = features.geometry;

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(rect, 0.0, 0.0);

  if (features.render_state == StyleFeatures::kRenderPressed) {
    painter->fillPath(backgroundPath, QColor(
                        color("primary_background")));
    QPen pen(QColor(color("primary_forground")),
             1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawPath(backgroundPath);
  } else if (features.render_state == StyleFeatures::kRenderRaised) {
    painter->fillPath(backgroundPath, QColor(
                        color("soft_background")));
    QPen pen(QColor(color("soft_forground")),
             1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawPath(backgroundPath);
  } else {
    painter->fillPath(backgroundPath, QColor(color("base_background")));
    QPen pen(QColor(color("base_forground")),
             1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawPath(backgroundPath);
  }

  painter->drawText(features.geometry, Qt::AlignCenter, features.text_data);
}

void CocoaStyle::drawWindowButton(const StyleFeatures &features,
                                  QPainter *painter) {
  QRectF rect = features.geometry.adjusted(0, 0, 0, 0);

  painter->save();
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setRenderHint(QPainter::HighQualityAntialiasing);

  QPainterPath background;
  background.addRoundedRect(rect, 4.0, 4.0);

  if (features.render_state == StyleFeatures::kRenderElement) {
    painter->fillPath(background, QColor(
                        color("accent_base_highlight")));
  } else {
    painter->fillPath(background, QColor(
                        color("accent_base_highlight")));
  }

  painter->save();
  QPen white_pen(QColor(
                   color("accent_primary_forground")),
                 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(white_pen);
  QRectF cross_rect(6.0, 6.0, rect.width() - 12, rect.height() - 12);

  // painter->drawRect(cross_rect);
  painter->drawLine(cross_rect.topLeft(), cross_rect.bottomRight());
  painter->drawLine(cross_rect.topRight(), cross_rect.bottomLeft());

  painter->restore();
  painter->restore();
}

void CocoaStyle::drawFrame(const StyleFeatures &features, QPainter *painter) {
  QRectF rect = features.geometry;

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(rect, 4.0, 4.0);

  painter->fillPath(backgroundPath, QColor(color("base_background")));

  // draw seperator
  if (!features.text_data.isNull() || !features.text_data.isEmpty()) {
    QRectF _window_title_rect(0.0, 0.0, features.geometry.width(), 54.0);

    QLinearGradient _seperator_line_grad(_window_title_rect.bottomLeft(),
                                         _window_title_rect.bottomRight());
    _seperator_line_grad.setColorAt(0.0, QColor(color("primary_background")));
    _seperator_line_grad.setColorAt(0.5, QColor(color("accent_primary_forground")));
    _seperator_line_grad.setColorAt(1.0, QColor(color("primary_background")));

    QPen linePen = QPen(_seperator_line_grad, 1, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin);

    // draw frame text;
    painter->save();
    QFont _font = painter->font();
    _font.setBold(true);
    _font.setPointSize(18 * scaleFactor());
    painter->setFont(_font);
    QTextOption option;
    option.setAlignment(Qt::AlignCenter);

    QPen _title_font_pen =
        QPen(QColor(color("primary_forground")),
             1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(_title_font_pen);

    painter->drawText(_window_title_rect, features.text_data, option);

    painter->restore();

    painter->save();
    painter->setPen(linePen);
    painter->drawLine(_window_title_rect.bottomLeft(),
                      _window_title_rect.bottomRight());
    /*
    painter->fillRect(_window_title_rect,
                      QColor(color("accent_primary_background")));
                      */
    painter->restore();
  } else {
    // painter->fillRect(QRectF(0.0, 0.0, rect.width(), 10), QColor("#F0F0F0"));
  }
  painter->restore();
}

void CocoaStyle::draw_clock_hands(QPainter *p, QRectF rect, int factor,
                                   float angle, QColor hand_color,
                                   int thikness) {
  p->save();
  float _adjustment = rect.width() / factor;

  QRectF _clock_hour_rect(rect.x() + _adjustment, rect.y() + _adjustment,
                          rect.width() - (_adjustment * 2),
                          rect.height() - (_adjustment * 2));

  QTransform _xform_hour;
  QPointF _transPos = _clock_hour_rect.center();
  _xform_hour.translate(_transPos.x(), _transPos.y());
  _xform_hour.rotate(angle);
  _xform_hour.translate(-_transPos.x(), -_transPos.y());
  p->setTransform(_xform_hour);

  QPen _clock_hour_pen(hand_color, thikness, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin);
  p->setPen(_clock_hour_pen);

  // p->drawRect(_clock_hour_rect);
  p->drawLine(_clock_hour_rect.topLeft(), _clock_hour_rect.center());
  p->restore();
}

void CocoaStyle::draw_timer_marker(QRectF rect,
                QTransform _xform_hour,
                QPainter *p,
                double mark_minutes,
                double mark_hour,
                QPen current_dot_min_pen,
                QPointF current_marker_location,
                QPointF _transPos,
                QPointF current_marker_location_for_min)
{
  double hour_angle = (((60.0 * mark_hour) + mark_minutes) / 2);
  double min_angle =  (6.0 * mark_minutes);

  QPainterPath clock_path;
  clock_path.addEllipse(rect);

  signed int multiply = 1;
  if (std::abs(min_angle) > std::abs(hour_angle)) {
      multiply = - multiply;
    }

  p->save();
  p->setPen(current_dot_min_pen);

  _xform_hour.reset();

  _transPos = rect.center();
  _xform_hour.translate(_transPos.x(), _transPos.y());
  _xform_hour.rotate(-90);
  _xform_hour.translate(-_transPos.x(), -_transPos.y());

  p->setTransform(_xform_hour);

  QPen current_timer_pen(QColor(color("accent_soft_highlight")), 2,
                             Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  p->setPen(current_timer_pen);

  QPainterPath timer_path;

  timer_path.moveTo(rect.center());
  timer_path.arcTo(rect, -hour_angle,
                   multiply * angle_between_hands(mark_hour, mark_minutes));

  /*
        timer_path.moveTo(current_marker_location);
        timer_path.lineTo(rect.center());
        timer_path.lineTo(current_marker_location_for_min);

        p->fillPath(timer_path, QColor("#f0f0f0"));
   */

  p->setOpacity(0.3);
  p->fillPath(timer_path, QColor(color("primary_background")));
  p->restore();
}

void CocoaStyle::drawClock(const StyleFeatures &features, QPainter *p)
{
  /* please note that the clock is drawn with the inverted color scheme */

  QRectF rect = features.geometry;
  double second_value = features.attributes["seconds"].toDouble();
  double minutes_value = features.attributes["minutes"].toDouble();
  double hour_value = features.attributes["hour"].toDouble();
  double mark_hour = features.attributes["mark_hour"].toDouble();
  double mark_minutes = features.attributes["mark_minutes"].toDouble();

  p->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing |
                    QPainter::HighQualityAntialiasing);

  QPen _clock_frame_pen(QColor(color("primary_forground")),
                        18, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  p->setPen(_clock_frame_pen);

  QPainterPath _clock_background;
  _clock_background.addEllipse(rect);

  p->fillPath(_clock_background, QColor(color("primary_forground")));
  p->drawEllipse(rect);

  //draw second markers.
  for (int i = 0; i < 60; i++) {
      double percent = (i / 60.0);
      QPointF marker_location = _clock_background.pointAtPercent(percent);

      p->save();

      int thikness = 1;
      QPen dot_frame_pen(QColor(color("primary_background")),
                         thikness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      p->setPen(dot_frame_pen);
      p->drawPoint(marker_location);

      p->restore();
  }

  //draw minute markers
  for (int i = 0; i < 360; i = i + 30) {
      float percent = (i / 360.0);
      QPointF marker_location = _clock_background.pointAtPercent(percent);

      p->save();
      QPen dot_frame_pen(QColor(color("primary_background")),
                         4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      p->setPen(dot_frame_pen);

      p->drawPoint(marker_location);
      p->restore();
  }

  //draw hour markers
  for (int i = 0; i < 360; i = i + 90) {
      float percent = (i / 360.0);
      QPointF marker_location = _clock_background.pointAtPercent(percent);

      p->save();
      QPen dot_frame_pen(QColor(color("primary_background")),
                         8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      p->setPen(dot_frame_pen);

      p->drawPoint(marker_location);
      p->restore();
  }

  //draw current hour
  if (std::abs(mark_hour) > 0) {
      double current_percent = (mark_hour) / 12.0;

      p->save();

      QTransform _xform_hour;
      QPointF _transPos = rect.center();
      _xform_hour.translate(_transPos.x(), _transPos.y());
      _xform_hour.rotate(-90);
      _xform_hour.translate(-_transPos.x(), -_transPos.y());
      p->setTransform(_xform_hour);

      QPointF current_marker_location =
          _clock_background.pointAtPercent(current_percent);

      QPen current_dot_frame_pen(QColor(color("accent_soft_highlight")), 16,
                                 Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      p->setPen(current_dot_frame_pen);
                        p->setOpacity(0.9);
      p->drawPoint(current_marker_location);

      p->restore();

      //draw current second
      double current_percent_min = (mark_minutes) / 60.0;

      p->save();

      _xform_hour.reset();
      _transPos = rect.center();
      _xform_hour.translate(_transPos.x(), _transPos.y());
      _xform_hour.rotate(-90);
      _xform_hour.translate(-_transPos.x(), -_transPos.y());
      p->setTransform(_xform_hour);

      QPointF current_marker_location_for_min =
          _clock_background.pointAtPercent(current_percent_min);

      QPen current_dot_min_pen(QColor(color("accent_soft_highlight")), 12,
                               Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      p->setPen(current_dot_min_pen);
                        p->setOpacity(0.9);
      p->drawPoint(current_marker_location_for_min);

      p->restore();

      //experiment
      draw_timer_marker(rect, _xform_hour, p, mark_minutes, mark_hour,
                        current_dot_min_pen, current_marker_location, _transPos,
                        current_marker_location_for_min);
    }

  /* Draw Hour Hand */
  draw_clock_hands(p, rect, 3, 45.0 + hour_value,
                   QColor(color("primary_background")), 6);
  draw_clock_hands(p, rect, 4, 45.0 + minutes_value,
                   QColor(color("primary_background")), 3);

  QRectF _clock_wheel_rect(rect.center().x() - 8, rect.center().y() - 8, 16, 16);
  QRectF _clock_wheel_inner_rect(rect.center().x() - 4,
                                 rect.center().y() - 4, 8, 8);

  QPainterPath _clock_wheel_path;
  QPainterPath _clock_wheel_inner_path;

  _clock_wheel_path.addEllipse(_clock_wheel_rect);
  _clock_wheel_inner_path.addEllipse(_clock_wheel_inner_rect);

  p->fillPath(_clock_wheel_path, QColor(color("primary_background")));

  QPen wheel_border_pen(QColor(color("base_forground")),
                         1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  p->save();
  p->setPen(wheel_border_pen);
  p->drawPath(_clock_wheel_path);
  p->restore();

  p->fillPath(_clock_wheel_inner_path, QColor(color("accent_primary_highlight")));

  draw_clock_hands(p, rect, 5, 45.0 + second_value,
                   QColor(color("accent_primary_highlight")), 2);


 }

void CocoaStyle::drawPushButtonText(const StyleFeatures &features,
                                    const QString &text, QPainter *painter) {
  /* Painter settings */
  QPen pen;

  pen = QPen(QColor(255, 255, 255), 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
  painter->setPen(pen);
  painter->drawText(features.geometry, Qt::AlignCenter, text);
  painter->restore();
}

void CocoaStyle::drawLineEdit(const StyleFeatures &features,
                              QPainter *painter) {
  QRectF rect = features.geometry.adjusted(1, 1, 0, 0);

  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  QPainterPath backgroundPath;
  backgroundPath.addRoundedRect(rect, 6, 0);

  QLinearGradient linearGrad(QPointF(0, 0), QPointF(0.0, rect.height()));

  linearGrad.setColorAt(0, QColor(189, 191, 196));
  linearGrad.setColorAt(1, QColor(255, 255, 255));

  painter->fillPath(backgroundPath, QColor(color("primary_background")));

  QPen pen;
  if (features.render_state == StyleFeatures::kRenderRaised) {
    pen =
        QPen(QColor(color("accent_base_forground")),
             1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  } else {
    pen =
        QPen(QColor(color("accent_primary_forground")),
             1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  }
  painter->setPen(pen);
  painter->drawPath(backgroundPath);

  painter->save();
  pen.setColor(QColor(color("primary_forground")));
  painter->setPen(pen);
  painter->drawText(features.geometry.adjusted(10.0, 0.0, 0.0, 0.0),
                    Qt::AlignLeft | Qt::AlignVCenter, features.text_data);
  painter->restore();
}

void CocoaStyle::drawLineEditText(const StyleFeatures &features,
                                  const QString &text, QPainter *painter) {
  /* Painter settings */
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
  QPen pen;

  pen = QPen(QColor(255, 255, 255), 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
  QFont font = QFont("", 18 * scaleFactor());
  //QFontMetrics fontMetrics(font);
  // int width = fontMetrics.width(text.left(features.cursorLocation));
  painter->setFont(font);

  painter->setPen(pen);
  painter->drawText(features.geometry.adjusted(11.0, 1.0, 1.0, 1.0),
                    Qt::AlignLeft | Qt::AlignVCenter, text);

  pen = QPen(QColor(0, 0, 0), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawText(features.geometry.adjusted(10.0, 0.0, 0.0, 0.0),
                    Qt::AlignLeft | Qt::AlignVCenter, text);

  pen =
      QPen(QColor(98, 101, 108), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  // painter->drawLine(QPoint(width + 11, 4), QPoint(width +
  // 11,features.geometry.height() - 4));
}

void CocoaStyle::drawLabelEditText(const StyleFeatures &features,
                                   const QString &text, QPainter *painter) {
  /*
  QPen pen;

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  painter->setFont(features.font);

  pen = QPen(QColor (88, 88, 88), 1, Qt::SolidLine, Qt::RoundCap,
  Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawText(features.geometry.adjusted(1, 1, 1, 1), features.fontFlags,
  text);

  painter->setOpacity(0.8);

  pen = QPen(features.fontColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawText(features.geometry, features.fontFlags, text);
  */
}

void CocoaStyle::drawSeperatorLine(const StyleFeatures &features,
                                   QPainter *painter) {
  painter->save();
  QPen pen = QPen(QColor(217, 217, 217), 1, Qt::SolidLine, Qt::RoundCap,
                  Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawLine(
      QPoint(features.geometry.x(), features.geometry.y()),
      QPoint(features.geometry.width(), features.geometry.height()));
  pen = QPen(QColor(255, 255, 255), 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
  painter->setPen(pen);
  painter->drawLine(
      QPoint(features.geometry.x() + 1, features.geometry.y() + 1),
      QPoint(features.geometry.width() + 1, features.geometry.height() + 1));
  painter->restore();
}

void CocoaStyle::drawProgressBar(const StyleFeatures &features,
                                 QPainter *painter) {
  painter->save();

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);
  painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

  switch (features.render_state) {
    case StyleFeatures::kRenderBackground: {
      QRectF rect = features.geometry;
      QLinearGradient linearGrad(QPointF(0, 0), QPointF(0.0, 20));
      linearGrad.setColorAt(1, QColor(color("primary_background")));
      linearGrad.setColorAt(0.5, QColor(color("primary_forground")));
      linearGrad.setColorAt(0, QColor(color("primary_background")));
      QPen backgroundPen(linearGrad, 4, Qt::SolidLine, Qt::RoundCap,
                         Qt::RoundJoin);

      QPointF backgroundLineStart(rect.topLeft().x(),
                                  rect.bottomLeft().y() / 2);
      QPointF backgroundLineEnd(rect.topRight().x(),
                                rect.bottomRight().y() / 2);
      painter->setPen(backgroundPen);
      painter->drawLine(backgroundLineStart, backgroundLineEnd);
    } break;
    case StyleFeatures::kRenderForground: {
      QRectF rect = features.geometry;
      QLinearGradient linearGrad(QPointF(0, 0), QPointF(0.0, 20));
      linearGrad.setColorAt(1, QColor(color("accent_soft_highlight")));
      linearGrad.setColorAt(0, QColor(color("accent_soft_forground")));
      QPen backgroundPen(linearGrad, 4, Qt::SolidLine, Qt::RoundCap,
                         Qt::RoundJoin);

      QPointF backgroundLineStart(rect.topLeft().x(),
                                  rect.bottomLeft().y() / 2);
      QPointF backgroundLineEnd(rect.topRight().x(),
                                rect.bottomRight().y() / 2);
      painter->setPen(backgroundPen);
      painter->drawLine(backgroundLineStart, backgroundLineEnd);
    } break;
    default:
      qDebug() << Q_FUNC_INFO << "Unknown progress bar state";
  }

  painter->restore();
}

void CocoaStyle::drawVListItem(const StyleFeatures &features,
                               QPainter *painter) {
  QRectF rect = features.geometry;

  QPen backgroundPen(QColor(color("primary_forground")),
                     1, Qt::SolidLine, Qt::RoundCap,
                     Qt::RoundJoin);

  painter->save();
  if (features.render_state == StyleFeatures::kRenderElement) {
    painter->setPen(backgroundPen);
    painter->fillRect(rect, QColor(color("primary_background")));

    QLinearGradient _seperator_line_grad(rect.bottomLeft(), rect.bottomRight());
    _seperator_line_grad.setColorAt(0.0, QColor(
                                      color("primary_background")));
    _seperator_line_grad.setColorAt(0.5,
                                    QColor(color("accent_primary_forground")));
    _seperator_line_grad.setColorAt(1.0,
                                    QColor(color("primary_background")));

    QPen linePen = QPen(_seperator_line_grad, 1, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin);
    painter->setPen(linePen);
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());
  }
  painter->restore();
}

void CocoaStyle::drawLabel(const StyleFeatures &aFeatures,
                           QPainter *aPainterPtr, const Widget *aWidget) {
  aPainterPtr->save();
  aPainterPtr->fillRect(aFeatures.geometry, QColor(color("primary_background")));
  aPainterPtr->drawText(aFeatures.geometry, aWidget->label(),
                        QTextOption(Qt::AlignHCenter | Qt::AlignLeft));
  aPainterPtr->restore();
}
