import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.1

Item {
    property Label informConnect: connectDialog.informConnect
    property Label informSignInUp: signInUpDialog.informSignInUp
    property Label informFindContact: findContactDialog.informFindContact
    property Button addButton: findContactDialog.addButton

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
                    onClicked: guiB.insertContact(findContactTextField.text)
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
}
