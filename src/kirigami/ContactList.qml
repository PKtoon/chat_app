import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQml.Models 2.12

Item {
    property string currentUser
    property int currentIndex
    RowLayout {
        anchors.fill: parent
        Frame {
            Layout.fillHeight: true
            Layout.fillWidth: true
            ListView {
                id: contactListView
                model: guiB.contactListModel
                anchors.fill: parent
                clip: true
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
                        currentIndex = index
                        currentUser = name
                        guiB.currentUser(index)
                    }
                }
            }
        }
    }
}
