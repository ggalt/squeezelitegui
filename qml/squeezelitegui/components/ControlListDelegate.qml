import QtQuick 2.0

Item {
    id: listItem
    property bool active: false
//    property alias mainText: lblImage.mainText
//    property alias subText: lblImage.subText
//    property alias image: lblImage.image
//    property alias drillDown: lblImage.drilldownVisisble
//    property alias iconAnnotated: lblImage.annotated
//    property alias iconVisible: lblImage.iconVisible
//    property variant myModel: model

    signal clicked
    signal pressAndHold
    function setActive(newstate) {
        active=newstate
    }

//    clicked.connect(main.controlClicked)

    height: 40
    width: parent.width

    SelectionBackground {
        id: background
        visible: mouseArea.pressed
    }

//    PlayingHighlight {
//        id: itemHighLight
////        visible: parent.myHighlight
//        visible: false
//    }


//    LabelImage {
//        id: lblImage
//        anchors.left: parent.left
//    }

    MouseArea {
        id: mouseArea
        anchors.fill: background
        onClicked: {
            main.controlClicked(name)
            listItem.clicked()
        }

        onPressAndHold: {
            listItem.pressAndHold()
        }
    }
}
