import QtQuick 2.0
import QtQuick.Dialogs 1.2
import org.kde.kirigami 2.12 as Kirigami
import pkChat.GuiBridge 1.0
import pkChat.ContactListModel 1.0
import pkChat.MessageListModel 1.0

Kirigami.ApplicationWindow {
    id: root
    property ContactListView contactList

    color: Kirigami.Theme.Window

    globalDrawer: MenuArea{}
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
        onSetConnectInformSignal: {
            messageDialog.title="Connection Inform"
            messageDialog.text=text
            messageDialog.open()
            //            pageStack.pop()
        }
        onSetSignInUpInformSignal: {
            messageDialog.title="Auth Info"
            messageDialog.text=text
            messageDialog.open()
            //            pageStack.push(contactList)
        }
        onFindContactSuccessSignal: {
            messageDialog.title="Find Info"
            messageDialog.text="Added Contact"
            guiB.insertContact(text)
            messageDialog.open()
        }
        onFindContactFailureSignal: {
            messageDialog.title="Find Info"
            messageDialog.text=error
            messageDialog.open()
        }
        onMessageReceivedSignal: {
            if (root.contactList.contactView.currentUser === name)
                guiB.currentUser(root.contactList.contactView.currentIndex)
            root.contactList.contactView.positionViewAtEnd()
        }
    }

    pageStack.initialPage: ContactListView {
        id: contactList
        title:"Contacts"
        Component.onCompleted: root.contactList=contactList
        contactModel: pkContactList
    }

//    Component {
//        id: contactList
//        Kirigami.Page {
//            title: "Contacts"
//            property ListView list: contactListView
//            ContactListView {
//                id: contactListView
//                anchors.fill: parent
//                model: pkContactList
//            }
//        }
//    }

    Component {
        id: messageArea
        MessageArea {
            messageListModel: pkMessageList
            contactList: root.contactList.contactView
        }
    }

    MessageDialog {
        id: messageDialog
    }

}
