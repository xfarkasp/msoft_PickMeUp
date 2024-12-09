#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QString>

enum class FormType {
    SendParcel,
    RequestRepair,
    Reclamation,
    ReclamationValidation
};

class FormularHandler : public QObject
{
public:
    FormularHandler(){}

    QWidget* getForm(int32_t objectId, FormType type, QWidget* parent=nullptr);

private:
    QWidget* createSendParcelForm(QWidget* parent);
    QWidget* createRepairTaskForm(QWidget* parent);
    QWidget* createReclamationForm(int32_t objectId, QWidget* parent);
    QWidget* createRecValidationForm(int32_t objectId, QWidget* parent);
};