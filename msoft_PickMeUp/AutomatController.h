#pragma once
#include <vector>
#include "DataHandler.h"
#include "Locker.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QString>

class AutomatController
{
public:

    AutomatController() { m_lockers.emplace_back(Locker{0, false, 0, LockerSize::BIG}); }
    bool checkSpace(DataHandler::ParcelSize size) { return true; }
    bool recordPackage(int32_t id);
    void openLocker(){}
    bool isInserted() { return true; }
    DataHandler::Parcel getPackgageData(int32_t id) { return DataHandler::getInstance().getParcel(id); }

private:
    std::vector<Locker> m_lockers;
};
