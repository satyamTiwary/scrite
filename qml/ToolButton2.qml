/****************************************************************************
**
** Copyright (C) TERIFLIX Entertainment Spaces Pvt. Ltd. Bengaluru
** Author: Prashanth N Udupa (prashanth.udupa@teriflix.com)
**
** This code is distributed under GPL v3. Complete text of the license
** can be found here: https://www.gnu.org/licenses/gpl-3.0.txt
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

import QtQuick 2.13
import QtQuick.Controls 2.13

ToolButton {
    id: toolButton

    property real suggestedWidth: {
        if(display === AbstractButton.IconOnly || text.length === 0)
            return suggestedHeight
        return 120
    }
    property real suggestedHeight: 55
    property string shortcut
    property string shortcutText: shortcut
    implicitWidth: suggestedWidth
    implicitHeight: suggestedHeight

    font.pixelSize: 16
    hoverEnabled: true
    display: AbstractButton.TextBesideIcon
    opacity: enabled ? 1 : 0.5
    background: Rectangle {
        color: (down || (checkable && checked)) ? "#9E9E9E" : Qt.rgba(0,0,0,0)
    }
    contentItem: Item {
        Row {
            anchors.centerIn: parent
            spacing: 10

            Image {
                source: toolButton.icon.source
                width: toolButton.icon.width
                height: toolButton.icon.height
                anchors.verticalCenter: parent.verticalCenter
                visible: status === Image.Ready
            }

            Text {
                text: toolButton.action.text
                font.pixelSize: toolButton.font.pixelSize
                font.bold: toolButton.down
                anchors.verticalCenter: parent.verticalCenter
                Behavior on color { ColorAnimation { duration: 250 } }
                visible: toolButton.display !== AbstractButton.IconOnly
            }
        }

        Text {
            font.pixelSize: 9
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            text: "[" + toolButton.shortcutText + "]"
            visible: toolButton.shortcutText !== ""
        }
    }
    action: Action {
        text: toolButton.text
        shortcut: toolButton.shortcut
    }

    ToolTip.text: app.polishShortcutTextForDisplay(text + "\t" + shortcut)
    ToolTip.visible: hovered
}
