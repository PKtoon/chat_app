import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import pkChat.GuiBridge 1.0
import pkChat.ContactListModel 1.0
import pkChat.MessageListModel 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true

    menuBar: MenusArea{}

    PKContact {
        id: pkContactList
    }
    PKMessageList {
        id: pkMessageList
    }

    PKGui {
        id: guiB
        contactListModel: pkContactList
        messageListModel: pkMessageList
        onSetConnectInformSignal: menuBar.informConnect.text = text
        onSetSignInUpInformSignal: menuBar.informSignInUp.text = text
        onFindContactSuccessSignal: {
            if(menuBar.findContactTextField.text === text)
            {
                menuBar.addButton.enabled=true
                menuBar.informFindContact.text = text+" found"
            }
        }

        onFindContactFailureSignal: menuBar.informFindContact.text = error
        onFindGroupSuccessSignal: {
            if(menuBar.findGroupTextField.text === text)
            {
                menuBar.addGroupButton.enabled=true
                menuBar.informFindGroup.text = text+" found"
            }
        }

        onFindGroupFailureSignal: menuBar.informFindGroup.text = error
        onMessageReceivedSignal: {
            if (contactView.contactList.currentUser === name)
                guiB.currentUser(contactView.contactList.currentIndex)
                contactView.contactList.positionViewAtEnd()
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.topMargin: 50
        ContactList {
            id: contactView
            Layout.fillHeight: true
            Layout.minimumWidth: parent.width/3
            listModel: pkContactList
        }
        MessageArea {
            messageList: pkMessageList
            contactList: contactView.contactList
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: 2*parent.width/3
        }
    }
}
