#include "Artwork.h"
#include "QtNetwork/qnetworkaccessmanager.h"
#include "QtNetwork/qnetworkrequest.h"
#include "qobject.h"
#include "qtmetamacros.h"
#include "qurl.h"
#include "qvariant.h"

Artwork::Artwork(QObject *parent)
    : QObject(parent), artwork_url(""), path(""), name(""), m_error(""),
      m_loading(false) {
  connect(manager, &QNetworkAccessManager::finished, this,
          &Artwork::onReplyFinished);
}

QString Artwork::getArtworkUrl() { return artwork_url; }

QString Artwork::getPath() { return path; }

QString Artwork::trackName() const { return name; }

void Artwork::setArtworkUrl(QString new_artwork_url) {
  artwork_url = new_artwork_url;
}

void Artwork::setPath(QString new_path) { path = new_path; }

void Artwork::setTrackName(QString new_name) {
  name = new_name;
  emit trackNameChanged();
}

void Artwork::fetchArtwork() {
  m_loading = true;
  emit loadingChanged();
  manager->get(QNetworkRequest(QUrl(artwork_url)));
}

void Artwork::onReplyFinished(QNetworkReply *reply) {
  m_loading = false;
  emit loadingChanged();

  if (reply->error() == QNetworkReply::NoError) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonDocument obj = doc.object();
  }
}
