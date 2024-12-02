#include "AutomatController.h"
#include "DataHandler.h"

bool AutomatController::recordPackage(int32_t id)
{
    DataHandler::Parcel& package = getPackgageData(id);
    DataHandler::ParcelSize size = package.getSize();

    if (!checkSpace(size))
    {
        return false;
    }

    openLocker();

    if (!isInserted())
    {
        return false;
    }

    package.setState(DataHandler::ParcelStatus::Sent);
    return true;
}