// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtTest

import "../../components/mobileshell/qml/components" as MobileComponents

Item {
    width: 160
    height: 100

    Flickable {
        id: flickable
        width: parent.width
        height: parent.height
        contentWidth: width
        contentHeight: height * 2
        opacity: 0

        Rectangle {
            width: flickable.width
            height: flickable.contentHeight
            color: "red"
        }
    }

    MobileComponents.FlickableOpacityGradient {
        id: effect
        anchors.fill: flickable
        flickable: flickable
    }

    TestCase {
        name: "MaskProvider"
        when: windowShown

        function test_usesTextureProvider() {
            verify(effect.maskEnabled)
            verify(effect.maskSource.layer.enabled)
            compare(effect.maskSource.width, flickable.width)
            compare(effect.maskSource.height, flickable.height)
            compare(flickable.atYEnd, false)
            verify(waitForRendering(effect))
        }
    }
}
