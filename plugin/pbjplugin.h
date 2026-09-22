/*
    SPDX-FileCopyrightText: 2026 Sohrab Kauchali <>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PBJPLUGIN_H
#define PBJPLUGIN_H

#include <QQmlExtensionPlugin>

class PBJPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override;
};

#endif // PBJPLUGIN_H
