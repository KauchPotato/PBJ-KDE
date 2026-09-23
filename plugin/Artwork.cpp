#include "Artwork.h"
#include "QtNetwork/qnetworkaccessmanager.h"
#include "QtNetwork/qnetworkrequest.h"
#include "qhashfunctions.h"
#include "qjsonobject.h"
#include "qlogging.h"
#include "qobject.h"
#include "qtmetamacros.h"
#include "qurl.h"
#include "qvariant.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <QtNetwork/QNetworkReply>

static void log(const QString &msg) {
  QFile f(QStringLiteral("/tmp/pbj-wallpaper.log"));
  if (!f.open(QIODevice::Append | QIODevice::Text))
    return;
  QTextStream out(&f);
  out << msg << "\n";
}

Artwork::Artwork(QObject *parent)
    : QObject(parent), manager(new QNetworkAccessManager(this)), name(),
      artwork_url(), path(), error(), loading(false) {
  connect(manager, &QNetworkAccessManager::finished, this,
          &Artwork::onReplyFinished);
}

QString Artwork::getArtworkUrl() { return artwork_url; }

QString Artwork::getPath() { return path; }

QString Artwork::trackName() const { return name; }

QString Artwork::getError() { return error; }

bool Artwork::getLoading() { return loading; }

void Artwork::setArtworkUrl(QString new_artwork_url) {
  artwork_url = new_artwork_url;
  Q_EMIT artworkurlChanged();
}

void Artwork::setPath(QString new_path) { path = new_path; }

void Artwork::setTrackName(QString new_name) { name = new_name; }

void Artwork::fetchArtwork() {
  loading = true;
  Q_EMIT loadingChanged();
  log(QStringLiteral("Starting request to: ") + artwork_url);
  manager->get(QNetworkRequest(QUrl(artwork_url)));
}

void Artwork::onReplyFinished(QNetworkReply *reply) {
  loading = false;
  Q_EMIT loadingChanged();

  int statusCode =
      reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  log(QStringLiteral("HTTP status: ") + QString::number(statusCode));
  log(QStringLiteral("Reply error: ") + QString::number(reply->error()));

  if (reply->error() == QNetworkReply::NoError) {
    QByteArray data = reply->readAll();
    log(QStringLiteral("Data length: ") + QString::number(data.size()));
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
      log(QStringLiteral("JSON parsing failed - response is not valid JSON"));
    } else {
      log(QString::fromUtf8(doc.toJson(QJsonDocument::Indented)));
      QJsonObject obj = doc.object();
    }
  } else {
    log(QStringLiteral("Request failed: ") + reply->errorString());
  }
  reply->deleteLater();
}
