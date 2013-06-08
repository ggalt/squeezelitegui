import QtQuick 2.0

Rectangle {
    id: playPauseButton
    property alias btnImage: buttonImage
    property alias buttonState: playPauseButton.state
    Image {
        id: buttonImage
        anchors.fill: parent
        source: "qrc:/icons/qml/squeezeliteqml2/icons/play.png"
    }

    // remember, if we are in the "playState", we are showing the "pause" button because that is
    // what action is available to do, and if we are in "pauseState" we show the "play" button
    states: [
        State {
            name: "playState"
            PropertyChanges {
                target: buttonImage
                source: "qrc:/icons/qml/squeezeliteqml2/icons/play_pressed.png"
            }
        },
        State {
            name: "playPressedState"
            PropertyChanges {
                target: buttonImage
                source: "qrc:/icons/qml/squeezeliteqml2/icons/play.png"
            }
        },
        State {
            name: "pauseState"
            PropertyChanges {
                target: buttonImage
                source: "qrc:/icons/qml/squeezeliteqml2/icons/pause_pressed.png"
            }
        },
        State {
            name: "pausePressedState"
            PropertyChanges {
                target: buttonImage
                source: "qrc:/icons/qml/squeezeliteqml2/icons/pause.png"
            }
        }
    ]
}
