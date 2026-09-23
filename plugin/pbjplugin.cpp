/*
    SPDX-FileCopyrightText: 2026 Sohrab Kauchali <>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "pbjplugin.h"
#include "Artwork.h"
#include "MprisListener.h"

// KF
#include <KLocalizedString>
// Qt
#include <QJSEngine>
#include <QQmlContext>
#include <QQmlEngine>

static QJSValue singletonTypeExampleProvider(QQmlEngine *engine,
                                             QJSEngine *scriptEngine) {
  Q_UNUSED(engine)

  QJSValue helloWorld = scriptEngine->newObject();
  helloWorld.setProperty(QStringLiteral("text"), i18n("Hello world!"));
  return helloWorld;
}

void PBJPlugin::registerTypes(const char *uri) {
  qmlRegisterSingletonType(uri, 1, 0, "HelloWorld",
                           singletonTypeExampleProvider);
  qmlRegisterType<Artwork>(uri, 1, 0, "Artwork");
  qmlRegisterType<MprisListener>(uri, 1, 0, "MprisListener");
}

#include "moc_pbjplugin.cpp"
