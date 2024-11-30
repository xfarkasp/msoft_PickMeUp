#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QString>
#include "AutomatController.h"

class ConnectionHandler : public QObject
{
public:
    // Constructor
    ConnectionHandler() {};
    AutomatController connectAutomat() { return AutomatController{}; }
    std::string getMessage() { return "message from automat"; }
    QWidget* connectionGui(int32_t id, QWidget* parent = nullptr);  
};