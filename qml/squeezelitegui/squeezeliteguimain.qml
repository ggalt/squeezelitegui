import QtQuick 2.0
import "components"
import net.galtfamily.controlListModel 1.0
//import net.galtfamily.controllistitem 1.0

//import QtQuick 1.0

Rectangle {
    id: main
    property alias volume: currentVol.curVol
    property alias trackDur: durationRect.songDuration
    property alias durTick: durationRect.durationTick
    property alias trackTime: durationRect.songTime

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
    signal shuffle(int state)
    signal repeat(int state)
    signal nextTrack()
    signal play(int state)
    signal prevTrack()
    signal volUp()
    signal volDown()
    signal setVolume(int vol)
    signal controlClicked(string s)
    signal controlPressAndHold(int idx)

    onVolumeChanged: {
        if(main.volume>100)
            main.volume=100
        else if(main.volume<0)
            main.volume=0
        currentVol.height = (volRect.height * main.volume)/100
    }

    function setMainVolume(vol) {
        main.volume=vol
    }

    function setControlViewListIndex(idx) {
        controlClicked("index has been set")
        controlListView.currentIndex=idx
    }

    function updateAlbumCover(imageurl) {
        coverImage.source=imageurl
    }

    function updatePlayMode(mode) {
        console.debug("updatePlayMode")
        console.debug(mode)
        playRect.setButtonState(mode)
    }

    function updateShuffleMode(mode) {
        shuffleRect.myState=mode
    }

    function updateRepeatMode(mode) {
        repeatRect.myState=mode
    }

    function updateProgress(stime) {
        main.trackTime = stime
    }

    function setSongDuration(duration) {
        main.trackDur = duration
    }

    Rectangle {
        id: playingRect
        color: "#00000000"
        anchors.fill: parent
        z: 2

        Rectangle {
            id: playWindow
            x: 491
            width: 400
            radius: 3
            border.color: "#85c4b7"
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#00c4a3"
                }

                GradientStop {
                    position: 1
                    color: "#000000"
                }
            }
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            z: 1
            Image {
                id: coverImage
                x: 18
                y: 5
                width: 300
                height: 300
                anchors.horizontalCenterOffset: -32
                anchors.horizontalCenter: parent.horizontalCenter
                source: "icons/noAlbumImage.png"
                anchors.top: parent.top
                anchors.topMargin: 5
            }

            Rectangle {
                id: rewindRect
                x: 5
                y: 338
                width: 50
                height: 50
                color: "#645a5a5a"
                MouseArea {
                    id: rewindMouseArea
                    anchors.fill: parent
                    onClicked: main.prevTrack()
                    onPressed: rewindImage.source="icons/rewind_pressed.png"
                    onReleased: rewindImage.source="icons/rewind.png"
                }

                Image {
                    id: rewindImage
                    source: "icons/rewind.png"
                    anchors.fill: parent
                }
                anchors.rightMargin: 345
                anchors.bottomMargin: 12
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }

            Rectangle {
                id: playRect
                x: 65
                y: 338
                width: 50
                height: 50
                color: "#645a5a5a"
                property int myState: 1 // start off paused, states are PLAY=0, PAUSE=1, STOP=2

                function setButtonState(newstate) {
                    myState=newstate
                    updateButtonIcon()
                }

                function updateButtonIcon() {
                    if(myState==0) {        // currently playing, so paused icon displayed
                        playImage.source="qrc:/icons/qml/squeezelitegui/icons/pause.png"
                    } else { // assume we're paused (though possibly "stopped"), so play icon displayed
                        playImage.source="qrc:/icons/qml/squeezelitegui/icons/play.png"
                    }
                }

                MouseArea {
                    id: playMouseArea
                    anchors.fill: parent
                    onClicked: {
                        playRect.myState += 1
                        playRect.myState %= 2    // stay within the 2 states of "pause and play"
                        playRect.updateButtonIcon()
                        console.debug("myState")
                        console.debug(playRect.myState)
                        main.play(playRect.myState)
                    }

                    onPressed: {
                        if( playRect.myState==0 ) {  // currently playing, so paused icon displayed
                            playImage.source="qrc:/icons/qml/squeezelitegui/icons/pause_pressed.png"
                        }
                        if( playRect.myState==1 ) {  // currently paused, so playing icon displayed
                            playImage.source="qrc:/icons/qml/squeezelitegui/icons/play_pressed.png"
                        }
                    }
                }

                Image {
                    id: playImage
                    source: "qrc:/icons/qml/squeezelitegui/icons/play.png"
                    anchors.fill: parent
                }
                anchors.rightMargin: 285
                anchors.bottomMargin: 12
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }

//            PlayButton {
//                id: playButton
//                x: 65
//                y: 338
//                width: 50
//                height: 50
//                property int mystate: 1

//                MouseArea {
//                    id: buttonMouseArea
//                    anchors.fill: parent
//                    onClicked: main.playButtonClicked()
//                    onPressed: {
//                        if(playButton.mystate==0) { //showing pause button in the "playState"
//                            console.debug("pausepressed button showing")
//                            playButton.btnImage.source="qrc:/icons/qml/squeezelitegui/icons/pause_pressed.png"
//                        }
//                        else if(playButton.mystate==1) // showing play button in the "pauseState"
//                            playButton.btnImage.source="qrc:/icons/qml/squeezelitegui/icons/play_pressed.png"
//                    }
//                }

//                function updateState(newstate) { // states are PLAY=0, PAUSE=1, STOP=2
//                    console.debug("updateState called with state")
//                    if(newstate===0) {
//                        buttonState = 'playState'
//                        playButton.mystate = 0
//                    } else if(newstate===1) {
//                        buttonState = 'pauseState'
//                        playButton.mystate = 1
//                    } else if(newstate===2) {   // we don't support "stop" so we'll just pause
//                        buttonState = 'pauseState'
//                        playButton.mystate = 1
//                    }
//                }
//            }

            Rectangle {
                id: forwardRect
                x: 125
                y: 338
                width: 50
                height: 50
                color: "#645a5a5a"
                MouseArea {
                    id: forwardMouseArea
                    anchors.fill: parent
                    onClicked: main.nextTrack()
                    onPressed: forwardImage.source="icons/forward_pressed.png"
                    onReleased: forwardImage.source="icons/forward.png"
                }

                Image {
                    id: forwardImage
                    source: "icons/forward.png"
                    anchors.fill: parent
                }
                anchors.rightMargin: 225
                anchors.bottomMargin: 12
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }
            Rectangle {
                id: shuffleRect
                x: 280
                y: 338
                width: 50
                height: 50
                property int myState: 0
                MouseArea {
                    signal shuffleClicked
                    id: shuffleMouseArea
                    anchors.fill: parent
                    onClicked: shuffleClicked()
                    onShuffleClicked: {
                        shuffleRect.myState += 1
                        shuffleRect.myState %= 3    // stay within the 3 states of "no shuffle, shuffle by song, shuffle by album"
                        main.shuffle(shuffleRect.myState)
                        if( shuffleRect.myState==0 ) {
                            shuffleImage.source="icons/no_shuffle.png"
                        }
                        else if( shuffleRect.myState==1 ) {
                            shuffleImage.source="icons/shuffle_by_song.png"
                        }
                        else if( shuffleRect.myState==2 ) {
                            shuffleImage.source="icons/shuffle_by_album.png"
                        }
                    }
                }

                Image {
                    id: shuffleImage
                    source: "icons/no_shuffle.png"
                    anchors.fill: parent
                }
                anchors.rightMargin: 70
                anchors.bottomMargin: 12
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }

            Rectangle {
                id: repeatRect
                x: 220
                y: 338
                width: 50
                height: 50
                color: "#645a5a5a"
                property int myState: 0
                MouseArea {
                    signal repeatClicked
                    id: repeatMouseArea
                    anchors.fill: parent
                    onClicked: repeatClicked()
                    onRepeatClicked: {
                        repeatRect.myState += 1
                        repeatRect.myState %= 3    // stay within the 3 states of "no repeat, repeat song, repeat album"
                        main.repeat(repeatRect.myState)
                        if( repeatRect.myState==0 ) {
                            repeatImage.source="icons/noRepeat.png"
                        }
                        else if( repeatRect.myState==1 ) {
                            repeatImage.source='icons/repeatSong.png'
                        }
                        else if( repeatRect.myState==2 ) {
                            repeatImage.source='icons/repeatAlbum.png'
                        }
                    }
                }

                Image {
                    id: repeatImage
                    source: "icons/noRepeat.png"
                    anchors.fill: parent
                }
                anchors.rightMargin: 130
                anchors.bottomMargin: 12
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }

            Rectangle {
                id: volDownRect
                x: 348
                y: 355
                width: 40
                height: 40
                color: "#645a5a5a"
                gradient: Gradient {
                    GradientStop {
                        position: 0.00;
                        color: "#5a5a5a";
                    }
                    GradientStop {
                        position: 0.52;
                        color: "#ffffff";
                    }
                }
                z: 3
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 5
                MouseArea {
                    id: volDownMouseArea
                    anchors.fill: parent
                    onClicked: {
                        main.volDown()
                        main.volume-=1
                    }
                }

                Image {
                    id: volDownImage
                    source: "icons/downvol.png"
                    anchors.fill: parent
                }
                anchors.rightMargin: 15
                anchors.right: parent.right
            }

            Rectangle {
                id: volUpRect
                x: 348
                y: 5
                width: 40
                height: 40
                color: "#645a5a5a"
                gradient: Gradient {
                    GradientStop {
                        position: 0.00;
                        color: "#5a5a5a";
                    }
                    GradientStop {
                        position: 0.52;
                        color: "#ffffff";
                    }
                }
                z: 2
                MouseArea {
                    id: volUpMouseArea
                    anchors.fill: parent
                    onClicked: {
                        main.volUp()
                        main.volume+=1
                    }
                }

                Image {
                    id: volUpImage
                    source: "icons/upvol.png"
                    anchors.fill: parent
                }
                anchors.rightMargin: 15
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: 5
            }

            Rectangle {
                id: volRect
                x: 345
                y: 47
                width: 40
                height: 305
                color: "#00000000"
                border.color: "#00ffff"
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 48
                z: 1

                Rectangle {
                    id: currentVol
                    property int curVol: 0
                    color: "#ffffff"
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: "#00ffff"
                        }

                        GradientStop {
                            position: 0.5
                            color: "#add8e6"
                        }
                    }
                }

                MouseArea {
                    id: volMouseArea
                    anchors.fill: parent
                    onClicked: {
                        main.volume = 100*(volRect.height-mouseY)/volRect.height
                        main.setVolume(main.volume)
                    }
                }
            }

            Rectangle {
                id: durationRect
                x: 5
                y: 313
                width: 211
                height: 16
                color: "#00000000"
                radius: 8
                border.color: "#0ce4d6"
                anchors.right: parent.right
                anchors.rightMargin: 184
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 71
                property int songDuration: 1
                property int durationTick: 1
                property int songTime: 0

                onSongTimeChanged :currentSongTime.width=songTime*durationTick

                Rectangle {
                    id: currentSongTime
                    width: 0
                    color: "#0ce4d6"
                    radius: 8
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.top: parent.top
                }
            }

            Rectangle {
                id: timeTextRect
                x: 220
                y: 313
                width: 110
                height: 16
                color: "#0ce4d6"

                Text {
                    id: timeText
                    text: qsTr("Text")
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 12
                }
            }
            anchors.rightMargin: 0
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 0
        }

        Rectangle {
            id: controlWindow
            width: 400
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#00c4a3"
                }

                GradientStop {
                    position: 1
                    color: "#000000"
                }
            }
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0

            Rectangle {
                id: controlBar
                height: 50
                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#000000"
                    }

                    GradientStop {
                        position: 0.44
                        color: "#00000000"
                    }

                    GradientStop {
                        position: 1
                        color: "#ffffff"
                    }
                }
                border.width: 0
                z: 3
                border.color: "#000000"
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0

                Rectangle {
                    id: homeBtn
                    width: 40
                    color: "#00000000"
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 5
                    anchors.top: parent.top
                    anchors.topMargin: 5

                    Image {
                        id: homeImage
                        anchors.fill: parent
                        source: "icons/home.png"
                    }

                    MouseArea {
                        id: homeMouseArea
                        x: 0
                        y: 0
                        anchors.fill: parent
                        onPressed: homeImage.source="icons/home_pressed.png"
                        onReleased: homeImage.source="icons/home.png"
                        onClicked: main.controlClicked("Home")
                    }
                }

                Rectangle {
                    id: returnBtn
                    x: 235
                    width: 40
                    color: "#00000000"
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 5
                    anchors.top: parent.top
                    anchors.topMargin: 5
                    anchors.right: parent.right
                    anchors.rightMargin: 5

                    Image {
                        id: returnImage
                        anchors.fill: parent
                        source: "icons/leftArrow.png"
                    }

                    MouseArea {
                        id: returnMousearea
                        x: 0
                        y: 0
                        anchors.fill: parent
                    }
                }

                Rectangle {
                    id: nowPlayingBtn
                    x: 105
                    width: 200
                    color: "#00000000"
                    border.color: "#000000"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 5
                    anchors.top: parent.top
                    anchors.topMargin: 5

                    Image {
                        id: nowPlayingImage
                        z: 1
                        anchors.fill: parent
                        source: "icons/now_playing.png"
                    }

                    MouseArea {
                        id: nowPlayingMouseArea
                        anchors.fill: parent
                        onPressed: nowPlayingImage.source="icons/now_playing_pressed.png"
                        onReleased: nowPlayingImage.source="icons/now_playing.png"
                        onClicked: main.controlClicked("NowPlaying")
                    }
                }
            }


            ListView {
                id: controlListView
                y: 333
                width: 400
                height: 350
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                highlight: highlight
                highlightFollowsCurrentItem: false
                focus: true

                model: controlListModel
                delegate: listDelegate

                Component {
                    id: listDelegate
                    Item {
                        x: 5
                        height: 40
                        width: controlListView.width
                        Row {
                            id: row1
                            spacing: 10
                            Image {
                                width: 40
                                height: 40
                                source: image
                            }

                            Text {
                                text: name
                                anchors.verticalCenter: parent.verticalCenter
                                font {
                                    pointSize: 10
                                    bold: true
                                }
                                color: "white"
                            }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                controlListView.currentIndex=index
                                main.controlClicked(index)
                            }

                            onPressAndHold: {
                                main.controlPressAndHold(index)
                            }
                        }
                    }
                }

                Component {
                    id: highlight
                    Image {
                        source: "icons/controlHighlight.png"
                        width: controlListView.width; height: 40
                        y: controlListView.currentItem.y
                        Behavior on y {
                            SpringAnimation {
                                spring: 3
                                damping: 0.2
                            }
                        }
                    }
                }


            }
        }
    }
}
