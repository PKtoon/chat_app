import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import pkChat.GuiBridge 1.0
import org.kde.kirigami 2.12 as Kirigami

ApplicationWindow {
    width: 640
    height: 480
    visible: true

    menuBar: MenusArea{}

    PKGui {
        id: guiB

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
            if (contactView.currentUser === name)
            {
                guiB.currentUser(contactView.currentIndex)
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.topMargin: 50
        ContactList {
            id: contactView
            Layout.fillHeight: true
            Layout.minimumWidth: parent.width/3
        }
        MessageArea {
            id: messageView
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: 2*parent.width/3
        }
    }
}
