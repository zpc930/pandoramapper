/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//! [1]
import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
//import OpenGLUnderQML 1.0
import MudOutput 1.0

import SceneGraphRendering 1.0
import Qt.QtBrokerSingleton 1.0

Item {
    id: mainItem


    width: 1400
    height: 900

    //    Squircle {
    //        id: squircle1
    //        x: 338
    //        width: 200
    //        height: 200
    //        anchors.top: parent.top
    //        anchors.topMargin: 5
    //        anchors.right: parent.right
    //        anchors.rightMargin: 5


    //        SequentialAnimation on t {
    //            NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
    //            NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
    //            loops: Animation.Infinite
    //            running: true
    //        }
    //    }

    Rectangle {
        color: "#b3848f8f"
        radius: 3
        border.width: 1
        anchors.fill: mainItem
        anchors.margins: -3

        Button {
            id: connectButton
            x: 20
            y: 18
            text: "Connect"

            onClicked: {
                console.log("Clicked!");
                QtBroker.connect();
            }
        }
    }


    Rectangle {
        color: "#b3848f8f"
        radius: 3
        border.width: 1
        anchors.fill: mainItem
        anchors.margins: -3

        Button {
            id: textTestButton
            x: 200
            y: 18
            text: "Set Text"

            onClicked: {
                console.log("Clicked!");
                QtBroker.appendText();
            }
        }
    }






    ColumnLayout {
        id: column_layout1
        anchors.rightMargin: 5
        anchors.leftMargin: 5
        anchors.bottomMargin: 5
        anchors.topMargin: 100
        anchors.fill: parent

        Rectangle {
            id: mud_output_rect

            color: Qt.rgba(1, 1, 1, 0.7)
            radius: 3
            border.width: 1
            border.color: "white"

            anchors.bottom: command_line.top
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottomMargin: 7

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 100
            Layout.preferredWidth: 200
            Layout.preferredHeight: 100

            anchors.margins: -3

            MudOutput {
                id: mud_output

                Component.onCompleted: {
                    console.log("Connecting signals of mud output component");
                    QtBroker.mudTextChanged.connect(mud_output.appendText)
                }

                anchors.margins: 3
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.top: parent.top

                text: qsTr("Mud stream\r\ntest")
                font.pixelSize: 12
            }
        }

        TextField {
            id: command_line
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: parent.bottom

            text: qsTr("Text Edit")
            font.pixelSize: 12
        }

        Rectangle {
            id: command_line_rect
            color: Qt.rgba(1, 1, 1, 0.7)
            radius: 3
            border.width: 1
            border.color: "white"
            anchors.fill: command_line
            anchors.margins: -3
        }



    }









    Rectangle {
        color: "#848f8f"
        radius: 3
        border.color: "#0f0c0c"
        border.width: 2
        anchors.fill: renderer

        anchors.margins: -3
    }

    states: [
        State {
            name: "NORMAL"
//            PropertyChanges { target: renderer;  }
        },
        State {
            name: "RAISED"
//            PropertyChanges { target: renderer; width: width * 2; height: height * 2; }
        }
    ]


    Renderer {
        id: renderer
        state: "NORMAL"
//            anchors: parent
        width: parent.width / 3;
        height: parent.height / 3;

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 3
        //anchors.margins: 10


        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_M) {
                if (state == "NORMAL") {
                    console.log("Key M, raising")
                    state = "RAISED"
                    raise()
                } else {
                    console.log("Key M, normal state")
                    state = "NORMAL"
                    minimize()
                }
            }
        }

        // The transform is just to show something interesting..
//            transform: [
//                Rotation { id: rotation; axis.x: 0; axis.z: 0; axis.y: 1; angle: 0; origin.x: renderer.width / 2; origin.y: renderer.height / 2; },
//                Translate { id: txOut; x: -renderer.width / 2; y: -renderer.height / 2 },
//                Scale { id: scale; },
//                Translate { id: txIn; x: renderer.width / 2; y: renderer.height / 2 }
//            ]

        SequentialAnimation {
//                PauseAnimation { duration: 5000 }
//                ParallelAnimation {
//                    NumberAnimation { target: scale; property: "xScale"; to: 0.6; duration: 1000; easing.type: Easing.InOutBack }
//                    NumberAnimation { target: scale; property: "yScale"; to: 0.6; duration: 1000; easing.type: Easing.InOutBack }
//                }
//                NumberAnimation { target: rotation; property: "angle"; to: 80; duration: 1000; easing.type: Easing.InOutCubic }
//                NumberAnimation { target: rotation; property: "angle"; to: -80; duration: 1000; easing.type: Easing.InOutCubic }
//                NumberAnimation { target: rotation; property: "angle"; to: 0; duration: 1000; easing.type: Easing.InOutCubic }
//                NumberAnimation { target: renderer; property: "opacity"; to: 0.5; duration: 1000; easing.type: Easing.InOutCubic }
//                PauseAnimation { duration: 1000 }
//                NumberAnimation { target: renderer; property: "opacity"; to: 0.8; duration: 1000; easing.type: Easing.InOutCubic }
//                ParallelAnimation {
//                    NumberAnimation { target: scale; property: "xScale"; to: 1; duration: 1000; easing.type: Easing.InOutBack }
//                    NumberAnimation { target: scale; property: "yScale"; to: 1; duration: 1000; easing.type: Easing.InOutBack }
//                }
            running: true
            loops: Animation.Infinite
        }

        function raise()
        {
            width = width * 2;
            height = height * 2;
        }

        function minimize()
        {
            width = width / 2;
            height = height / 2;
        }
    }




//! [2]
}
