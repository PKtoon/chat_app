import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.1

Item {
    property Label informConnect: connectDialog.informConnect
    property Label informSignInUp: signInUpDialog.informSignInUp

    property Label informFindContact: findContactDialog.informFindContact
    property Button addButton: findContactDialog.addButton
    property TextField findContactTextField: findContactDialog.findContactTextField

    property Label informFindGroup: findGroupDialog.informFindGroup
    property Button addGroupButton: findGroupDialog.addGroupButton
    property TextField findGroupTextField: findGroupDialog.findGroupTextField

    Action {
        id: connectAction
        text: "C&onnect"
        onTriggered: connectDialog.open()
    }
    Action {
        id: signInUpAction
        text: "&SignIn/Up"
        onTriggered: signInUpDialog.open()
    }
    Action {
        id: findContactAction
        text: "&Find Contact"
        onTriggered: findContactDialog.open()
    }
    Action {
        id: findGroupAction
        text: "&Find Group"
        onTriggered: findGroupDialog.open()
    }
    Action {
        id: createGroupAction
        text: "Create Group"
        onTriggered: createGroupDialog.open()
    }

    MenuBar {
        Layout.fillWidth: true
        Menu {
            title: "&Connect"
            MenuItem { action: connectAction }
        }
        Menu {
            title: "&User"
            MenuItem { action: signInUpAction }
        }
        Menu {
            title: "&Message"
            MenuItem { action: findContactAction }
            MenuItem { action: findGroupAction }
            MenuItem { action: createGroupAction }
        }
    }

    Dialog {
        id: connectDialog
        property Label informConnect: informConnect
        title: "Connect"
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                Label {
                    Layout.minimumWidth: Layout.width/2
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                    text: "Host"
                }
                Label {
                    Layout.minimumWidth: Layout.width/2
                    Layout.alignment: Qt.AlignRight
                    text: "Port"
                }
            }
            RowLayout {
                Layout.fillWidth: true
                TextField {
                    id: hostName
                    Layout.minimumWidth: Layout.width/2
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignCenter
                    text: "localhost"
                }
                TextField {
                    id: port
                    Layout.minimumWidth: Layout.width/2
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignCenter
                    text: "1098"
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Button {
                    Layout.alignment: Qt.AlignCenter
                    Layout.minimumWidth: Layout.width/2
                    Layout.fillWidth: true
                    text: "Connect"
                    onClicked: guiB.connect(hostName.text,port.text);
                }
                Button {
                    Layout.alignment: Qt.AlignCenter
                    Layout.minimumWidth: Layout.width/2
                    Layout.fillWidth: true
                    text: "Cancel"
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                Label {
                    id: informConnect
                    Layout.alignment: Qt.AlignCenter
                    text: ""

                }
            }
        }
    }

    Dialog {
        id: signInUpDialog
        title: "SignIn/Up"
        property Label informSignInUp: informSignInUp
        ColumnLayout {
            spacing: 10
            RowLayout {
                spacing: 10
                ColumnLayout {
                    Label {
                        Layout.alignment: Qt.AlignCenter
                        text: "Username"
                    }
                    TextField {
                        id: usernameTextField
                        Layout.alignment: Qt.AlignCenter
                        text: "pk"
                    }
                    Button {
                        Layout.alignment: Qt.AlignCenter
                        Layout.fillWidth: true
                        text: "Sign Up"
                        onClicked: guiB.initSignUp(usernameTextField.text,passwordTextField.text)
                    }
                }
                ColumnLayout {
                    Label {
                        Layout.alignment: Qt.AlignCenter
                        text: "Password"
                    }
                    TextField {
                        id: passwordTextField
                        Layout.alignment: Qt.AlignCenter
                        text: "111"
                    }
                    Button {
                        Layout.alignment: Qt.AlignCenter
                        Layout.fillWidth: true
                        text: "Sign In"
                        onClicked: guiB.initSignIn(usernameTextField.text,passwordTextField.text)
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                Label {
                    id: informSignInUp
                    Layout.alignment: Qt.AlignCenter
                    text: ""

                }
            }
        }
    }

    Dialog {
        id: findContactDialog
        property Label informFindContact: informFindContact
        property Button addButton: addButton
        property TextField findContactTextField: findContactTextField
        ColumnLayout {
            anchors.fill: parent
            Label {
                Layout.alignment: Qt.AlignCenter
                text: "Contact Name"
            }
            TextField {
                id: findContactTextField
                Layout.alignment: Qt.AlignCenter
            }
            RowLayout {
                Button {
                    id: addButton
                    text: "Add"
                    enabled: false
                    onClicked: {
                        informFindContact.text="Contact Added"
                        guiB.insertContact(findContactTextField.text)
                    }
                }
                Button {
                    text: "Find"
                    onClicked: guiB.findContact(findContactTextField.text)
                }
            }
            Label {
                id: informFindContact
                Layout.alignment: Qt.AlignCenter
                text: ""
            }
        }
    }
    Dialog {
        id: findGroupDialog
        property Label informFindGroup: informFindGroup
        property Button addGroupButton: addGroupButton
        property TextField findGroupTextField: findGroupTextField
        ColumnLayout {
            anchors.fill: parent
            Label {
                Layout.alignment: Qt.AlignCenter
                text: "Group Name"
            }
            TextField {
                id: findGroupTextField
                Layout.alignment: Qt.AlignCenter
            }
            RowLayout {
                Button {
                    id: addGroupButton
                    text: "Add"
                    enabled: false
                    onClicked: {
                        informFindGroup.text="Group Added"
                        guiB.insertGroup(findGroupTextField.text)
                    }
                }
                Button {
                    text: "Find"
                    onClicked: guiB.findGroup(findGroupTextField.text)
                }
            }
            Label {
                id: informFindGroup
                Layout.alignment: Qt.AlignCenter
                text: ""
            }
        }
    }

    Dialog {
        id: createGroupDialog
        width: 500
        height: 500
        ColumnLayout
        {
            anchors.fill: parent
            Label {
                text: "Create group"
                Layout.alignment: Qt.AlignHCenter
            }

        GroupBox {
            //implicitHeight: parent.height * 2/3
            Layout.fillHeight: true
            Layout.fillWidth: true
            ListView {
                model: guiB.contactListModel
                anchors.fill: parent
                delegate: ItemDelegate {
                    width: ListView.width
                    visible: (type == 1) ? true : false
                    RowLayout{
                        CheckBox {
                            checked: false
                        }
                        Label {
                            text: name
                        }
                    }
                }
            }
        }
}

        /*height: 500
        width: 500
            GroupList {
                anchors.fill: parent
                contactList: contactList
            }*/
    }
}
