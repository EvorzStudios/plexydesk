/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QScroller>
#include <QDebug>
#include <QScroller>
#include <QGraphicsSceneWheelEvent>

#include "scrollwidget.h"

namespace UIKit {

class ScrollWidget::Private {

public:
  Private() {}
  ~Private() {}

  QSizeF m_size;
  QGraphicsObject *m_viewport;
};

ScrollWidget::ScrollWidget(const QRectF &a_rect, QGraphicsObject *parent)
    : Widget(parent), d(new Private) {
  d->m_viewport = 0;
  QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);
  d->m_size = a_rect.size();
}

ScrollWidget::~ScrollWidget() { delete d; }

void ScrollWidget::set_viewport(QGraphicsObject *a_widget_ptr) {
  if (!a_widget_ptr) {
    return;
  }

  if (a_widget_ptr == d->m_viewport) {
    return;
  }

  if (d->m_viewport != 0) {
    d->m_viewport->setParentItem(0);
    delete d->m_viewport;
  }

  if (a_widget_ptr) {
    a_widget_ptr->setParentItem(this);
    d->m_viewport = a_widget_ptr;
  }
}

void ScrollWidget::scroll_by(int x, int y) {
  if (d->m_viewport) {
    // resetric to viewport
    int y_pos = d->m_viewport->y() + y;
    int view_height = this->boundingRect().height();
    int y_max = (-1) * (d->m_viewport->boundingRect().height() - view_height);

    if (y_pos < (this->y()) && y_pos > y_max) {
      d->m_viewport->setPos(d->m_viewport->x() + x, d->m_viewport->y() + y);
    } else if (!(y_pos < (this->y()))) {
      d->m_viewport->setPos(d->m_viewport->x() + x, this->y());
    }
  }
}

void ScrollWidget::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {}

void ScrollWidget::wheelEvent(QGraphicsSceneWheelEvent *a_event_ptr) {
  if (a_event_ptr->delta() < 0) {
    scroll_by(0, -10);
  } else {
    scroll_by(0, 10);
  }
  a_event_ptr->accept();

  //?
  Widget::wheelEvent(a_event_ptr);
}

void ScrollWidget::dragMoveEvent(QGraphicsSceneDragDropEvent *a_event_ptr) {
  Widget::dragMoveEvent(a_event_ptr);
}

bool ScrollWidget::event(QEvent *a_event_ptr) {
  if (!d->m_viewport) {
    return QGraphicsObject::event(a_event_ptr);
  }

  switch (a_event_ptr->type()) {
    case QScrollPrepareEvent::ScrollPrepare: {
      qDebug() << Q_FUNC_INFO << "Prepare :";
      QScrollPrepareEvent *se = static_cast<QScrollPrepareEvent *>(a_event_ptr);
      se->setViewportSize(d->m_size);
      QRectF br = d->m_viewport->boundingRect();
      se->setContentPosRange(
          QRectF(0, 0, qMax(qreal(0), br.width() - d->m_size.width()),
                 qMax(qreal(0), br.height() - d->m_size.height())));
      se->setContentPos(-d->m_viewport->pos());
      se->accept();
      return QGraphicsObject::event(a_event_ptr);
    }
    case QScrollEvent::Scroll: {
      qDebug() << Q_FUNC_INFO << "Scroll";
      QScrollEvent *se = static_cast<QScrollEvent *>(a_event_ptr);
      d->m_viewport->setPos(-se->contentPos() - se->overshootDistance());
      return true;
    }

    default:
      break;
  }
  return QGraphicsObject::event(a_event_ptr);
}

bool ScrollWidget::sceneEvent(QEvent *a_e_ptr) {
  /*
  switch (e->type()) {
  case QEvent::TouchBegin: {
      // We need to return true for the TouchBegin here in the
      // top-most graphics object - otherwise gestures in our parent
      // objects will NOT work at all (the accept() flag is already
      // set due to our setAcceptTouchEvents(true) call in the c'tor
      return true;

  }
  case QEvent::GraphicsSceneMousePress: {
      // We need to return true for the MousePress here in the
      // top-most graphics object - otherwise gestures in our parent
      // objects will NOT work at all (the accept() flag is already
      // set to true by Qt)
      return true;

  }
  default:
      break;
  }
  */
  return QGraphicsObject::sceneEvent(a_e_ptr);
}

} // namespace PlexyDesk
