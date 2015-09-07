/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.3
import Ubuntu.Components 1.2
import Ubuntu.Web 0.2
import ".." as LocalComponents

LocalComponents.Page {
    objectName: "hereTermsPage"

    title: i18n.tr("Terms & Conditions")
    customBack: true
    customTitle: true

    onBackClicked: {
        if (webview.visible) {
            showBrowser(false);
        } else {
            pageStack.prev();
        }
    }

    function showBrowser(show) {
        if (show) {
            label1.visible = false
            label2.visible = false
            label3.visible = false
            label4.visible = false
            label5.visible = false
            webview.visible = true
        } else {
            webview.visible = false
            label1.visible = true
            label2.visible = true
            label3.visible = true
            label4.visible = true
            label5.visible = true
        }
    }

    Column {
        id: column
        anchors.fill: content
        spacing: units.gu(3)

        Label {
            id: label1
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.Wrap
            color: textColor
            fontSize: "small"
            font.weight: Font.Light
            text: i18n.tr("Your device uses positioning technologies provided by HERE.")
        }

        Label {
            id: label2
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.Wrap
            color: textColor
            fontSize: "small"
            font.weight: Font.Light
            text: i18n.tr("To provide you with positioning services and to improve their quality, HERE collects " +
                          "information about nearby cell towers and Wi-Fi hotspots around your current location " +
                          "whenever your position is being found.")
        }

        Label {
            id: label3
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.Wrap
            color: textColor
            fontSize: "small"
            font.weight: Font.Light
            text: i18n.tr("The information collected is used to analyze the service and to " +
                          "improve the use of service, but not to identify you personally.")
        }

        Label {
            id: label4
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.Wrap
            color: textColor
            fontSize: "small"
            font.weight: Font.Light
            linkColor: UbuntuColors.orange
            text: i18n.tr("By continuing, you agree to the HERE Platform Service Terms:") +
                  " <a href=\"http://here.com/terms/service-terms\">http://here.com/terms/service-terms</a>"
            onLinkActivated: {
                showBrowser(true)
                webview.url = link
            }
        }

        Label {
            id: label5
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.Wrap
            color: textColor
            fontSize: "small"
            font.weight: Font.Light
            linkColor: UbuntuColors.orange
            text: i18n.tr("and Privacy Policy:") +
                  " <a href=\"http://here.com/privacy/privacy-policy\">http://here.com/privacy/privacy-policy</a>"
            onLinkActivated: {
                showBrowser(true);
                webview.url = link;
            }
        }

        WebView {
            id: webview
            objectName: "webview"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: -leftMargin
            anchors.rightMargin: -rightMargin
            height: parent.height
            visible: false
        }
    }
}
