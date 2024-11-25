#include "FormularHandler.h"
#include "DataHandler.h"
#include <QStackedWidget>

// Constructor
FormularHandler::FormularHandler() {}

QWidget* FormularHandler::getForm(FormType type, QWidget* parent)
{
    switch (type)
    {
    case FormType::SendParcel:
        return createSendParcelForm(parent);
    default:
        return nullptr; // Handle invalid input
    }
}

QWidget* FormularHandler::createSendParcelForm(QWidget* parent) {
    QWidget* form = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(form);

    // Sender Name (Mandatory)
    QLabel* senderLabel = new QLabel("Sender Name:", form);
    QLineEdit* senderInput = new QLineEdit(form);
    senderInput->setObjectName("SenderInput");
    layout->addWidget(senderLabel);
    layout->addWidget(senderInput);

    // Recipient Name (Mandatory)
    QLabel* recipientLabel = new QLabel("Recipient Name:", form);
    QLineEdit* recipientInput = new QLineEdit(form);
    recipientInput->setObjectName("RecipientInput");
    layout->addWidget(recipientLabel);
    layout->addWidget(recipientInput);

    // Source Address (Mandatory)
    QLabel* sourceLabel = new QLabel("Source Address:", form);
    QLineEdit* sourceInput = new QLineEdit(form);
    sourceInput->setObjectName("SourceInput");
    layout->addWidget(sourceLabel);
    layout->addWidget(sourceInput);

    // Destination Address (Mandatory)
    QLabel* destinationLabel = new QLabel("Destination Address:", form);
    QLineEdit* destinationInput = new QLineEdit(form);
    destinationInput->setObjectName("DestinationInput");
    layout->addWidget(destinationLabel);
    layout->addWidget(destinationInput);

    // Submit Button
    QPushButton* submitButton = new QPushButton("Submit", form);
    layout->addWidget(submitButton);

    // Connect the Submit button to validation function
    connect(submitButton, &QPushButton::clicked, parent, [parent, senderInput, recipientInput, sourceInput, destinationInput] {
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
        DataHandler::getInstance().writteParcel(senderInput->text().toStdString(),
                                                recipientInput->text().toStdString(),
                                                sourceInput->text().toStdString(),
                                                destinationInput->text().toStdString());
        QStackedWidget* stackedWidget = parent->findChild<QStackedWidget*>();
        if (!stackedWidget) return;
        stackedWidget->setCurrentIndex(1);
        });

    return form;
}

