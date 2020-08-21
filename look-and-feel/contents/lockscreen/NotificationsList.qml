import QtQuick 2.12
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.12
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.notificationmanager 1.1 as Notifications
import "../components"

ListView {
    model: ListModel {
//         ListElement {
//             summary: "Heading"
//             body: "Text"
//         }
//         ListElement {
//             summary: "Heading"
//             body: "Text"
//         }
    }
    
    opacity: 1 - (passwordFlickable.contentY / passwordFlickable.columnHeight)
    spacing: units.gridUnit
    
    delegate: Rectangle {
        anchors {
            left: parent.left
            right: parent.right
        }
        radius: 3
        color: "white"
        z: 5
        height: notifLayout.height + units.gridUnit
        
        ColumnLayout {
            id: notifLayout
            anchors {
                left: parent.left
                leftMargin: units.gridUnit * 0.5
                right: parent.right
                rightMargin: units.gridUnit * 0.5
                verticalCenter: parent.verticalCenter
            }
            
            spacing: units.gridUnit / 2
            Label {
                text: model.summary
                color: "#212121"
            }
            Label {
                text: model.body
                color: "#616161"
            }
        }
        
        Component.onCompleted: {
            console.log(model.summary);
            console.log(notifLayout.height);
        }
    }
}
