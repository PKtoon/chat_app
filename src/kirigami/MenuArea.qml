import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.1
import org.kde.kirigami 2.12 as Kirigami

Kirigami.GlobalDrawer {
    title: "Menu"
    actions: [
        Kirigami.Action {
            text: "Connect"
            onTriggered: root.pageStack.push(connectDialog)
        },
        Kirigami.Action {
            text: "Sign In"
            onTriggered: root.pageStack.push(signInUpDialog)
        },
        Kirigami.Action {
            text: "Find Contact"
            onTriggered: root.pageStack.push(findContactDialog)
        }
    ]

    Component {
        id: connectDialog
        Kirigami.Page {
            id: connectPage
            mainAction: Kirigami.Action {
                iconName: "go-home"
                onTriggered: root.pageStack.pop()
            }
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
                        Layout.columnSpan: 2
                        text: "Connect"
                        onClicked: guiB.connect(hostName.text,port.text);
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
    }

    Component {
        id: signInUpDialog
        Kirigami.Page {
            title: "SignIn/Up"
            mainAction: Kirigami.Action {
                iconName: "go-home"
                onTriggered: root.pageStack.pop()
            }
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
    }

    Component {
        id: findContactDialog
        Kirigami.Page {
            property Label informFindContact: informFindContact
            property Button addButton: addButton
            mainAction: Kirigami.Action {
                iconName: "go-home"
                text: "Back"
                onTriggered: root.pageStack.pop()
            }
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
}
