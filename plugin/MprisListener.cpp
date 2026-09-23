#include "MprisListener.h"
#include "qobject.h"
#include "qtmetamacros.h"
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusServiceWatcher>

MprisListener::MprisListener(QObject *parent) : QObject(parent) {
  m_watcher = new QDBusServiceWatcher(this);
  m_watcher->setConnection(QDBusConnection::sessionBus());
  m_watcher->setWatchMode(QDBusServiceWatcher::WatchForOwnerChange);

  connect(m_watcher, &QDBusServiceWatcher::serviceOwnerChanged, this,
          &MprisListener::onServiceOwnerChanged);

  QDBusConnection::sessionBus().connect(
      QString(), QStringLiteral("/org/mpris/MediaPlayer2"),
      QStringLiteral("org.freedesktop.DBus.Properties"),
      QStringLiteral("PropertiesChanged"), this,
      SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
  findExistingPlayers();
}

MprisListener::~MprisListener() {}

void MprisListener::findExistingPlayers() {
  QDBusInterface dbus(QStringLiteral("org.freedesktop.DBus"),
                      QStringLiteral("/org/freedesktop/DBus"),
                      QStringLiteral("org.freedesktop.DBus"),
                      QDBusConnection::sessionBus());

  QDBusReply<QStringList> reply = dbus.call(QStringLiteral("ListNames"));
  if (reply.isValid()) {
    for (const QString &name : reply.value()) {
      if (name.startsWith(QStringLiteral("org.mpris.MediaPlayer2."))) {
        m_players.append(name);
        if (m_currentPlayer.isEmpty()) {
          m_currentPlayer = name;
        }
        connectToPlayer(name);
      }
    }

    if (!m_players.isEmpty()) {
      m_currentPlayer = m_players.first();
      Q_EMIT playersChanged();
      Q_EMIT currentPlayerChanged();
    }
  }
}

static QVariantMap demarshalMetadata(const QVariant &v) {
  QVariantMap metadata;
  if (v.canConvert<QDBusArgument>()) {
    QDBusArgument arg = v.value<QDBusArgument>();
    arg >> metadata;
  } else {
    metadata = v.toMap();
  }
  return metadata;
}

void MprisListener::onServiceOwnerChanged(const QString &serviceName,
                                          const QString &oldOwner,
                                          const QString &newOwner) {
  if (!serviceName.startsWith(QStringLiteral("org.mpris.MediaPlayer2."))) {
    return;
  }

  if (oldOwner.isEmpty() && !newOwner.isEmpty()) {
    qDebug() << QStringLiteral("MPRIS player appeared:") << serviceName;
    if (!m_players.contains(serviceName)) {
      m_players.append(serviceName);
      connectToPlayer(serviceName);
      Q_EMIT playersChanged();

      if (m_currentPlayer.isEmpty()) {
        m_currentPlayer = serviceName;
        Q_EMIT currentPlayerChanged();
      }
    }
  } else if (!oldOwner.isEmpty() && newOwner.isEmpty()) {
    qDebug() << QStringLiteral("MPRIS player disappeared:") << serviceName;
    m_players.removeAll(serviceName);
    disconnectFromPlayer(serviceName);
    Q_EMIT playersChanged();

    if (m_currentPlayer == serviceName) {
      m_currentPlayer = m_players.isEmpty() ? QString() : m_players.first();
      Q_EMIT currentPlayerChanged();

      if (!m_currentPlayer.isEmpty()) {
        QDBusInterface iface(m_currentPlayer,
                             QStringLiteral("/org/mpris/MediaPlayer2"),
                             QStringLiteral("org.freedesktop.DBus.Properties"),
                             QDBusConnection::sessionBus());

        QDBusReply<QVariant> reply =
            iface.call(QStringLiteral("Get"),
                       QStringLiteral("org.mpris.MediaPlayer2.Player"),
                       QStringLiteral("Metadata"));

        if (reply.isValid()) {
          updateMetadata(demarshalMetadata(reply));
        }
      } else {
        m_artUrl.clear();
        m_playing = false;
        Q_EMIT metadataChanged();
        Q_EMIT playbackChanged();
      }
    }
  }
}

void MprisListener::connectToPlayer(const QString &serviceName) {
  QDBusInterface iface(serviceName, QStringLiteral("/org/mpris/MediaPlayer2"),
                       QStringLiteral("org.freedesktop.DBus.Properties"),
                       QDBusConnection::sessionBus());

  QDBusReply<QVariant> metadataReply = iface.call(
      QStringLiteral("Get"), QStringLiteral("org.mpris.MediaPlayer2.Player"),
      QStringLiteral("Metadata"));

  if (metadataReply.isValid() && serviceName == m_currentPlayer) {
    updateMetadata(demarshalMetadata(metadataReply));
  }

  QDBusReply<QVariant> statusReply = iface.call(
      QStringLiteral("Get"), QStringLiteral("org.mpris.MediaPlayer2.Player"),
      QStringLiteral("PlaybackStatus"));

  if (statusReply.isValid() && serviceName == m_currentPlayer) {
    m_playing = (statusReply.value().toString() == QStringLiteral("Playing"));
    Q_EMIT playbackChanged();
  }
}

void MprisListener::disconnectFromPlayer(const QString &serviceName) {
  Q_UNUSED(serviceName);
}

void MprisListener::onPropertiesChanged(
    const QString &interface, const QVariantMap &changed_properties,
    const QStringList &invalidated_properties) {
  Q_UNUSED(invalidated_properties);

  QDBusMessage message = QDBusMessage::createMethodCall(
      QStringLiteral("org.freedesktop.DBus"),
      QStringLiteral("/org/freedesktop/DBus"),
      QStringLiteral("org.freedesktop.DBus"),
      QStringLiteral("GetConnectionUnixProcessID"));

  QString sender = message.service();

  if (interface == QStringLiteral("org.mpris.MediaPlayer2.Player")) {
    if (changed_properties.contains(QStringLiteral("Metadata"))) {
      QVariant metadata = changed_properties[QStringLiteral("Metadata")];
      if (metadata.isValid()) {
        updateMetadata(demarshalMetadata(metadata));
      }
    }

    if (changed_properties.contains(QStringLiteral("PlaybackStatus"))) {
      QString status =
          changed_properties[QStringLiteral("PlaybackStatus")].toString();
      m_playing = (status == QStringLiteral("Playing"));
      Q_EMIT playbackChanged();
    }
  }
}

void MprisListener::updateMetadata(const QVariantMap &metadata) {
  m_artUrl = metadata.value(QStringLiteral("mpris:artUrl")).toString();

  qDebug() << QStringLiteral("Artwork:") << m_artUrl;

  Q_EMIT metadataChanged();
}

void MprisListener::setCurrentPlayer(const QString &player) {
  if (m_players.contains(player) && player != m_currentPlayer) {
    m_currentPlayer = player;
    Q_EMIT currentPlayerChanged();
    QDBusInterface iface(player, QStringLiteral("/org/mpris/MediaPlayer2"),
                         QStringLiteral("org.freedesktop.DBus.Properties"),
                         QDBusConnection::sessionBus());

    QDBusReply<QVariant> reply = iface.call(
        QStringLiteral("Get"), QStringLiteral("org.mpris.MediaPlayer2.Player"),
        QStringLiteral("Metadata"));

    if (reply.isValid()) {
      updateMetadata(demarshalMetadata(reply));
    }
  }
}

void MprisListener::callPlayerMethod(const QString &method) {
  if (m_currentPlayer.isEmpty()) {
    return;
  }

  QDBusInterface iface(m_currentPlayer,
                       QStringLiteral("/org/mpris/MediaPlayer2"),
                       QStringLiteral("org.mpris.MediaPlayer2.Player"),
                       QDBusConnection::sessionBus());

  iface.call(method);
}
