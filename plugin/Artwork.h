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
  Q_PROPERTY(QString artwork_url READ getArtworkUrl NOTIFY artworkurlChanged)
  Q_PROPERTY(bool loading READ getLoading NOTIFY loadingChanged)
  Q_PROPERTY(QString error READ getError NOTIFY errorChanged)

public:
  explicit Artwork(QObject *parent = nullptr);

  Q_INVOKABLE QString trackName() const;
  Q_INVOKABLE QString getArtworkUrl();
  Q_INVOKABLE QString getPath();
  Q_INVOKABLE QString getError();
  Q_INVOKABLE bool getLoading();

  Q_INVOKABLE void setPath(QString new_path);
  Q_INVOKABLE void setArtworkUrl(QString new_artwork_url);
  Q_INVOKABLE void setTrackName(QString new_name);

  Q_INVOKABLE void fetchArtwork();

  void fetchFromDBus();

Q_SIGNALS:
  void artworkurlChanged();
  void loadingChanged();
  void errorChanged();

private Q_SLOTS:
  void onReplyFinished(QNetworkReply *reply);

private:
  QNetworkAccessManager *manager;
  QString name;
  QString artwork_url;
  QString path;
  QString error;
  bool loading;
};
#endif // !ARTWORKH
