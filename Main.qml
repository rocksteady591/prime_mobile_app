import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic


ApplicationWindow {
    id: window
    width: 640
    height: 480
    minimumWidth: 200
    minimumHeight: 250
    visible: true
    title: qsTr("Prime")
    color: "#0F1626"

    Rectangle{
        anchors.fill: parent
        color: "#0B0F19"
        // Декоративные элементы фона (имитация неоновых отсветов)
        Rectangle {
            width: 300; height: 300
            radius: 150
            color: "#2A82FF"
            opacity: 0.1
            anchors { top: parent.top; left: parent.left; margins: -50 }
        }
        Rectangle {
            width: 400; height: 400
            radius: 200
            color: "#FF6B6B"
            opacity: 0.05
            anchors { bottom: parent.bottom; right: parent.right; margins: -100 }
        }
    }

    // Менеджер экранов
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: "LoginScreen.qml" // Первый экран при запуске
    }


    /*TabBar {
           id: tabBar
            width: parent.width
            anchors.bottom: parent.bottom
            background: Rectangle { color: "#181E2E" }

            TabButton { text: "Chats"; icon.source: "qrc:/icons/chat.svg" }
            TabButton { text: "Contacts"; icon.source: "qrc:/icons/contacts.svg" }
            TabButton { text: "Calls"; icon.source: "qrc:/icons/calls.svg" }
            TabButton { text: "Settings"; icon.source: "qrc:/icons/settings.svg" }
        }*/

}
