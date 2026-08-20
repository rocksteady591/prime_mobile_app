import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    width: parent.width
    height: parent.height

    Rectangle {
        anchors.fill: parent
        color: "#0F172A"

        // Главный контейнер расставляет элементы вертикально друг под другом
        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // ==========================================
            // 1. ХЕДЕР (Верхняя панель)
            // ==========================================
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 60 // Исправлено: preferredHeight (две буквы 'r')
                color: "#1C2333"

                RowLayout {
                    anchors.fill: parent   // Исправлено: anchors.fill (убрана лишняя 'l')
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20

                    Text {
                        text: "Чаты"
                        color: "white"
                        font.pixelSize: 22
                        font.bold: true
                    }

                    Item { Layout.fillWidth: true }

                    Button {
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                        background: Rectangle {
                            color: "#2A3447"
                            radius: 20
                        }
                        contentItem: Text {
                            text: "🔍"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            console.log("Кнопка поиска нажата")
                        }
                    }
                }
            } // <--- Хедер здесь корректно закрывается

            // ==========================================
            // 2. СПИСОК ЧАТОВ (Теперь на одном уровне с хедером)
            // ==========================================
            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true // Забирает всё оставшееся пространство посередине
                clip: true

                model: ListModel {
                    ListElement {
                        chatName: "Алексей"
                        lastMessage: "Привет, как продвигается проект с C++?"
                        time: "12:30"
                    }
                    ListElement {
                        chatName: "C++ Разработчики"
                        lastMessage: "Кто-нибудь разбирался с move семантикой?"
                        time: "Вчера"
                    }
                    ListElement {
                        chatName: "Анна"
                        lastMessage: "Документы по API отправила на почту."
                        time: "Пн"
                    }
                }

                delegate: Item {
                    width: parent.width
                    height: 75

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        spacing: 15

                        Rectangle {
                            Layout.preferredWidth: 50
                            Layout.preferredHeight: 50
                            radius: 25
                            color: "#3B82F6"

                            Text {
                                anchors.centerIn: parent
                                text: model.chatName[0]
                                color: "white"
                                font.bold: true
                                font.pixelSize: 18
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            Text {
                                text: model.chatName
                                color: "white"
                                font.bold: true
                                font.pixelSize: 16
                                Layout.fillWidth: true
                            }

                            Text {
                                text: model.lastMessage
                                color: "#8FA0C0"
                                font.pixelSize: 14
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }
                        }

                        Text {
                            text: model.time
                            color: "#8FA0C0"
                            font.pixelSize: 12
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log("Кликнули на чат с: " + model.chatName)
                        }
                    }
                }
            } // <--- Список здесь закрывается

            // ==========================================
            // 3. НИЖНЯЯ ПАНЕЛЬ НАВИГАЦИИ (Футер)
            // ==========================================
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 65
                color: "#1C2333"

                RowLayout {
                    anchors.fill: parent

                    Button {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        background: Rectangle { color: "transparent" }
                        contentItem: Text {
                            text: "💬 Чаты"
                            color: "#2A82FF"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        background: Rectangle { color: "transparent" }
                        contentItem: Text {
                            text: "⚙️ Настройки"
                            color: "#8FA0C0"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            console.log("Переход в настройки")
                        }
                    }
                }
            } // <--- Футер здесь закрывается

        } // Закрытие главного ColumnLayout
    }
}
