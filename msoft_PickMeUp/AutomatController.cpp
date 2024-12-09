#include "AutomatController.h"
#include "DataHandler.h"

// USE CASE 1: Odošli balík a USE CASE 2: Doruč nový balík
// Method to record new package to be delivered
// See seqDiagram recordPackage
bool AutomatController::recordPackage(int32_t id)
{
    // Get package info from DB
    DataHandler::Parcel& package = getPackgageData(id);
    // Check if package fits
    DataHandler::ParcelSize size = package.getSize();
    if (!checkSpace(size))
    {
        return false;
    }
    // Open free locker, where package fits(logic not implemented)
    openLocker();
    // Validate insertion(logic not implemented, return true)
    if (!isInserted())
    {
        return false;
    }
    // Sets the state of the package to sent which indicate
    package.setState(DataHandler::ParcelStatus::Sent);
    return true;
}

//---------------------------------------------------------------------------