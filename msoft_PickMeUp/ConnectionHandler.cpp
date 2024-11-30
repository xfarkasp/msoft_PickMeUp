#include "AutomatController.h"
#include "ConnectionHandler.h"
#include "DataHandler.h"
#include <QStackedWidget>

QWidget* ConnectionHandler::connectionGui(int32_t id, QWidget* parent) {

    // Create the main widget for the page
    QWidget* connectionPage = new QWidget(parent);

    // Set up layout
    QVBoxLayout* layout = new QVBoxLayout(connectionPage);

    // Message label
    QLabel* messageLabel = new QLabel("Click the button to connect to Automat", connectionPage);
    messageLabel->setAlignment(Qt::AlignCenter);

    // Connect button
    QPushButton* connectButton = new QPushButton("Connect", connectionPage);
    QPushButton* confirmButton = new QPushButton("Confirm", connectionPage);
    QPushButton* mainMenuButton = new QPushButton("Main Menu", connectionPage);
    confirmButton->setVisible(false);
    mainMenuButton->setVisible(false);
    
    // Update the label when the button is clicked
    connect(connectButton, &QPushButton::clicked, [=]() {
        messageLabel->setText("Connected to Automat!\n Insert your package and click on confirm button.");
        confirmButton->setVisible(true);
        connectButton->setVisible(false);
        });

    connect(confirmButton, &QPushButton::clicked, [=]() {
        messageLabel->setText("Package is to be delivered");
        confirmButton->setVisible(false);
        mainMenuButton->setVisible(true);
        });
    connect(mainMenuButton, &QPushButton::clicked, [=]() {

        auto automat = connectAutomat();
        if (!automat.recordPackage(id))
        {
            return;
        }

        QStackedWidget* stackedWidget = parent->findChild<QStackedWidget*>();
        if (!stackedWidget) return;
        stackedWidget->setCurrentIndex(2);
        messageLabel->setText("Click the button to connect to Automat");
        connectButton->setVisible(true);
        confirmButton->setVisible(false);
        mainMenuButton->setVisible(false);
        });

    // Add widgets to layout
    layout->addWidget(messageLabel);
    layout->addWidget(connectButton);
    layout->addWidget(confirmButton);
    layout->addWidget(mainMenuButton);

    return connectionPage;
}