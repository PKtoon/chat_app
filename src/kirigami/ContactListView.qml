import QtQuick 2.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.12 as Kirigami
import pkChat.ContactListModel 1.0

Kirigami.Page {
    property PKContact contactModel
    ListView {
        id: contactView
        model: contactModel
        clip: true
        anchors.fill: parent
        highlightFollowsCurrentItem: true
        property string currentUser
        delegate: ItemDelegate {
            width: ListView.view.width
            text: name
            onClicked: {
                contactView.currentIndex = index
                contactView.currentUser=name
                root.pageStack.push(messageArea)
                guiB.currentUser(index)
            }
        }
    }

}
