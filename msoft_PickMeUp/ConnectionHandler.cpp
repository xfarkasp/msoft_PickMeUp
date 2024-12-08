#include "AutomatController.h"
#include "ConnectionHandler.h"
#include "DataHandler.h"
#include <QStackedWidget>
#include <QPixmap>
#include "Navigation.h"

// Find a GuI page index to switch to (useful when you don't know the page indxes anymore)
int32_t ConnectionHandler::findWidget(std::string widgetName, QWidget* parent)
{
    int targetIndex = -1;
    QList<QWidget*> widgets = parent->findChildren<QWidget*>();
    QStackedWidget* stackedWidget = parent->findChild<QStackedWidget*>();
    QList<QWidget*>::const_iterator it;
    for (it = widgets.begin(); it != widgets.end(); ++it) {
        QWidget* widget = *it;
        if (widget->objectName() == widgetName) {
            return stackedWidget->indexOf(widget);
        }
    }
}

// Cereate the connection gui
QWidget* ConnectionHandler::connectionGui(int32_t id, ConnectionType conType, QWidget* parent) {

    QWidget* connectionPage = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(connectionPage);
    QLabel* messageLabel = new QLabel("Click the button to connect to Automat", connectionPage);
    
    QPushButton* connectButton = new QPushButton("Connect", connectionPage);
    QPushButton* confirmButton = new QPushButton("Confirm", connectionPage);
    QPushButton* mainMenuButton = new QPushButton("Main Menu", connectionPage);
    QPushButton* simulateError = new QPushButton("Simulate full", connectionPage);
    confirmButton->setVisible(false);
    mainMenuButton->setVisible(false);

    connect(simulateError, &QPushButton::clicked, [=]() 
        {
            QString message = QString("Automat is full, plese proceed to the nearest automat.");
            QMessageBox::information(nullptr, "Warrning", message);
            messageLabel->setText("Please move to the nearest automat!\n");
            Navigation::navigate();
        });

    // Simulate connection to the automat
    connect(connectButton, &QPushButton::clicked, [=]() mutable {
        DataHandler& dh = DataHandler::getInstance();
        DataHandler::Task* task = dh.getTask(id);
        if (conType == ConnectionType::Delivery)
        {
            id = task->m_objectId;
            Navigation::navigate();

        }
        DataHandler::Parcel parcel = dh.getParcel(id);
        // If the locker is opened by a courier
        if (parcel.m_status == DataHandler::ParcelStatus::Sent)
        {
            Navigation::navigate();
            messageLabel->setText("Connected to Automat!\n Pick up parcel.");
        }
        else
        {
            // When inserting a new package
            messageLabel->setText("Connected to Automat!\n Insert your package and click on confirm button.");
        }
        confirmButton->setVisible(true);
        connectButton->setVisible(false);
        });

    // Simulate Inserting a package
    connect(confirmButton, &QPushButton::clicked, [=]() mutable {
        // Get the package info from the database
        DataHandler& dh = DataHandler::getInstance();
        DataHandler::Task* task = dh.getTask(id);
        if (conType == ConnectionType::Delivery)
        {
            id = task->m_objectId;
        }
        DataHandler::Parcel& parcel = dh.getParcel(id);
        // When courier is inserting a package
        if (parcel.m_status == DataHandler::ParcelStatus::Sent)
        {
            QString message = QString("Navigation system is navigating you to your destination.");
            Navigation::navigate();
            QMessageBox::information(nullptr, "Task Details", message);
            messageLabel->setText("Connected to Automat!\n Insert delivered package.");
            parcel.setState(DataHandler::ParcelStatus::Arrived);
        }
        // When courier delivered the package
        else if (parcel.m_status == DataHandler::ParcelStatus::Arrived)
        {
            messageLabel->setText("Delivery finished");
            confirmButton->setVisible(false);
            mainMenuButton->setVisible(true);
        }
        // When the user inserted a new package to the automat
        else
        {
            messageLabel->setText("Package is to be delivered");
            confirmButton->setVisible(false);
            mainMenuButton->setVisible(true);
        }
        
        });
    connect(mainMenuButton, &QPushButton::clicked, [=]() mutable {
        messageLabel->setText("Click the button to connect to Automat");
        connectButton->setVisible(true);
        confirmButton->setVisible(false);
        mainMenuButton->setVisible(false);
        // get package info from the database
        DataHandler& dh = DataHandler::getInstance();
        DataHandler::Task* task = dh.getTask(id);
        if (conType == ConnectionType::Delivery)
        {
            id = task->m_objectId;
        }
        DataHandler::Parcel& parcel = dh.getParcel(id);
        
        QStackedWidget* stackedWidget = parent->findChild<QStackedWidget*>();
        if (!stackedWidget)
        {
            return;
        }

        // Record an newly inserted package to the system, to indicate waiting for delivery
        if (parcel.m_status == DataHandler::ParcelStatus::Created)
        {
            auto automat = connectAutomat();
            // Check if the package was succesfuly recorded to be delivered
            if (!automat.recordPackage(id))
            {
                return;
            }
            // Return to main menu
            stackedWidget->setCurrentIndex(findWidget("BasicMenu", parent));
        }
        else
        {
            // Return to main menu
            task->m_taskStatus = DataHandler::TaskStatus::Completed;
            stackedWidget->setCurrentIndex(findWidget("FreelanceCourierMenu", parent));
        }
        
        });

    layout->addWidget(messageLabel);
    layout->addWidget(connectButton);
    layout->addWidget(confirmButton);
    layout->addWidget(mainMenuButton);

    return connectionPage;
}