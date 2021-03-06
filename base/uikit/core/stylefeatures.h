#ifndef STYLE_FEATURES_H
#define STYLE_FEATURES_H

#include <QObject>
#include <QRectF>
#include <QPainter>
#include <QVariantMap>
#include <plexydesk_ui_exports.h>

namespace UIKit {

class DECL_UI_KIT_EXPORT StyleFeatures {

public:
  typedef enum {
    kRenderBackground,
    kRenderForground,
    kRenderElement,
    kRenderRaised,
    kRenderPressed
  } RenderState;

  QRectF geometry;
  QString text_data;
  QTextOption text_options;
  QPixmap image_data;
  RenderState render_state;

  QVariantMap attributes;
};
}
#endif
