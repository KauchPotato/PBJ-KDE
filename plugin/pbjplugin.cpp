/*
    SPDX-FileCopyrightText: 2026 Sohrab Kauchali <>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "pbjplugin.h"

// KF
#include <KLocalizedString>
// Qt
#include <QJSEngine>
#include <QQmlEngine>
#include <QQmlContext>

static QJSValue singletonTypeExampleProvider(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)

    QJSValue helloWorld = scriptEngine->newObject();
    helloWorld.setProperty(QStringLiteral("text"), i18n("Hello world!"));
    return helloWorld;
}


void PBJPlugin::registerTypes(const char* uri)
{
    qmlRegisterSingletonType(uri, 1, 0, "HelloWorld", singletonTypeExampleProvider);
}

#include "moc_pbjplugin.cpp"
