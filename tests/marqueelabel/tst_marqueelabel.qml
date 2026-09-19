// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtTest
import "../../components/mobileshell/qml/components" as Components

Item {
    id: scene
    width: 480
    height: 240

    Component {
        id: labelComponent
        Components.MarqueeLabel {
            width: 100
            inputText: "A long quick-setting label that needs to scroll"
            textFormat: Text.PlainText
        }
    }

    Component {
        id: containerComponent
        Item {}
    }

    Component {
        id: windowComponent
        Window {
            width: 300
            height: 100
            visible: true
        }
    }

    TestCase {
        name: "MarqueeLabel"
        when: windowShown

        function animationFor(label) {
            const animation = findChild(label, "marqueeTextAnimation");
            verify(animation !== null);
            return animation;
        }

        function test_scrollingControlStopsAndRestartsAnimation() {
            const label = createTemporaryObject(labelComponent, scene);
            verify(label);
            const animation = animationFor(label);
            tryCompare(animation, "running", true);

            label.scrollingEnabled = false;
            compare(label.shouldAnimate, false);
            compare(animation.running, false);
            label.inputText += " changed while panel is hidden";
            compare(animation.running, false);

            label.scrollingEnabled = true;
            tryCompare(animation, "running", true);
            label.inputText += " changed while panel is visible";
            compare(animation.running, true);
        }

        function test_hiddenAncestorStopsAnimation() {
            const container = createTemporaryObject(containerComponent, scene);
            verify(container);
            const label = createTemporaryObject(labelComponent, container);
            verify(label);
            const animation = animationFor(label);
            tryCompare(animation, "running", true);
            container.visible = false;
            compare(animation.running, false);
            container.visible = true;
            tryCompare(animation, "running", true);
        }

        function test_hiddenWindowStopsAnimation() {
            const window = createTemporaryObject(windowComponent, null);
            verify(window);
            const label = createTemporaryObject(labelComponent, window.contentItem);
            verify(label);
            const animation = animationFor(label);
            tryCompare(animation, "running", true);
            window.visible = false;
            compare(label.shouldAnimate, false);
            compare(animation.running, false);
            window.visible = true;
            tryCompare(animation, "running", true);
        }

        function test_unattachedLabelDoesNotAnimate() {
            const label = createTemporaryObject(labelComponent, null);
            verify(label);
            verify(label.charactersOverflowing);
            compare(label.shouldAnimate, false);
            compare(animationFor(label).running, false);
        }

        function test_metricsAndOverflowRespondToChanges() {
            const label = createTemporaryObject(labelComponent, scene);
            verify(label);
            const animation = animationFor(label);
            tryCompare(animation, "running", true);
            const initialAdvance = label.textAdvanceWidth;

            label.width = initialAdvance + 100;
            compare(label.charactersOverflowing, false);
            compare(animation.running, false);
            label.width = 100;
            tryCompare(animation, "running", true);

            label.inputText = "Short\ntext";
            compare(label.filteredText, "Short text");
            verify(label.textAdvanceWidth < initialAdvance);
            label.font.pixelSize = 40;
            tryCompare(animation, "running", true);
        }

        function test_maskingOnlyForOverflowingText() {
            const label = createTemporaryObject(labelComponent, scene, { inputText: "Short", width: 300 });
            verify(label);
            const animation = animationFor(label);
            compare(label.charactersOverflowing, false);
            compare(label.layer.enabled, false);
            compare(animation.running, false);

            label.width = 10;
            tryCompare(label.layer, "enabled", true);
            tryCompare(animation, "running", true);

            label.scrollingEnabled = false;
            compare(animation.running, false);
            // Text that still overflows must stay clipped and masked even when scrolling is disabled.
            compare(label.layer.enabled, true);

            label.width = 300;
            tryCompare(label.layer, "enabled", false);
            label.inputText = "";
            compare(label.layer.enabled, false);
        }
    }
}
