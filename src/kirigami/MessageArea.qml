import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    property ListView messagesListView: messageList
    ColumnLayout {
        anchors.fill: parent
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 2*Layout.height/3
            ListView {
                id: messageList
                anchors.fill: parent
                model: guiB.messageListModel
                highlightFollowsCurrentItem: true
                clip: true
                spacing: 15
                delegate: ItemDelegate {
                    width: ListView.view.width
                    height: subjectText.contentHeight+messageText.contentHeight+10
                    ColumnLayout {
                        anchors.fill: parent
                        Text {
                            id: subjectText
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: sender + ":"
                        }
                        Text {
                            id: messageText
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: message
                        }
                    }
                }
            }

        }
        GroupBox {
            Layout.fillWidth: true
            Layout.minimumHeight: Layout.height/3
            RowLayout {
                anchors.fill: parent
                TextField {
                    id: messageField
                    Layout.fillWidth: true
                }
                Button {
                    text: "Send"
                    Layout.minimumWidth: parent.width/3
                    onClicked: {
                        if (guiB.contactListModel.currentIndex !== -1 && messageField.text !== "" ) {
                            guiB.writer(guiB.contactListModel.currentIndex, messageField.text)
                        }
                        messageField.text = ""
                    }
                }
            }
        }
    }
}
