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
    RequestRepair
};

class FormularHandler : public QObject
{
public:
    FormularHandler();

    QWidget* getForm(FormType type, QWidget* parent = nullptr);

private:
    QWidget* createSendParcelForm(QWidget* parent);
    QWidget* createRepairTaskForm(QWidget* parent);
};