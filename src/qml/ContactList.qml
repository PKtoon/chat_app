import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQml.Models 2.12
import pkChat.ContactListModel 1.0

Item {
    property PKContact listModel
    property ListView contactList: contactList
    RowLayout {
        anchors.fill: parent
        Frame {
            Layout.fillHeight: true
            Layout.fillWidth: true
            ListView {
                id: contactList
                model: listModel
                anchors.fill: parent
                clip: true
                property string currentUser
                delegate: ItemDelegate {
                    width: ListView.view.width
                    highlighted: ListView.isCurrentItem
                    Rectangle {
                        //can be used for dp
                        width: 10
                        height: 10
                        color: "yellow"
                    }
                    text: name
                    onClicked: {
                        contactList.currentIndex = index
                        contactList.currentUser = name
                        guiB.currentUser(index)
                    }
                }
            }
        }
    }
}
