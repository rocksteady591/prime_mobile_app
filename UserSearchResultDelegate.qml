import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Item {
    id: root
    width: parent ? parent.width : 300
    height: 70

    // Свойства, которые передаем из модели поиска
    property string username: "Имя пользователя"
    property string userAvatar: ""
    signal clicked()

    Rectangle {
        anchors.fill: parent
        anchors.margins: 5
        radius: 12
        color: mouseArea.containsMouse ? "#181E2E" : "transparent" // Эффект наведения

        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 15

            // Заглушка под аватарку (круг с первой буквой)
            Rectangle {
                Layout.preferredWidth: 45
                Layout.preferredHeight: 45
                radius: 22.5
                color: "#2A82FF"

                Text {
                    anchors.centerIn: parent
                    text: username.length > 0 ? username[0].toUpperCase() : "?"
                    color: "white"
                    font.bold: true
                    font.pixelSize: 18
                }
            }

            // Информация о пользователе
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Text {
                    text: username
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    text: "Нажмите, чтобы написать сообщение"
                    color: "#8C92A4" // Приглушенный цвет для подписи
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: root.clicked()
        }
    }
}
