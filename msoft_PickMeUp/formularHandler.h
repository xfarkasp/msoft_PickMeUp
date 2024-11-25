#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QString>

// Enum for different form types
enum class FormType {
    SendParcel,
    ReceiveParcel,
    OrderHistory,
    UserSettings
};

class FormularHandler : public QObject
{
public:
    // Constructor
    FormularHandler();

    // Factory method to get the desired form
    QWidget* getForm(FormType type, QWidget* parent = nullptr);

private:
    QWidget* createSendParcelForm(QWidget* parent);
    QWidget* createRepairTaskForm(QWidget* parent);
};