/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Michael Zanetti <michael.zanetti@canonical.com>
*/

import QtQuick 2.0
import Mir.Application 0.1

Item {
    id: root

    signal clicked()
    property real topMarginProgress
    property bool interactive: true
    property real maximizedAppTopMargin
    property var application: ApplicationManager.get(index)

    // FIXME: This really should be invisible to QML code.
    // e.g. Create a SurfaceItem {} in C++ which we just use without any imperative hacks.
    property var surface: application ? application.surface : null
    onSurfaceChanged: {
        if (surface) {
            print("########################## surface created!")
            surface.parent = root;
            surface.z = 1;
        }
    }

    Binding {
        target: surface
        property: "enabled"
        value: root.interactive
    }

    state: {
        if (surface) {
            if (surface.state === MirSurfaceItem.Fullscreen) {
                "fullscreen";
            } else {
                "maximized";
            }
        } else {
            return "empty"
        }
    }
    states: [
        State {
            name: "fullscreen"
            PropertyChanges {
                target: surface
                x: 0
                y: 0
                width: root.width
                height: root.height
            }
        },
        State {
            name: "maximized"
            PropertyChanges {
                target: surface
                x: 0
                y: maximizedAppTopMargin
                width: root.width
                height: root.height - y
            }
        },
        State {
            name: "empty"
        }
    ]

    BorderImage {
        id: dropShadow
        anchors.fill: surface
        anchors.margins: -units.gu(2)
        source: "graphics/dropshadow.png"
        opacity: .4
        border { left: 50; right: 50; top: 50; bottom: 50 }
    }

    // This is used to get clicked events on the whole app. e.g. when in spreadView.
    // It's only enabled when the surface is !interactive
    MouseArea {
        anchors.fill: parent
        z: 2
        enabled: !root.interactive
        onClicked: {
            root.clicked()
        }
    }
}
