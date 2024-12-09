#include "FormularHandler.h"
#include "DataHandler.h"
#include <QStackedWidget>
#include <QTextEdit>
#include <QFileDialog>
#include "ConnectionHandler.h"

// Metod returns a form creation instance based on the type of the requested form
QWidget* FormularHandler::getForm(int32_t parcelId, FormType type, QWidget* parent)
{
    switch (type)
    {
    case FormType::SendParcel: // USE CASE 1: Odošli balík
        return createSendParcelForm(parent);
    case FormType::RequestRepair:// USE CASE 10: Požiadaj o opravu automatu.
        return createRepairTaskForm(parent);
    case FormType::Reclamation: // USE CASE 9 : Reklamuj balík
        return createReclamationForm(parcelId, parent);
    case FormType::ReclamationValidation: // USE CASE 9 : Reklamuj balík
        return createRecValidationForm(parcelId, parent);
    default:
        return nullptr;
    }
}

//---------------------------------------------------------------------------
// USE CASE 1: Odošli balík
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
        QWidget* connectionPage = connection.connectionGui(id, ConnectionHandler::ConnectionType::Creation, parent);
        connectionPage->setObjectName("ReceiveParcelPage");
        stackedWidget->addWidget(connectionPage);
        stackedWidget->setCurrentWidget(connectionPage);
        });

    return form;
}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------
// USE CASE 9 : Reklamuj balík
QWidget* FormularHandler::createReclamationForm(int32_t objectId, QWidget* parent)
{
    QWidget* reclamationTab = new QWidget();
    QVBoxLayout* reclamationLayout = new QVBoxLayout(reclamationTab);

    QLabel* reclamationDescLabel = new QLabel("Reclamation Description:", reclamationTab);
    QTextEdit* reclamationDescEdit = new QTextEdit(reclamationTab);
    reclamationDescEdit->setPlaceholderText("No description provided.");

    QLabel* reclamationPicLabel = new QLabel("Reclamation Picture:", reclamationTab);
    QLabel* reclamationPicPreview = new QLabel("No picture available.", reclamationTab);
    QPushButton* uploadImageButton = new QPushButton("Upload Image", reclamationTab);
    QPushButton* sendButton = new QPushButton("Send", reclamationTab);

    reclamationPicPreview->setAlignment(Qt::AlignCenter);
    reclamationPicPreview->setStyleSheet("border: 1px solid gray;");
    reclamationPicPreview->setFixedSize(200, 200);

    QString statusText{ "Reclamation Status: Pending" };
    DataHandler& dataHandler = DataHandler::getInstance();
    for (const auto& task : dataHandler.getTasks())
    {
        if (task.second.m_objectId == objectId && task.second.m_taskType == DataHandler::TaskType::Reclamation)
        {
            statusText = DataHandler::taskStatusToString(task.second.m_taskStatus);
            reclamationPicPreview->setPixmap(task.second.m_pixmap);
            reclamationDescEdit->setText(QString::fromStdString(task.second.m_description));
            break;
        }
    }
    QLabel* reclamationStatusLabel = new QLabel(statusText, reclamationTab);

    reclamationLayout->addWidget(reclamationDescLabel);
    reclamationLayout->addWidget(reclamationDescEdit);
    reclamationLayout->addWidget(reclamationPicLabel);
    reclamationLayout->addWidget(reclamationPicPreview);
    reclamationLayout->addWidget(uploadImageButton);
    reclamationLayout->addWidget(sendButton);
    reclamationLayout->addWidget(reclamationStatusLabel);

    QObject::connect(uploadImageButton, &QPushButton::clicked, this, [=]() {
        QString imagePath = QFileDialog::getOpenFileName(parent, "Select an Image", "", "Images (*.png *.xpm *.jpg *.jpeg)");
        if (!imagePath.isEmpty()) {
            QPixmap uploadedImage(imagePath);
            if (!uploadedImage.isNull()) {
                reclamationPicPreview->setPixmap(uploadedImage.scaled(reclamationPicPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                reclamationPicPreview->setText("");
            }
            else {
                QMessageBox::warning(parent, "Error", "The selected file could not be loaded as an image.");
            }
        }
        });
    
    QObject::connect(sendButton, &QPushButton::clicked, this, [=]()
        {

            QMessageBox::information(parent, "Information", "Reclamation sent.");
            reclamationStatusLabel->setVisible(true);
            DataHandler& dataHandler = DataHandler::getInstance();
            DataHandler::Parcel& parcel = dataHandler.getParcel(objectId);
            dataHandler.writteTask(objectId, "none", "Customer", "", DataHandler::TaskType::Reclamation, reclamationPicPreview->pixmap(), reclamationDescEdit->toPlainText().toStdString());
        });

    return reclamationTab;
}

//---------------------------------------------------------------------------
// USE CASE 9 : Reklamuj balík
QWidget* FormularHandler::createRecValidationForm(int32_t objectId, QWidget* parent)
{
    // Create dialog and layout
    QDialog* reclamationDialog = new QDialog(parent); // Set MainWindow as parent
    QVBoxLayout* mainLayout = new QVBoxLayout(reclamationDialog);
    reclamationDialog->setWindowTitle("Order Dialog");

    DataHandler& dataHandler = DataHandler::getInstance();
    DataHandler::Task& task = dataHandler.getTask(objectId);

    QWidget* reclamationTab = new QWidget();
    QVBoxLayout* reclamationLayout = new QVBoxLayout(reclamationTab);

    QLabel* reclamationDescLabel = new QLabel("Reclamation Description:", reclamationTab);
    QTextEdit* reclamationDescEdit = new QTextEdit(reclamationTab);
    reclamationDescEdit->setPlaceholderText(QString::fromStdString(task.m_description));

    QLabel* reclamationPicLabel = new QLabel("Reclamation Picture:", reclamationTab);
    QLabel* reclamationPicPreview = new QLabel("No picture available.", reclamationTab);
    reclamationPicPreview->setPixmap(task.m_pixmap.scaled(reclamationPicPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    reclamationPicPreview->setAlignment(Qt::AlignCenter);
    reclamationPicPreview->setStyleSheet("border: 1px solid gray;");
    reclamationPicPreview->setFixedSize(200, 200);

    QString statusText{ "Reclamation Status: Pending" };
    QLabel* reclamationStatusLabel = new QLabel(statusText, reclamationTab);
    for (const auto& task : dataHandler.getTasks())
    {
        if (task.second.m_objectId == objectId && task.second.m_taskType == DataHandler::TaskType::Reclamation)
        {
            statusText = DataHandler::taskStatusToString(task.second.m_taskStatus);
            break;
        }
    }
    reclamationStatusLabel->setText(statusText);

    QPushButton* acceptButton = new QPushButton("Accept", reclamationTab);
    QPushButton* denyButton = new QPushButton("Deny", reclamationTab);

    reclamationLayout->addWidget(reclamationDescLabel);
    reclamationLayout->addWidget(reclamationDescEdit);
    reclamationLayout->addWidget(reclamationPicLabel);
    reclamationLayout->addWidget(reclamationPicPreview);
    reclamationLayout->addWidget(reclamationStatusLabel);
    reclamationLayout->addWidget(acceptButton);
    reclamationLayout->addWidget(denyButton);

    QObject::connect(acceptButton, &QPushButton::clicked, this, [=, &task]()
        {
            task.m_taskStatus = DataHandler::TaskStatus::AcceptedReclamation;
            QMessageBox::information(parent, "Information", "Request accepted.");
            reclamationStatusLabel->setText(DataHandler::taskStatusToString(task.m_taskStatus));
        });
    
    QObject::connect(denyButton, &QPushButton::clicked, this, [=, &task]()
        {
            task.m_taskStatus = DataHandler::TaskStatus::DeniedReclamation;
            QMessageBox::information(parent, "Information", "Request denied.");
            reclamationStatusLabel->setText(DataHandler::taskStatusToString(task.m_taskStatus));
        });

   
    // Add tab widget to the main layout
    mainLayout->addWidget(reclamationTab);

    // Execute dialog
    reclamationDialog->exec();

    return reclamationTab;
}

//---------------------------------------------------------------------------