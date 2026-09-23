/*
    SPDX-FileCopyrightText: 2026 Sohrab Kauchali <>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents
import org.kde.plasma.plasmoid
import QtQuick.Effects
import "Plugin"

WallpaperItem {
    id: root

    Artwork {
        id: artwork
        Component.onCompleted: {
          artwork.setArtworkUrl(listener.artUrl)
          artwork.fetchArtwork()
        }
      }
    MprisListener{
      id: listener
      onPlayersChanged:{
        console.log("Available players:",players)
      }
      onMetadataChanged:{
        console.log("Song:",artUrl)
      }
    }

    Rectangle {
        anchors.fill: parent
        color: Kirigami.Theme.backgroundColor
      }
    Image{
      id: pp
      source: listener.artUrl
      anchors.fill: parent
      fillMode: Image.PreserveAspectCrop
      visible: false
    }
    MultiEffect {
      source: pp
      anchors.fill: pp
      blurEnabled: true
      blur: 1         
      blurMax: 100         
      blurMultiplier: 10
      autoPaddingEnabled: true
    }
    Image{
      anchors.centerIn: parent
      id: front
      source: listener.artUrl
    }
}
