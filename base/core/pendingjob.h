#ifndef PENDINGJOB_H
#define PENDINGJOB_H

#include <plexy.h>
#include <QtCore/QObject>
#include <PlexyDeskCore_export.h>

namespace PlexyDesk {
class PlexyDeskCore_EXPORT PendingJob : public QObject {
  Q_OBJECT
public:
  /**
  **/
  bool isError();
  /**
  **/
  QString errorName() const;
  /**
  **/
  QString errorMessage() const;
  /**
  **/
  void setFinished(bool status, QString &error, const QString &message);
Q_SIGNALS:
  void finished();
  void finished(PendingJob *);

protected:
  PendingJob(QObject *parent);

private:
  Q_DISABLE_COPY(PendingJob)
  class Private;
  Private *const d;
};
}
#endif // PENDINGJOB_H
