#include "imagebutton.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QPropertyAnimation>
#include <QAbstractAnimation>
#include <themepackloader.h>

#include <QDebug>

namespace UI
{

class ImageButton::PrivateImageButton
{
public:
  PrivateImageButton() {}
  ~PrivateImageButton()
  {
    if (mZoomAnimation) {
      delete mZoomAnimation;
    }
  }

  QPixmap mPixmap;

  QPropertyAnimation *mZoomAnimation;
  QString mLabel;
  QColor mBgColor;
};

void ImageButton::createZoomAnimation()
{
  if (!d->mZoomAnimation) {
    return;
  }

  d->mZoomAnimation->setDuration(100);
  d->mZoomAnimation->setStartValue(1.0);
  d->mZoomAnimation->setEndValue(1.1);
}

void ImageButton::setBackgroundColor(const QColor &color)
{
  d->mBgColor = color;
  update();
}

StylePtr ImageButton::style() const
{
  return Theme::instance()->defaultDesktopStyle();
}

ImageButton::ImageButton(QGraphicsObject *parent)
  : Widget(parent), d(new PrivateImageButton)
{
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setAcceptHoverEvents(true);

  d->mBgColor = Qt::transparent;

  /// setSize(QSize(32.0, 32.0));

  d->mZoomAnimation = new QPropertyAnimation(this, "scale");
  d->mZoomAnimation->setDuration(100);
  d->mZoomAnimation->setStartValue(1.0);
  d->mZoomAnimation->setEndValue(1.1);

  connect(d->mZoomAnimation, SIGNAL(finished()), this, SLOT(onZoomDone()));
}

ImageButton::~ImageButton() { delete d; }

void ImageButton::setSize(const QSize &size)
{
  setGeometry(QRectF(0, 0, size.width(), size.height()));
}

QSizeF ImageButton::sizeHint(Qt::SizeHint which,
                             const QSizeF &constraint) const
{
  return geometry().size();
}

void ImageButton::setPixmap(const QPixmap &pixmap)
{
  d->mPixmap = pixmap;
  // QGraphicsItem::setTransformOriginPoint(boundingRect().center());
}

void ImageButton::setLable(const QString &text) { d->mLabel = text; }

QString ImageButton::label() const { return d->mLabel; }

void ImageButton::onZoomDone() {}

void ImageButton::onZoomOutDone() {}

void ImageButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  if (!(d->mZoomAnimation->state() == QAbstractAnimation::Running)) {
    d->mZoomAnimation->setDirection(QAbstractAnimation::Backward);
    d->mZoomAnimation->start();
  }
  event->accept();
  Q_EMIT clicked();
}

void ImageButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  event->accept();
  Q_EMIT selected(true);
}

void ImageButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  if (!(d->mZoomAnimation->state() == QAbstractAnimation::Running)) {
    d->mZoomAnimation->setDirection(QAbstractAnimation::Forward);
    d->mZoomAnimation->start();
  }

  event->accept();
}

void ImageButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  if (scale() > 1.0) {
    d->mZoomAnimation->setDirection(QAbstractAnimation::Backward);
    d->mZoomAnimation->start();
  }

  event->accept();

  Q_EMIT selected(false);
}

void ImageButton::paintView(QPainter *painter, const QRectF &rect)
{
  painter->save();

  painter->setRenderHints(QPainter::SmoothPixmapTransform |
                          QPainter::Antialiasing |
                          QPainter::HighQualityAntialiasing);

  QPainterPath bgPath;

  bgPath.addEllipse(rect);
  painter->fillPath(bgPath, d->mBgColor);

  painter->drawPixmap(rect.toRect(), d->mPixmap);
  painter->restore();
}
}
