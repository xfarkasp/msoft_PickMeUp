#include "FormularHandler.h"
#include "DataHandler.h"
#include <QStackedWidget>
#include "ConnectionHandler.h"

// Metod returns a form creation instance based on the type of the requested form
QWidget* FormularHandler::getForm(FormType type, QWidget* parent)
{
    switch (type)
    {
    case FormType::SendParcel:
        return createSendParcelForm(parent);
    case FormType::RequestRepair:
        return createRepairTaskForm(parent);
    default:
        return nullptr;
    }
}

// Creates a from for creating a new parcel to send
QWidget* FormularHandler::createSendParcelForm(QWidget* parent)
{
    // Create the gui elements
    QWidget* form = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(form);

    QLabel* senderLabel = new QLabel("Sender Name:", form);
    QLineEdit* senderInput = new QLineEdit(form);
    senderInput->setObjectName("SenderInput");
    layout->addWidget(senderLabel);
    layout->addWidget(senderInput);

    QLabel* recipientLabel = new QLabel("Recipient Name:", form);
    QLineEdit* recipientInput = new QLineEdit(form);
    recipientInput->setObjectName("RecipientInput");
    layout->addWidget(recipientLabel);
    layout->addWidget(recipientInput);

    QLabel* sourceLabel = new QLabel("Source Address:", form);
    QLineEdit* sourceInput = new QLineEdit(form);
    sourceInput->setObjectName("SourceInput");
    layout->addWidget(sourceLabel);
    layout->addWidget(sourceInput);

    QLabel* destinationLabel = new QLabel("Destination Address:", form);
    QLineEdit* destinationInput = new QLineEdit(form);
    destinationInput->setObjectName("DestinationInput");
    layout->addWidget(destinationLabel);
    layout->addWidget(destinationInput);

    QPushButton* submitButton = new QPushButton("Submit", form);
    layout->addWidget(submitButton);

    connect(submitButton, &QPushButton::clicked, parent, [parent, senderInput, recipientInput, sourceInput, destinationInput] {
        // Vlaidation proccess
        if (senderInput->text().isEmpty()) {
            QMessageBox::warning(parent, "Validation Error", "Sender Name is required.");
            return;
        }
        if (recipientInput->text().isEmpty()) {
            QMessageBox::warning(parent, "Validation Error", "Recipient Name is required.");
            return;
        }
        if (sourceInput->text().isEmpty()) {
            QMessageBox::warning(parent, "Validation Error", "Source Address is required.");
            return;
        }
        if (destinationInput->text().isEmpty()) {
            QMessageBox::warning(parent, "Validation Error", "Destination Address is required.");
            return;
        }

        // Writte the new parcel to the database
        int32_t id = DataHandler::getInstance().writteParcel(senderInput->text().toStdString(),
                                                recipientInput->text().toStdString(),
                                                sourceInput->text().toStdString(),
                                                destinationInput->text().toStdString());

        QStackedWidget* stackedWidget = parent->findChild<QStackedWidget*>();
        if (!stackedWidget)
        {
            return;
        }

        // Create connection to automat
        ConnectionHandler connection;
        QWidget* connectionPage = connection.connectionGui(id, parent);
        connectionPage->setObjectName("ReceiveParcelPage");
        stackedWidget->addWidget(connectionPage);
        stackedWidget->setCurrentWidget(connectionPage);
        });

    return form;
}

// create a form for requesting automat repair 
QWidget* FormularHandler::createRepairTaskForm(QWidget* parent)
{
    // Create the gui elements
    QWidget* form = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(form);

    QLabel* reporterNameLabel = new QLabel("Reporter Name:", form);
    QLineEdit* reporterNameInput = new QLineEdit(form);
    reporterNameInput->setObjectName("ReporterInput");
    layout->addWidget(reporterNameLabel);
    layout->addWidget(reporterNameInput);

    QLabel* severityLabel = new QLabel("Severity:", form);
    QLineEdit* severityInput = new QLineEdit(form);
    severityInput->setObjectName("SourceInput");
    layout->addWidget(severityLabel);
    layout->addWidget(severityInput);

    QLabel* descriptionLabel = new QLabel("Description:", form);
    QLineEdit* descriptionInput = new QLineEdit(form);
    descriptionInput->setObjectName("DestinationInput");
    layout->addWidget(descriptionLabel);
    layout->addWidget(descriptionInput);

    QLabel* locationLabel = new QLabel("Location:", form);
    QLineEdit* locationInput = new QLineEdit(form);
    locationInput->setObjectName("DestinationInput");
    layout->addWidget(locationLabel);
    layout->addWidget(locationInput);

    QPushButton* submitButton = new QPushButton("Submit", form);
    layout->addWidget(submitButton);

    connect(submitButton, &QPushButton::clicked, parent, [=] {
        // Validate inputed data
        if (reporterNameInput->text().isEmpty()) {
            QMessageBox::warning(parent, "Validation Error", "Field required.");
            return;
        }
        if (severityInput->text().isEmpty()) {
            QMessageBox::warning(parent, "Validation Error", "Field required.");
            return;
        }
        if (descriptionInput->text().isEmpty()) {
            QMessageBox::warning(parent, "Validation Error", "Field required.");
            return;
        }
        if (locationInput->text().isEmpty()) {
            QMessageBox::warning(parent, "Validation Error", "Field required.");
            return;
        }

        QStackedWidget* stackedWidget = parent->findChild<QStackedWidget*>();
        if (!stackedWidget)
        {
            return;
        }

        DataHandler& dataHandler = DataHandler::getInstance();
        // Writte repair request to database
        int32_t repairId = dataHandler.writteRepair(reporterNameInput->text().toStdString(),
                                                                   severityInput->text().toStdString(),
                                                                   descriptionInput->text().toStdString(),
                                                                   locationInput->text().toStdString());
        // Create a task for the repair with referenced repairRequest data
        dataHandler.writteTask(repairId, std::string{}, "Operator", locationInput->text().toStdString(), DataHandler::TaskType::Repair);
        QString message = QString("Repair was requested!");
        QMessageBox::information(nullptr, "Repair Details", message);
        // Return to main menu
        stackedWidget->setCurrentIndex(ConnectionHandler::findWidget("OperatorMenu", parent));
        });

    return form;
}