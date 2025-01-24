import QtQuick
import QtQuick.Controls

Window {
    id: root

    property int fadeOutWait: 1500
    property int fadeDuration: 400
    property real darkOpacity: 0.6
    property real translucentOpacity: 0.0
    property int unlockDuration: 400
    property int bgMargin: 20

    flags: Qt.ToolTip | Qt.FramelessWindowHint | Qt.WA_TranslucentBackground

    color: "#00000000"
    width: 800
    height: 600
    visible: true
    visibility: Window.FullScreen

    // Image {
    //     source: "wallpaper.png"
    //     anchors.fill: parent
    // }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log("clicked")
            background.opacity = darkOpacity;
            unlockSlider.opacity = 1.0
            background.scale = 1.0
            bgFadeOutTimer.start()
        }
    }

    Timer {
        id: bgFadeOutTimer
        interval: fadeOutWait
        running: false
        onTriggered: {
            console.log("bgFadeOutTimer triggered");
            if (unlockSlider.pressed) {
                console.log("slider pressed.");
                return;
            } else {
                console.log("slider not pressed, fading out");
            }
            background.opacity = translucentOpacity;
            unlockSlider.opacity = translucentOpacity;
        }
    }

    Rectangle {
        id: background
        //property int m: 20
        anchors.fill: parent
        anchors.margins: root.bgMargin
        radius: root.bgMargin
        color: "#000000"
        opacity: translucentOpacity
        Behavior on opacity {
            NumberAnimation { duration: fadeDuration }
        }
    }

    Rectangle {
        id: locked_background

        anchors.fill: parent
        anchors.margins: root.bgMargin
        radius: root.bgMargin
        color: "#000000"
        opacity: 0.0

        Label {
            id: lockText
            anchors.centerIn: parent
            // anchors.bottom: unlockSlider.top
            //anchors.bottomMargin: unlockSlider.sliderHeight
            font.pointSize: unlockSlider.sliderHeight / 3
            text: "Touchscreen Locked"
            color: "white"
            // opacity: 0.0
            // Behavior on opacity {
            //     NumberAnimation { duration: fadeDuration / 2 }
            // }
        }
    }

    SequentialAnimation {
        id: lockAnimation
        NumberAnimation { target: locked_background; property: "opacity"; to: 0.0; duration: fadeDuration * 2 }
        ParallelAnimation {
            NumberAnimation { target: locked_background; property: "scale"; from: 1.2; to: 1.0; duration: fadeDuration * 2 }
            NumberAnimation { target: locked_background; property: "opacity"; to: 0.8; duration: fadeDuration * 2 }
        }
        ParallelAnimation {
            NumberAnimation { target: locked_background; property: "scale"; to: 2.5; duration: fadeDuration }
            NumberAnimation { target: locked_background; property: "opacity"; to: 0.0; duration: fadeDuration }
        }
        //NumberAnimation { target: unlockText; property: "opacity"; to: 0; duration: fadeDuration / 2  }
    }

    ParallelAnimation {
        id: unlockAnimation
        NumberAnimation { target: background; property: "opacity"; to: 0; duration: unlockDuration }
        NumberAnimation { target: background; property: "scale"; to: 0; duration: unlockDuration }
        NumberAnimation { target: unlockText; property: "opacity"; to: 0; duration: unlockDuration / 2  }
    }

    Timer {
        id: unlockTimer
        interval: 300
        running: false
        onTriggered: {
            unlockText.opacity = 0;
            unlockSlider.opacity = 0;
            unlockAnimation.running = true;
        }
    }

    Timer {
        id: resetTimer
        running: false
        interval: unlockDuration * 1.5
        onTriggered: {
            console.log("reset / quit.")
            background.opacity = 0.0
            background.scale = 1.0
            unlockSlider.value = 0
            unlockText.opacity = 0.0

            console.log("Destroying LockTouchScreen.");
            root.visible = false;
            console.log("rp: "+ root.parent);
            if (root.parent != null) {
                root.destroy();
            } else {
                Qt.quit();
            }
        }
    }

    Slider {
        id: unlockSlider

        opacity: translucentOpacity
        //transformOrigin: Item.BottomRight

        Behavior on opacity {
            NumberAnimation { duration: fadeDuration / 2 }
        }

        x: (root.width - unlockSlider.width) / 2
        y: root.height * 0.7
        width: root.width * 0.4
        height: 200

        onPressedChanged: {
            if (unlockSlider.value === 1) {
                console.log("unlocking...: " + unlockSlider.value);
                unlockText.opacity = 1.0;
                bgFadeOutTimer.stop();
                unlockTimer.start();
                resetTimer.start();
            } else {
                console.log("keeping locked, value: " + unlockSlider.value);
                unlockSlider.value = 0;
                bgFadeOutTimer.restart()
            }
        }

        property int sliderHeight: 80
        property int sliderRadius: unlockSlider.sliderHeight / 2

        background: Rectangle {
            x: unlockSlider.leftPadding
            y: unlockSlider.topPadding + unlockSlider.availableHeight / 2 - height / 2
            implicitWidth: 200
            implicitHeight: unlockSlider.sliderHeight
            width: unlockSlider.availableWidth
            height: implicitHeight
            radius: unlockSlider.sliderRadius
            color: "#bdbebf"
            opacity: 0.7

            Label {
                text: "Slide to Unlock"
                color: "black"
                opacity: 1.0 - unlockSlider.value
                font.pixelSize: unlockSlider.sliderHeight / 3
                anchors.centerIn: parent
            }

            Rectangle {
                width: unlockSlider.visualPosition * parent.width
                height: parent.height
                opacity: unlockSlider.value
                color: "#21be2b"
                radius: unlockSlider.sliderRadius
            }
        }

        handle: Rectangle {
            x: unlockSlider.leftPadding + unlockSlider.visualPosition * (unlockSlider.availableWidth - width)
            y: unlockSlider.topPadding + unlockSlider.availableHeight / 2 - height / 2
            implicitWidth: unlockSlider.sliderHeight
            implicitHeight: unlockSlider.sliderHeight
            radius: unlockSlider.sliderRadius
            color: unlockSlider.pressed ? "#f0f0f0" : "#f6f6f6"
            border.color: "#bdbebf"
        }

    }

    Label {
        id: unlockText
        anchors.horizontalCenter: unlockSlider.horizontalCenter
        anchors.bottom: unlockSlider.top
        //anchors.bottomMargin: unlockSlider.sliderHeight
        font.pointSize: unlockSlider.sliderHeight / 3
        text: "Unlocking."
        color: "white"
        opacity: 0.0
        Behavior on opacity {
            NumberAnimation { duration: fadeDuration / 2 }
        }
    }

    Component.onCompleted: {
        console.log("Created LockTouchScreen.")
        lockAnimation.running = true;

    }
}
