#include "AutomatController.h"
#include "ConnectionHandler.h"
#include "DataHandler.h"
#include <QStackedWidget>

int findWidget(std::string widgetName, QWidget* parent)
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

QWidget* ConnectionHandler::connectionGui(int32_t id, QWidget* parent) {

    QWidget* connectionPage = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(connectionPage);
    QLabel* messageLabel = new QLabel("Click the button to connect to Automat", connectionPage);
    messageLabel->setAlignment(Qt::AlignCenter);

    QPushButton* connectButton = new QPushButton("Connect", connectionPage);
    QPushButton* confirmButton = new QPushButton("Confirm", connectionPage);
    QPushButton* mainMenuButton = new QPushButton("Main Menu", connectionPage);
    confirmButton->setVisible(false);
    mainMenuButton->setVisible(false);

    connect(connectButton, &QPushButton::clicked, [=]() {
        DataHandler::Parcel parcel = DataHandler::getInstance().getParcel(id);
        if (parcel.m_status == DataHandler::ParcelStatus::Sent)
        {
            messageLabel->setText("Connected to Automat!\n Pick up parcel.");
        }
        else
        {
            messageLabel->setText("Connected to Automat!\n Insert your package and click on confirm button.");
        }
        confirmButton->setVisible(true);
        connectButton->setVisible(false);
        });

    connect(confirmButton, &QPushButton::clicked, [=]() {
        DataHandler::Parcel& parcel = DataHandler::getInstance().getParcel(id);
        if (parcel.m_status == DataHandler::ParcelStatus::Sent)
        {
            QString message = QString("Navigation system is navigating you to your destination.");
            QMessageBox::information(nullptr, "Task Details", message);
            messageLabel->setText("Connected to Automat!\n Insert delivered package.");
            parcel.setState(DataHandler::ParcelStatus::Arrived);
        }
        else if (parcel.m_status == DataHandler::ParcelStatus::Arrived)
        {
            messageLabel->setText("Delivery finished");
            confirmButton->setVisible(false);
            mainMenuButton->setVisible(true);
        }
        else
        {
            messageLabel->setText("Package is to be delivered");
            confirmButton->setVisible(false);
            mainMenuButton->setVisible(true);
        }
        
        });
    connect(mainMenuButton, &QPushButton::clicked, [=]() {
        messageLabel->setText("Click the button to connect to Automat");
        connectButton->setVisible(true);
        confirmButton->setVisible(false);
        mainMenuButton->setVisible(false);

        DataHandler::Parcel& parcel = DataHandler::getInstance().getParcel(id);

        QStackedWidget* stackedWidget = parent->findChild<QStackedWidget*>();
        if (!stackedWidget)
        {
            return;
        }

        if (parcel.m_status == DataHandler::ParcelStatus::Created)
        {
            auto automat = connectAutomat();
            if (!automat.recordPackage(id))
            {
                return;
            }
            stackedWidget->setCurrentIndex(findWidget("BasicMenu", parent));
        }
        else
        {
            stackedWidget->setCurrentIndex(findWidget("FreelanceCourierMenu", parent));
        }
        
        });

    layout->addWidget(messageLabel);
    layout->addWidget(connectButton);
    layout->addWidget(confirmButton);
    layout->addWidget(mainMenuButton);

    return connectionPage;
}