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
    ConnectionHandler() {};
    AutomatController connectAutomat() { return AutomatController{}; }
    std::string getMessage() { return "message from automat"; }
    QWidget* connectionGui(int32_t id, QWidget* parent = nullptr);  // This just create the gui page
    void sendPackageId(int32_t){} //not implemented
    static int findWidget(std::string widgetName, QWidget* parent); // This is just a gui helper function
};