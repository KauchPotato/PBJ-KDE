#include "QtDBus/qdbusconnection.h"
#include "QtDBus/qdbusservicewatcher.h"
#include "qcontainerfwd.h"
#include "qtmetamacros.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusServiceWatcher>

class MprisListener : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString artUrl READ artUrl NOTIFY metadataChanged)
  Q_PROPERTY(bool playing READ playing NOTIFY playbackChanged)
  Q_PROPERTY(QStringList players READ players NOTIFY playersChanged)
  Q_PROPERTY(
      QString currentPlayer READ currentPlayer NOTIFY currentPlayerChanged)
public:
  explicit MprisListener(QObject *parent = nullptr);
  ~MprisListener();

  QString artUrl() const { return m_artUrl; }
  bool playing() const { return m_playing; }
  QStringList players() const { return m_players; }
  QString currentPlayer() const { return m_currentPlayer; }

  Q_INVOKABLE void setCurrentPlayer(const QString &player);
Q_SIGNALS:
  void metadataChanged();
  void playbackChanged();
  void playersChanged();
  void currentPlayerChanged();
private Q_SLOTS:
  void onServiceOwnerChanged(const QString &serviceName,
                             const QString &oldOwner, const QString &newOwner);
  void onPropertiesChanged(const QString &interface,
                           const QVariantMap &changed_properties,
                           const QStringList &invalidated_properties);

private:
  void findExistingPlayers();
  void connectToPlayer(const QString &serviceName);
  void disconnectFromPlayer(const QString &serviceName);
  void updateMetadata(const QVariantMap &metadata);
  void callPlayerMethod(const QString &method);

  QDBusServiceWatcher *m_watcher;
  QStringList m_players;
  QString m_currentPlayer;

  QString m_artUrl;
  bool m_playing = false;
};
