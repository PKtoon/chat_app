import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.12 as Kirigami
import pkChat.MessageListModel 1.0

Kirigami.ScrollablePage {
    property PKMessageList messageListModel
    property ListView contactList
    title: contactList.currentUser+"'s Messages"

    mainAction: Kirigami.Action {
        iconName: "go-previous"
        onTriggered: root.pageStack.pop()
    }

    ListView {
        clip: true
        model: messageListModel
        delegate: ItemDelegate {
            width: ListView.view.width
            text: sender+"\n\n"+message
        }

    }

    footer: RowLayout {
        TextField {
            id: messageField
            Layout.minimumWidth: 2*parent.width/3
        }
        Button {
            Layout.minimumWidth: parent.width/3
            text: "Send"
            onClicked: {
                if (contactList.currentIndex !== -1 && messageField.text !== "" ) {
                    guiB.writer(contactList.currentIndex, messageField.text)
                }
                messageField.text = ""
            }
        }
    }
}
