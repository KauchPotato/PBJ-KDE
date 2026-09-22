#ifndef ARTWORKH
#define ARTWORKH
#include "QtNetwork/qnetworkaccessmanager.h"
#include "qhashfunctions.h"
#include "qobject.h"
#include "qtmetamacros.h"
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>

class Artwork : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString trackName READ trackName NOTIFY trackNameChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
  Q_PROPERTY(QString error READ error NOTIFY errorChanged)

public:
  explicit Artwork(QObject *parent = nullptr);

  Q_INVOKABLE QString trackName() const;
  Q_INVOKABLE bool loading() const;
  Q_INVOKABLE QString error() const;

  Q_INVOKABLE QString getArtworkUrl();
  Q_INVOKABLE QString getPath();

  Q_INVOKABLE void setPath(QString new_path);
  Q_INVOKABLE void setArtworkUrl(QString new_artwork_url);
  Q_INVOKABLE void setTrackName(QString new_name);

  Q_INVOKABLE void fetchArtwork();

signals:
  void trackNameChanged();
  void loadingChanged();
  void errorChanged();

private slots:
  void onReplyFinished(QNetworkReply *reply);

private:
  QNetworkAccessManager *manager;
  QString name;
  QString artwork_url;
  QString path;
  QString m_error;
  bool m_loading;
};
#endif // !ARTWORKH
