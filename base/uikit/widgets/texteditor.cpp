#include "texteditor.h"
#include <QGraphicsProxyWidget>
#include <QTextEdit>
#include <QAbstractScrollArea>
#include <QObject>
#include <QTextBlock>
#include <QTextBrowser>
#include <QClipboard>
#include <QMimeData>
#include <QTextFragment>
#include <QTextDocumentFragment>
#include <QDebug>
#include <QApplication>

#include <widget.h>

namespace UI
{

class TextEditor::PrivateTextEditor
{
public:
  PrivateTextEditor() {}
  ~PrivateTextEditor() {}

  QString extractHeader(const QString &headerText);

  QGraphicsProxyWidget *mProxyWidget;
  QTextBrowser *mEditor;
  qreal mTextScaleFactor;
};

TextEditor::TextEditor(QGraphicsObject *parent)
  : UI::Widget(parent), d(new PrivateTextEditor)
{
  d->mProxyWidget = new QGraphicsProxyWidget(this);
  d->mEditor = new QTextBrowser(0);
  d->mEditor->setReadOnly(false);
  d->mEditor->setAcceptRichText(true);
  d->mEditor->setAutoFormatting(QTextEdit::AutoAll);
  d->mEditor->setStyleSheet("background-color:qlineargradient(x1: 0, y1: 0, "
                            "x2: 0, y2: 1, stop: 1 #ffffff, stop: 0 #E5EAEE); "
                            "border : 0");
  d->mEditor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  d->mEditor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  d->mProxyWidget->setWidget(d->mEditor);

  // QRectF rect (0.0, 0.0, 320.0, 200.0);
  // d->mProxyWidget->resize(rect.size());
  // d->mProxyWidget->setMinimumSize(rect.size());
  // d->mProxyWidget->setMaximumSize(rect.size());

  d->mProxyWidget->show();
  d->mEditor->move(0.0, 0.0);
  d->mProxyWidget->setPos(0.0, 0.0);

  d->mTextScaleFactor = 1.0;
  setWindowFlag(UI::Widget::kRenderDropShadow, false);
  setWindowFlag(UI::Widget::kRenderBackground, false);
  setFlag(QGraphicsItem::ItemIsMovable, false);

  connect(d->mEditor, SIGNAL(textChanged()), this, SLOT(onTextUpdated()));
  connect(d->mEditor->document(), SIGNAL(blockCountChanged(int)), this,
          SLOT(onBlockCountChanged(int)));
}

TextEditor::~TextEditor() { delete d; }

void TextEditor::setText(const QString &text) { d->mEditor->setText(text); }

void TextEditor::setPlaceholderText(const QString &placeholderText)
{
  // if (d->mEditor)
  //   d->mEditor->setPlaceholderText(placeholderText);
}

void TextEditor::setFontPointSize(qreal s)
{
  if (d->mEditor) {
    d->mEditor->setFontPointSize(s);
  }
}

QString TextEditor::text() const
{
  if (d->mEditor) {
    return d->mEditor->toPlainText();
  }

  return QString();
}

void TextEditor::style(const QString &style)
{
  if (d->mEditor) {
    d->mEditor->setStyleSheet(style);
  }
}

QSizeF TextEditor::sizeHint(Qt::SizeHint which,
                            const QSizeF &constraint) const
{
  return d->mEditor->size();
}

void TextEditor::setGeometry(const QRectF &rect)
{
  qDebug() << Q_FUNC_INFO << "=========================================" << rect
           << "=========================================";

  d->mProxyWidget->setMinimumSize(rect.size());
  d->mProxyWidget->setMaximumSize(rect.size());
  d->mProxyWidget->resize(rect.size());
  d->mEditor->move(0.0, 0.0);

  Widget::setGeometry(rect);
}

void TextEditor::updateTextScale()
{
  QRectF bounds = boundingRect();
  const QRectF newBounds(bounds.x(), bounds.y(),
                         bounds.width() / d->mTextScaleFactor,
                         bounds.height() / d->mTextScaleFactor);
  setGeometry(newBounds);

  d->mProxyWidget->setMinimumSize(newBounds.size());
  d->mProxyWidget->setMaximumSize(newBounds.size());

  d->mProxyWidget->setGeometry(newBounds);
  d->mProxyWidget->resize(newBounds.size());

  setScale(d->mTextScaleFactor);
}

void TextEditor::beginList()
{
  if (!d->mEditor) {
    return;
  }

  QTextCursor cursor = d->mEditor->textCursor();

  cursor.beginEditBlock();
  cursor.insertList(QTextListFormat::ListCircle);
  cursor.insertText("");
  cursor.endEditBlock();
}

void TextEditor::endList()
{
  if (!d->mEditor) {
    return;
  }
}

void TextEditor::convertToLink()
{
  if (!d->mEditor) {
    return;
  }

  QTextCursor cursor = d->mEditor->textCursor();

  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();

  if (mimeData->hasUrls()) {
    Q_FOREACH(const QUrl & url, mimeData->urls()) {
      qDebug() << Q_FUNC_INFO << url;
      cursor.beginEditBlock();
      cursor.insertFragment(QTextDocumentFragment::fromHtml(
                              QString("<a href=\"%1\"> %2</a> <p> </p>").arg(url.toString()).arg(
                                url.toString())));
      cursor.endEditBlock();
    }
  } else if (mimeData->hasText()) {
    QString data = mimeData->text();
    QString proto = data.left(5);
    QUrl testUrl(data);

    if (proto == "http:" || proto == "https" || proto == "ftp:/") {
      cursor.beginEditBlock();
      cursor.insertFragment(QTextDocumentFragment::fromHtml(
                              QString("<a href=\"%1\">%2</a> <p> </p>").arg(testUrl.toString()).arg(
                                testUrl.toString())));
      cursor.endEditBlock();
    }
  }
}

void TextEditor::onTextUpdated()
{
  QTextDocument *doc = d->mEditor->document();
  Q_EMIT textUpdated(this->text());
}

void TextEditor::onBlockCountChanged(int count)
{
  if (count == 2) {
    QTextDocument *document = d->mEditor->document();
    QTextCursor cursor(document);

    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    // format.setFontPointSize (18);

    cursor.mergeCharFormat(format); // do the text as Bold
    Q_EMIT documentTitleAvailable(document->firstBlock().text());
  }
}

void TextEditor::setTextScaleFactor(qreal scaleFactor)
{
  d->mTextScaleFactor = scaleFactor;
  updateTextScale();
}

qreal TextEditor::textScaleFactor() const { return d->mTextScaleFactor; }

QString TextEditor::PrivateTextEditor::extractHeader(
  const QString &headerText)
{
  QString headerString = QString("<h1><b>" + headerText + "</b></h1>");

  return headerString;
}
}
