import QtQuick 1.1
//import QtQuick 1.0

Rectangle {
    id: main
    width: 800
    height: 400
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#00000000"
        }

        GradientStop {
            position: 1
            color: "#000000"
        }
    }

    Flipable {
        id: flipable
        anchors.fill: parent
        property bool flipped: false
        back: Flickable {
            id: backFlickable
            flickableDirection: Flickable.HorizontalFlick
            anchors.fill: parent

            Rectangle {
                id: leftRect
                color: "#00000000"
                anchors.right: parent.right
                anchors.rightMargin: 400
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0

                ListView {
                    id: backLeftList
                    anchors.fill: parent
                    model: ListModel {
                        ListElement {
                            name: "Grey"
                            colorCode: "grey"
                        }

                        ListElement {
                            name: "Red"
                            colorCode: "red"
                        }

                        ListElement {
                            name: "Blue"
                            colorCode: "blue"
                        }

                        ListElement {
                            name: "Green"
                            colorCode: "green"
                        }
                    }
                    delegate: Item {
                        x: 5
                        height: 40
                        Row {
                            id: row1
                            Rectangle {
                                width: 40
                                height: 40
                                color: colorCode
                            }

                            Text {
                                text: name
                                anchors.verticalCenter: parent.verticalCenter
                                font.bold: true
                            }
                            spacing: 10
                        }
                    }
                }
            }

            Rectangle {
                id: rightRect
                x: 400
                y: 0
                width: 400
                color: "#00000000"
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0

                ListView {
                    id: backRightList
                    anchors.fill: parent
                    model: ListModel {
                        ListElement {
                            name: "Grey"
                            colorCode: "grey"
                        }

                        ListElement {
                            name: "Red"
                            colorCode: "red"
                        }

                        ListElement {
                            name: "Blue"
                            colorCode: "blue"
                        }

                        ListElement {
                            name: "Green"
                            colorCode: "green"
                        }
                    }
                    delegate: Item {
                        x: 5
                        height: 40
                        Row {
                            id: row2
                            Rectangle {
                                width: 40
                                height: 40
                                color: colorCode
                            }

                            Text {
                                text: name
                                anchors.verticalCenter: parent.verticalCenter
                                font.bold: true
                            }
                            spacing: 10
                        }
                    }
                }
            }
        }

        front: Rectangle {
            id: frontRect
            color: "#00000000"
            anchors.fill: parent

            Rectangle {
                id: playWindow
                x: 491
                width: 400
                height: 400
                color: "#00000000"
                Image {
                    id: coverImage
                    x: 63
                    y: 5
                    width: 300
                    height: 300
                    anchors.horizontalCenterOffset: 13
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "icons/noAlbumImage.png"
                    anchors.top: parent.top
                    anchors.topMargin: 5
                }

                Rectangle {
                    id: rewindRect
                    x: 50
                    y: 338
                    width: 50
                    height: 50
                    color: "#645a5a5a"
                    MouseArea {
                        id: rewindMouseArea
                        anchors.fill: parent
                    }

                    Image {
                        id: rewindImage
                        source: "icons/rewind.png"
                        anchors.fill: parent
                    }
                    anchors.rightMargin: 300
                    anchors.bottomMargin: 12
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                }

                Rectangle {
                    id: playRect
                    x: 110
                    y: 338
                    width: 50
                    height: 50
                    color: "#645a5a5a"
                    MouseArea {
                        id: playMouseArea
                        anchors.fill: parent
                    }

                    Image {
                        id: playImage
                        source: "icons/play.png"
                        anchors.fill: parent
                    }
                    anchors.rightMargin: 240
                    anchors.bottomMargin: 12
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                }

                Rectangle {
                    id: forwardRect
                    x: 170
                    y: 338
                    width: 50
                    height: 50
                    color: "#645a5a5a"
                    MouseArea {
                        id: forwardMouseArea
                        anchors.fill: parent
                    }

                    Image {
                        id: forwardImage
                        source: "icons/forward.png"
                        anchors.fill: parent
                    }
                    anchors.rightMargin: 180
                    anchors.bottomMargin: 12
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                }

                Rectangle {
                    id: shuffleRect
                    x: 330
                    y: 338
                    width: 50
                    height: 50
                    color: "#645a5a5a"
                    MouseArea {
                        id: shuffleMouseArea
                        anchors.fill: parent
                    }

                    Image {
                        id: shuffleImage
                        source: "icons/play.png"
                        anchors.fill: parent
                    }
                    anchors.rightMargin: 25
                    anchors.bottomMargin: 12
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                }

                Rectangle {
                    id: repeatRect
                    x: 270
                    y: 338
                    width: 50
                    height: 50
                    color: "#645a5a5a"
                    MouseArea {
                        id: repeatMouseArea
                        anchors.fill: parent
                    }

                    Image {
                        id: repeatImage
                        source: "icons/play.png"
                        anchors.fill: parent
                    }
                    anchors.rightMargin: 85
                    anchors.bottomMargin: 12
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                }

                Rectangle {
                    id: volDownRect
                    x: 0
                    y: 355
                    width: 40
                    height: 40
                    color: "#645a5a5a"
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 5
                    MouseArea {
                        id: volDownMouseArea
                        anchors.fill: parent
                    }

                    Image {
                        id: volDownImage
                        source: "icons/downvol.png"
                        anchors.fill: parent
                    }
                    anchors.rightMargin: 360
                    anchors.right: parent.right
                }

                Rectangle {
                    id: volUpRect
                    x: 7
                    y: 261
                    width: 40
                    height: 40
                    color: "#645a5a5a"
                    MouseArea {
                        id: volUpMouseArea
                        anchors.fill: parent
                    }

                    Image {
                        id: volUpImage
                        source: "icons/upvol.png"
                        anchors.fill: parent
                    }
                    anchors.rightMargin: 360
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: 5
                }

                Rectangle {
                    id: volRect
                    x: 0
                    width: 40
                    anchors.right: parent.right
                    anchors.rightMargin: 360
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 5
                    anchors.top: parent.top
                    anchors.topMargin: 5
                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: "#cef90a"
                        }

                        GradientStop {
                            position: 1
                            color: "#80f9f6f6"
                        }
                    }
                    z: -1
                }

                Rectangle {
                    id: rectangle1
                    x: 50
                    y: 313
                    width: 325
                    height: 16
                    color: "#00000000"
                    border.color: "#7a7a7a"
                    anchors.right: parent.right
                    anchors.rightMargin: 25
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 71
                }
                anchors.rightMargin: 0
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: 0
            }

            Component {
                id: playListItem
                Item {
                    width: 395; height: 40
                    Column {
                        Image {source: image }
                        Text {text: name }
                    }
                }
            }

            ListView {
                id: playListFlick
                width: 398
                interactive: true
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0
//                snapMode: ListView.SnapToItem
//                model: playListModel {}
                delegate: playListItem
//                delegate: Item {
//                    x: 5
//                    height: 40
//                    Row {
//                        id: row1
//                        Rectangle {
//                            width: 40
//                            height: 40
//                            color: colorCode
//                        }

//                        Text {
//                            text: name
//                            anchors.verticalCenter: parent.verticalCenter
//                            font.bold: true
//                        }
//                        spacing: 10
//                    }
//                }
            }
        }


        transform: Rotation {
            id: rotation
            origin.x: flipable.width/2
            origin.y: flipable.height/2
            axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
            angle: 0    // the default angle
        }

        MouseArea {
            id: mouse_area1
            anchors.fill: parent
            onDoubleClicked: flipable.flipped = !flipable.flipped
        }

        states: State {
            name: "back"
            PropertyChanges { target: rotation; angle: 180 }
            when: flipable.flipped
        }

        transitions: Transition {
            NumberAnimation { target: rotation; property: "angle"; duration: 750 }
        }
    }
}
